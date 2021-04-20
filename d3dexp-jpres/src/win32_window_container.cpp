#include "win32_window_container.h"


namespace d3dexp
{
	LRESULT win32_window_container::window_proc(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CHAR:
		{
			auto letter = static_cast<unsigned char>(wparam);
			return 0;
		}
		case WM_KEYDOWN:
		{
			auto keycode = static_cast<unsigned char>(wparam);
			return 0;
		}
		default:
		{
			return DefWindowProc(window_h, msg, wparam, lparam);
		}
		}
	}
}