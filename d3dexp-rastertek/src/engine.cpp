#include "engine.h"

namespace d3dexp
{
	engine::engine(std::wstring const& app_name, win32_graphics_settings_t& settings) noexcept
		: m_window(app_name, settings)
	{}

	int engine::run() noexcept
	{
		auto msg = MSG{};
		auto is_done = false;

		while (!is_done)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT)
			{
				is_done = true;
			}
			else
			{
				if (!frame())
				{
					is_done = true;
				}
			}
		}

		return 0;
	}

	bool engine::frame() noexcept
	{
		// exit application on user pressing Esc key
		if (m_window.input().is_key_pressed(VK_ESCAPE))
		{
			return false;
		}
		
		// process graphics frame
		if (!m_window.graphics().frame())
		{
			return false;
		}

		return true;
	}
}