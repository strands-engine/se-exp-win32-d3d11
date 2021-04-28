#include "win32_window.h"

#include <string>
#include <stdexcept>

#include "win32_app.h"

namespace d3dexp::bell0bytes
{
	win32_window* s_main_window = nullptr;

	LRESULT CALLBACK main_wnd_proc(HWND window_h, unsigned int msg, WPARAM wparam, LPARAM lparam)
	{
		return s_main_window->wnd_proc(window_h, msg, wparam, lparam);
	}


	win32_window::win32_window(win32_app* app_p) : m_app_p(app_p)
	{
		s_main_window = this;

		auto init_result = initialize();
		if (!init_result)
		{
			try
			{
				init_result.throw_if_failed();
			}
			catch (std::exception& e)
			{
				auto msg = std::string{ "Win32 window initialization failed: " + std::string{e.what()} + "\n" };
				OutputDebugStringA(msg.c_str());

				throw std::runtime_error("Window creation failed");
			}

		}
	}

	expected_t<void> win32_window::initialize()
	{
		// specify the window class description
		auto wc = WNDCLASSEX{};

		wc.cbClsExtra = 0;										// no extra bytes needed
		wc.cbSize = sizeof(WNDCLASSEX);							// size of the window description structure
		wc.cbWndExtra = 0;										// no extra bytes needed
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// brush to repaint the background with
		wc.hCursor = LoadCursor(0, IDC_ARROW);					// load the standard arrow cursor
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);				// load the standard application icon
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);				// load the standard small application icon
		wc.hInstance = m_app_p->m_instance_h;					// handle to the core application instance
		wc.lpfnWndProc = main_wnd_proc;							// window procedure function
		wc.lpszClassName = L"D3DEXP bell0bytes";				// class name
		wc.lpszMenuName = 0;									// no menu
		wc.style = CS_HREDRAW | CS_VREDRAW;						// send WM_SIZE message when either the height or the width of the client area are changed

		// register window class
		if (!RegisterClassEx(&wc))
		{
			return std::runtime_error("The window class could not be registered.");
		}

		// create the window
		m_window_h = CreateWindowEx(
			WS_EX_OVERLAPPEDWINDOW,				// overlapped window - with all shiny thingies
			wc.lpszClassName, 
			wc.lpszClassName,
			WS_OVERLAPPEDWINDOW, 
			CW_USEDEFAULT,						// leave system decision of initial placement position and size
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			NULL, 
			NULL, 
			m_app_p->m_instance_h, 
			NULL);

		if (!m_window_h)
		{
			return std::runtime_error("The window could not be created.");
		}

		// show and update the windows - only update - no set foreground/focus
		ShowWindow(m_window_h, SW_SHOW);
		UpdateWindow(m_window_h);

		// log and return success
		OutputDebugStringA("The main window was successfully created.\n");
		return {};
	}

	LRESULT win32_window::wnd_proc(HWND window_h, unsigned int msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{

		case WM_DESTROY:
		{
			// window is flagged to be destroyed - send a quit message
			OutputDebugStringA("Win32 window was flagged for destruction.\n");
			PostQuitMessage(0);
			return 0;
		}
		}

		// let Windows handle other messages
		return DefWindowProc(window_h, msg, wparam, lparam);
	}

}
