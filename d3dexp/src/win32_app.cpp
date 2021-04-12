#include "win32_app.h"

#include <sstream>

#include "win32_exception.h"

namespace d3dexp
{
	win32_app::win32_app() : m_window(800, 600, "D3DEXP Main Window") {}

	int win32_app::run()
	{
		// main application loop
		auto msg = MSG{};
		auto result = BOOL{};
		while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			frame();
		}

		if (result == -1)
		{
			RAISE_WIN32_LAST_ERROR();
		}

		return static_cast<int>(msg.wParam);
	}

	void win32_app::frame()
	{
		if (m_window.keyboard().is_key_pressed(d3dexp::key_t::spacebar))
		{
			MessageBox(nullptr, "Awww!!!", "Space Bar was pressed!", MB_OK);
		}

		while (!m_window.mouse().is_queue_empty())
		{
			const auto event = m_window.mouse().read();
			if (event.action() == d3dexp::win32_mouse::win32_mouse_event::action_t::leave)
			{
				m_window.set_title("DEBUG: Mouse released from capture.");
			}
			else if (event.action() == d3dexp::win32_mouse::win32_mouse_event::action_t::move)
			{
				std::ostringstream oss;
				oss << "DEBUG: Mouse position: (" << event.x_pos() << ", " << event.y_pos() << ")";
				m_window.set_title(oss.str());
			}
		}
	}
}
