#include "win32_window.h"

#include <windowsx.h>
#include "win32_key_code_converter.h"
#include "win32_debug_message_provider.h"
#include "win32_exception.h"

#include "../resource.h"

namespace d3dexp
{

	// win32_window::win32_window_class implementation

	win32_window::win32_window_class win32_window::win32_window_class::s_instance = win32_window::win32_window_class{};

	win32_window::win32_window_class::win32_window_class() noexcept
		: m_app_instance_h( GetModuleHandle( nullptr ) )
	{
		// newly registered window class settings struct 
		WNDCLASSEX wc = {};

		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = handle_message_setup;

		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;

		wc.hInstance = instance();

		wc.hIcon = load_icon_resource(IDI_ICON1, 32);
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;

		wc.lpszClassName = name();

		wc.hIconSm = load_icon_resource(IDI_ICON1, 16);

		RegisterClassEx(&wc);
		// error handling - currently skipped due to the way the singleton instance is resolved
	}

	win32_window::win32_window_class::~win32_window_class() noexcept
	{
		UnregisterClass(name(), instance());
	}

	// win32_window implementation

	win32_window::win32_window(int width, int height, char const* title)
		: m_title(title), m_cli_width(width), m_cli_height(height)
	{
		// choices for window style
		const DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

		// prepare window size based on desired client region size
		RECT wnd_rect = {};
		wnd_rect.left = 100;
		wnd_rect.right = m_cli_width + wnd_rect.left;
		wnd_rect.top = 100;
		wnd_rect.bottom = m_cli_height + wnd_rect.top;
		auto is_ok = AdjustWindowRect(&wnd_rect, style, FALSE);
		if (!is_ok)
		{
			RAISE_WIN32_LAST_ERROR();
		}

		// store adjusted window size
		m_wnd_width = wnd_rect.right - wnd_rect.left;
		m_wnd_height = wnd_rect.bottom - wnd_rect.top;

		// create win32 window instance and store its handle
		const auto main_window_title = "D3DEXP";

		m_window_h = CreateWindow( win32_window_class::name(), title, style,
			CW_USEDEFAULT, CW_USEDEFAULT, m_wnd_width, m_wnd_height,
			nullptr, nullptr, win32_window_class::instance(), this);
		if (m_window_h == nullptr)
		{
			RAISE_WIN32_LAST_ERROR();
		}

		// show created window instance
		ShowWindow(m_window_h, SW_SHOWDEFAULT);

		// initialize D3D graphics object using handle to that window
		m_graphics_p = std::make_unique<d3d11_graphics>(m_window_h);
	}

	win32_window::~win32_window() noexcept
	{
		DestroyWindow(m_window_h);
	}

	d3d11_graphics& win32_window::graphics()
	{
		if (!m_graphics_p)
		{
			RAISE_NO_GRAPHICS_ERROR();
		}
		return *m_graphics_p;
	}

	d3d11_graphics const& win32_window::graphics() const
	{
		if (!m_graphics_p)
		{
			RAISE_NO_GRAPHICS_ERROR();
		}
		return *m_graphics_p;
	}

	void win32_window::set_title(std::string const& title)
	{
		const auto is_ok = SetWindowText(m_window_h, title.c_str());
		if (!is_ok)
		{
			RAISE_WIN32_LAST_ERROR();
		}
		m_title = title;
	}

	std::optional<int> win32_window::process_messages()
	{
		auto msg = MSG{};

		// consume messages as long as they are in queue
		// NOTE: check PeekMessage error handling
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// if quit message is encountered, return optional return code
			if (msg.message == WM_QUIT)
			{
				return static_cast<int>(msg.wParam);
			}

			// if other messages are present, translate them (to spawn WM_CHAR messages) and dispatch

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// return empty optional if there are no more messages (and we are not quitting)
		return {};
	}

	LRESULT CALLBACK win32_window::handle_message_setup(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept
	{
		// use lparam given during window instance creation to store the pointer to actual win32_window class instance
		if (msg_id == WM_NCCREATE)
		{
			// get pointer to win32_window instance from creation data
			const auto create_data_p = reinterpret_cast<CREATESTRUCTW*>(lparam);
			const auto window_p = static_cast<win32_window*>(create_data_p->lpCreateParams);

			// set managed user data to store pointer to win32_window class instance
			SetWindowLongPtr(wnd_h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window_p));

			// set message handler to handler defined in given win32_window class instance
			SetWindowLongPtr(wnd_h, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&win32_window::handle_message_adapter));

			// forward message to window class handler
			return window_p->handle_message(wnd_h, msg_id, wparam, lparam);
		}

		// handle any messages before WM_NCCREATE using default handler
		return DefWindowProc(wnd_h, msg_id, wparam, lparam);
	}

	LRESULT CALLBACK win32_window::handle_message_adapter(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept
	{
		// obtain pointer to respective win32_window class instance and call its handler method
		const auto window_p = reinterpret_cast<win32_window*>(GetWindowLongPtr(wnd_h, GWLP_USERDATA));
		return window_p->handle_message(wnd_h, msg_id, wparam, lparam);
	}

	LRESULT win32_window::handle_message(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept
	{
#ifdef _DEBUG
		static d3dexp::win32_debug_message_provider s_msg;
#endif // _DEBUG

		static d3dexp::win32_key_code_converter s_kc_conv;

		switch (msg_id)
		{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// KEYBOARD AND MOUSE INPUT HANDLING

		case WM_KILLFOCUS:
		{
			// prevent not clearing keyboard state when window focus changes
			keyboard().clear_state();
			break;
		}

		case WM_KEYDOWN:
		{
			if (!(lparam & 0x40000000) || keyboard().is_autorepeat_enabled())
			{
#ifdef _DEBUG
				OutputDebugString(s_msg.for_key_event("key down", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
				keyboard().on_key_pressed(s_kc_conv.convert(wparam, lparam));
			}
			break;
		}

		case WM_KEYUP:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_key_event("key up", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
			keyboard().on_key_released(s_kc_conv.convert(wparam, lparam));
			break;
		}

		case WM_SYSKEYDOWN:
		{
			if (!(lparam & 0x40000000) || keyboard().is_autorepeat_enabled())
			{
#ifdef _DEBUG
				OutputDebugString(s_msg.for_key_event("sys key down", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
				keyboard().on_key_pressed(s_kc_conv.convert(wparam, lparam));
			}
			break;
		}

		case WM_SYSKEYUP:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_key_event("sys key up", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
			keyboard().on_key_released(s_kc_conv.convert(wparam, lparam));
			break;
		}

		case WM_CHAR:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_char_event(static_cast<char>(wparam)).c_str());
#endif // _DEBUG
			keyboard().on_char(static_cast<char>(wparam));
			break;
		}

		case WM_LBUTTONDOWN:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("left btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::lmb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_lmb_pressed(pts.x, pts.y);
			break;
		}

		case WM_LBUTTONUP:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("left btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::lmb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_lmb_released(pts.x, pts.y);
			break;
		}

		case WM_RBUTTONDOWN:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("right btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::rmb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_rmb_pressed(pts.x, pts.y);
			break;
		}

		case WM_RBUTTONUP:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("right btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::rmb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_rmb_released(pts.x, pts.y);
			break;
		}

		case WM_MBUTTONDOWN:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("mid btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::mmb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_mmb_pressed(pts.x, pts.y);
			break;
		}

		case WM_MBUTTONUP:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("mid btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::mmb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_mmb_released(pts.x, pts.y);
			break;
		}

		case WM_XBUTTONDOWN:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("x btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::x1mb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_xmb_pressed(pts.x, pts.y);
			break;
		}

		case WM_XBUTTONUP:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("x btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::x1mb).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_xmb_released(pts.x, pts.y);
			break;
		}

		case WM_MOUSEWHEEL:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_wheel_event(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), GET_WHEEL_DELTA_WPARAM(wparam)).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);
			mouse().on_wheel_delta(pts.x, pts.y, GET_WHEEL_DELTA_WPARAM(wparam));
			break;
		}

		case WM_MOUSEMOVE:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_mouse_event("mouse move", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)).c_str());
#endif // _DEBUG
			const POINTS pts = MAKEPOINTS(lparam);

			// mouse is in client region
			if (pts.x >= 0 && pts.x < width() && pts.y >= 0 && pts.y < height())
			{
				// raise move event 
				mouse().on_move(pts.x, pts.y);
				
				// if previously outside the window - capture and raise enter event
				if (!mouse().is_in_window())
				{
					SetCapture(m_window_h);
					mouse().on_enter();
				}
			}

			// mouse is outside client region
			else 
			{
				// if any mouse button is pressed, then maintain capture 
				if (wparam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON | MK_XBUTTON1 | MK_XBUTTON2))
				{
					mouse().on_move(pts.x, pts.y);
				}

				// otherwise, release capture and raise leave event
				else
				{
					ReleaseCapture();
					mouse().on_leave();
				}
			}
			break;
		}

		default:
		{
#ifdef _DEBUG
			OutputDebugString(s_msg.for_generic_event(msg_id, wparam, lparam).c_str());
#endif // _DEBUG
			break;
		}
		}
		return DefWindowProc(wnd_h, msg_id, wparam, lparam);

	}

	HICON win32_window::win32_window_class::load_icon_resource(int resource_id, int size) noexcept
	{
		return static_cast<HICON>(LoadImage(instance(), MAKEINTRESOURCE(resource_id), IMAGE_ICON, size, size, 0));
	}

}