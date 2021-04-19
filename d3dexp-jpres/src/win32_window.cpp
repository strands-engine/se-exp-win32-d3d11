#include "win32_window.h"

#include "string_converter.h"
#include "error_logger.h"

namespace d3dexp
{
	win32_window::~win32_window() noexcept
	{
		if (is_initialized())
		{
			DestroyWindow(m_window_h);
			unregister_window_class();
		}
	}

	bool win32_window::initialize(HINSTANCE instance_h, std::string title, std::string class_name, int width, int height) noexcept
	{
		m_instance_h = instance_h;

		m_title = std::move(title);
		m_title_w = string_converter::to_wide(m_title);
		m_class_name = std::move(class_name);
		m_class_name_w = string_converter::to_wide(m_class_name);

		m_width = width;
		m_height = height;

		register_window_class();

		// window creation
		m_window_h = CreateWindowEx(
			0,                                         // extended window style flags (left default)
			m_class_name_w.c_str(),                    // name of window class that instane we are creating
			m_title_w.c_str(),                         // name of window instance that appears as its title
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,  // window style flags - enable minimization icon, right-click context enu and title caption 
			0,                                         // X position of window's top left corner
			0,                                         // Y position of window's top left corner
			m_width,                                   // window width
			m_height,                                  // window height
			NULL,                                      // handle to parent window of this window (here none, as it is main window)
			NULL,                                      // handle to resource representing window menu (unused)
			m_instance_h,                              // handle to the instance of application owning that window
			nullptr                                    // pointer to extra data passed to window (currently unused)
		);

		// checking and handling errors in window creation
		if (!is_initialized())
		{
			error_logger::log(GetLastError(), std::string("Failed to create window: ") + m_title);
			return false;
		}

		// show window, set as foreground and bring focus to it
		ShowWindow(m_window_h, SW_SHOW);
		SetForegroundWindow(m_window_h);
		SetFocus(m_window_h);

		return true;
	}

	bool win32_window::process_messages() noexcept
	{
		auto msg = MSG{};
		if (PeekMessage(&msg, m_window_h, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// checking if window was closed
		// NOTE: using WM_NULL vs WM_QUIT to check for app termination to be able to handle multiple windows
		if (msg.message == WM_NULL)
		{
			if (!IsWindow(m_window_h))
			{
				// message loop handles destroying window (just reset handle for proper functioning of object)
				m_window_h = NULL;
				unregister_window_class();
				return false;
			}
		}

		return true;
	}

	void win32_window::register_window_class() noexcept
	{
		auto wc = WNDCLASSEX{};
		
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // style flags - make window redraw on width/length changes due to move or resize; also it owns its own gfx context
		wc.lpfnWndProc = DefWindowProc;                // pointer to function processing window's messages (currently default one)
		wc.cbClsExtra = 0;                             // bytes reserved after window class object, unused
		wc.cbWndExtra = 0;                             // bytes reserved after window instance object, unused
		wc.hInstance = m_instance_h;                   // handle to current instance of running application
		wc.hIcon = NULL;                               // handle to window icon resource (left default)
		wc.hIconSm = NULL;                             // handle to window icon resource, small version (left default)
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // handle to window cursor resource (loading default system arrow)
		wc.hbrBackground = NULL;                       // handle to brush used to render window background (left null as d3d will be used for rendering entire window space)
		wc.lpszMenuName = NULL;                        // name of window menu (unused)
		wc.lpszClassName = m_class_name_w.c_str();     // name of class being registered
		wc.cbSize = sizeof(WNDCLASSEX);                // size of this config structure

		// registering window class
		RegisterClassEx(&wc);
	}

	void win32_window::unregister_window_class() noexcept
	{
		UnregisterClass(m_class_name_w.c_str(), m_instance_h);
	}
}