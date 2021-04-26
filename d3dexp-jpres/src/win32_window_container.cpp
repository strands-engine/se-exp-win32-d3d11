#include "win32_window_container.h"

#include <memory>

#include "error_logger.h"

namespace d3dexp::jpres
{
	win32_window_container::win32_window_container() noexcept
	{
		static bool is_raw_input_initialized = false;
		if (!is_raw_input_initialized)
		{
			auto rid = RAWINPUTDEVICE{};

			rid.usUsagePage = 1u;
			rid.usUsage = 2u;       // both define mouse as device of interest
			rid.dwFlags = 0u;       // no extra options
			rid.hwndTarget = NULL;  // not only for current window

			if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
			{
				error_logger::log(GetLastError(), "Failed to register mouse for raw input collection.");
				exit(-1);
			}
			is_raw_input_initialized = true;
		}
	}

	LRESULT win32_window_container::window_proc(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{

		// KEYBOARD INPUT
		case WM_CHAR:
		{
			const auto was_pressed = lparam & 0x40000000u;
			if (!was_pressed || m_keyboard.is_char_autorepeat_enabled())
			{
				m_keyboard.on_char(static_cast<unsigned char>(wparam));
			}
			return 0;
		}
		case WM_KEYDOWN:
		{
			const auto was_pressed = lparam & 0x40000000u;
			if (!was_pressed || m_keyboard.is_key_autorepeat_enabled())
			{
				m_keyboard.on_key_pressed(static_cast<unsigned char>(wparam));
			}
			return 0;
		}
		case WM_KEYUP:
		{
			m_keyboard.on_key_released(static_cast<unsigned char>(wparam));
			return 0;
		}

		// MOUSE INPUT
		case WM_LBUTTONDOWN:
		{
			m_mouse.on_lmb_pressed(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}
		case WM_LBUTTONUP:
		{
			m_mouse.on_lmb_released(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			m_mouse.on_rmb_pressed(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}
		case WM_RBUTTONUP:
		{
			m_mouse.on_rmb_released(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			m_mouse.on_mmb_pressed(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}
		case WM_MBUTTONUP:
		{
			m_mouse.on_mmb_released(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			const auto delta = GET_WHEEL_DELTA_WPARAM(wparam);
			if (delta > 0)
			{
				m_mouse.on_wheel_up(LOWORD(lparam), HIWORD(lparam));
			}
			else if (delta < 0)
			{
				m_mouse.on_wheel_down(LOWORD(lparam), HIWORD(lparam));
			}
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			m_mouse.on_move(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}

		// RAW MOUSE INPUT
		case WM_INPUT:
		{
			// NOTE: can be differentiated if window is in background or not (see docs)
			// NOTE: optionally GetRawInputBuffer may be used instead of GetRawInputData (did not work for jpres; see docs)
			
			// obtain raw input struct size
			auto size = UINT{};
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

			// obtain raw input struct
			if (size > 0)
			{
				auto data = std::make_unique<BYTE[]>(size);				
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, data.get(), &size, sizeof(RAWINPUTHEADER)) == size)
				{
					// processing raw input data if properly obtained
					const auto raw_input_p = reinterpret_cast<RAWINPUT*>(data.get());
					if (raw_input_p->header.dwType == RIM_TYPEMOUSE)
					{
						m_mouse.on_move_raw(raw_input_p->data.mouse.lLastX, raw_input_p->data.mouse.lLastY);
					}
				}
			}

			return DefWindowProc(window_h, msg, wparam, lparam);
		}

		// OTHER
		default:
		{
			return DefWindowProc(window_h, msg, wparam, lparam);
		}
		}
	}
}