#include <sstream>

#include "win32_header_wrapper.h"
#include "win32_window.h"
#include "d3d_exception.h"


int CALLBACK WinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPSTR argv, _In_ int cmd_show)
{
	try
	{

		// new window creation
		auto window = d3dexp::win32_window(800, 600, "D3DEXP Main Window");

		// main loop
		auto msg = MSG{};
		auto result = BOOL{};
		while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (window.keyboard().is_key_pressed(d3dexp::key_t::spacebar))
			{
				MessageBox(nullptr, "Awww!!!", "Space Bar was pressed!", MB_OK);
			}

			while (!window.mouse().is_queue_empty())
			{
				const auto event = window.mouse().read();
				if (event.action() == d3dexp::win32_mouse::win32_mouse_event::action_t::leave)
				{
					window.set_title("DEBUG: Mouse released from capture.");
				}
				else if (event.action() == d3dexp::win32_mouse::win32_mouse_event::action_t::move)
				{
					std::ostringstream oss;
					oss << "DEBUG: Mouse position: (" << event.x_pos() << ", " << event.y_pos() << ")";
					window.set_title(oss.str());
				}
			}
		}

		return result < 0 ? result : static_cast<int>(msg.wParam);
	}
	catch (d3dexp::d3d_exception const& e)
	{
		MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONERROR);
	}
	catch (std::exception const& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, "No error details available.", "Unknown Exception", MB_OK | MB_ICONERROR);
	}

	return -1;
}