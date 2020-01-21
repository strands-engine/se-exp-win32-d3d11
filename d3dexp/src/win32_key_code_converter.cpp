// imported from old strands code

#include "win32_key_code_converter.h"

namespace d3dexp
{
	key_t win32_key_code_converter::convert(WPARAM wparam, const LPARAM lparam) const noexcept
	{
		bool is_extended = (lparam & 0x01000000) != 0;

		switch (wparam)
		{
		case VK_SHIFT:
			wparam = MapVirtualKey((lparam & 0x00FF0000) >> 16, MAPVK_VSC_TO_VK_EX);
			break;

		case VK_CONTROL:
			wparam = is_extended ? VK_RCONTROL : VK_LCONTROL;
			break;

		case VK_MENU:
			wparam = is_extended ? VK_RMENU : VK_LMENU;
			break;

		case VK_RETURN:
			wparam = is_extended ? 0xFF : VK_RETURN;
			break;

		default:
			break;
		}

		return static_cast<key_t>(WIN32VK_TO_KEY[wparam]);
	}

}