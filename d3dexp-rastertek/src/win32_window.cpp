#include "win32_window.h"

namespace d3dexp
{
    win32_window::win32_window(std::wstring const& title, d3d11_graphics_settings_t& settings) noexcept
        : m_title(title), m_width(settings.width), m_height(settings.height), m_is_fullscreen(settings.is_fullscreen)
    {
		// set global pointer to window class
		g_window_p = this;
		
		// get application handle
		m_instance_h = GetModuleHandle(NULL);
		
		// setup windows class settings
		auto wc = WNDCLASSEX{};
		
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = wnd_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_instance_h;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);        // w/o d3d necessary to see borderless window (with WS_POPUP style)
		wc.lpszMenuName = NULL;
		wc.lpszClassName = m_title.c_str();
		wc.cbSize = sizeof(WNDCLASSEX);

		// register the window class (no error checking!)
		RegisterClassEx(&wc);

		// obtain current screen dimensions
		auto screen_width = GetSystemMetrics(SM_CXSCREEN);
		auto screen_height = GetSystemMetrics(SM_CYSCREEN);

		// setup screen settings depending on whether mode is set to be windowed or fullscreen
		if (m_is_fullscreen)
		{
			// setup device mode settings struct
			auto dm = DEVMODE{};

			dm.dmSize = sizeof(DEVMODE);
			dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			dm.dmBitsPerPel = DWORD{ 32ul };
			dm.dmPelsWidth = static_cast<DWORD>(screen_width);
			dm.dmPelsHeight = static_cast<DWORD>(screen_height);

			// change display settings to fullscreen
			ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

			// set window size to screen size
			m_width = screen_width;
			m_height = screen_height;

			// update window dimensions in settings struct
			settings.width = m_width;
			settings.height = m_height;
		}

		// adjust position and dimensions for windowed mode
		else
		{
			// position window in the center of the screen
			m_pos_x = (screen_width - m_width) / 2;
			m_pos_y = (screen_height - m_height) / 2;
		}

		// create window (no error checking)
		m_window_h = CreateWindowEx(
			WS_EX_APPWINDOW,                                // extended style used (APPWINDOW - ??? - unnecessary or useful for borderless (w/WS_POPUP))
			m_title.c_str(),
			m_title.c_str(),
			WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,   // window styles used (??? - for parent window) - omitted WS_POPUP (makes window invisible (NOTE: WS_BORDER for borderless ?? + get rid of caption - WS_POPUP?)
			m_pos_x,
			m_pos_y,
			m_width,
			m_height,
			NULL,
			NULL,
			m_instance_h,
			NULL);

		// show window, bring it to foreground and give it focus
		ShowWindow(m_window_h, SW_SHOW);
		SetForegroundWindow(m_window_h);
		SetFocus(m_window_h);

		// hide the mouse cursor if in fullscreen mode
		if (m_is_fullscreen)
		{
			ShowCursor(false);
		}

		// initialize graphics
		m_graphics_p = std::make_unique<win32_graphics>(m_window_h, settings);
    }

    win32_window::~win32_window() noexcept 
    {
		// if leaving fullscreen mode, reset display settings and show back mouse cursor
		if (m_is_fullscreen)
		{
			ShowCursor(true);
			ChangeDisplaySettings(NULL, 0);
		}

		// destroy the window and unregister its class
		DestroyWindow(m_window_h);
		UnregisterClass(m_title.c_str(), m_instance_h);

		// clear global pointer to this window class
		g_window_p = nullptr;
    }

	LRESULT CALLBACK win32_window::message_handler(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
		{
			input().on_key_pressed(static_cast<unsigned char>(wparam));
			return 0;
		}
		case WM_KEYUP:
		{
			input().on_key_released(static_cast<unsigned char>(wparam));
			return 0;
		}
		default:
		{
			return DefWindowProc(window_h, msg, wparam, lparam);
		}
		}
	}

	LRESULT wnd_proc(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return g_window_p->message_handler(window_h, msg, wparam, lparam);
		}
		}
	}

}