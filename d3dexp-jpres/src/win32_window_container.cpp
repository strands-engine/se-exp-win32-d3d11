#include "win32_window_container.h"


namespace d3dexp
{
	LRESULT win32_window_container::window_proc(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
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
		default:
		{
			return DefWindowProc(window_h, msg, wparam, lparam);
		}
		}
	}
}