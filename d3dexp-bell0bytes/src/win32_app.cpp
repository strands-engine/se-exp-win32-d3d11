#include "win32_app.h"

#include <stdexcept>

namespace d3dexp::bell0bytes
{
	win32_app::~win32_app()
	{
		shutdown();
	}

	expected_t<int> win32_app::run()
	{
		auto is_running = true;
		auto msg = MSG{};

		// main loop
		while (is_running)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					is_running = false;
				}
			}
		}
		return static_cast<int>(msg.wParam);
	}

	expected_t<void> win32_app::initialize()
	{
		try
		{
			m_window_p = std::make_unique<win32_window>(this);
		}
		catch (std::runtime_error&)
		{
			return std::runtime_error("Failed to create main window.");
		}

		OutputDebugStringA("Win32 app successfully initialized.\n");
		return {};
	}

	void win32_app::shutdown(expected_t<void>* expected)
	{
		OutputDebugStringA("Win32 app successfully shutdown.\n");
	}
}
