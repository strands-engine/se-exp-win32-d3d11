#include "app.h"

#include <string>

namespace d3dexp::bell0bytes
{
	expected_t<void> app::initialize()
	{
		auto init_result = win32_app::initialize();
		if (!init_result)
		{
			return init_result;
		}
		OutputDebugStringA("App successfully initialized.\n");

		return {};
	}

	void app::shutdown(expected_t<void>* expected)
	{
		if (expected != nullptr && !expected->is_valid())
		{
			try
			{
				expected->throw_if_failed();
			}
			catch (std::exception& e)
			{
				auto msg = std::string{ "Exception thrown in app: " } + e.what() + "\n";
				OutputDebugStringA(msg.c_str());
			}
		}

		OutputDebugStringA("App shutdown successful.\n");
	}

	void app::update(double dt)
	{
		win32_app::update(dt);
	}

	expected_t<int> app::render(double farseer)
	{
		// clear buffers to prepare for drawing new frame
		m_graphics_p->clear_buffers();

		// rendering

		// display fps info
		auto result = m_graphics_2d_p->render_text(d2d1_graphics::context_t::fps, d2d1_graphics::colour_t::black);
		if (!result)
		{
			return std::runtime_error{ "Failed to write fps information!" };
		}

		// presents drawn frame to front buffer
		return win32_app::render(farseer);
	}

	void app::on_key_down(WPARAM wparam, LPARAM lparam)
	{
		// handle app specific key inputs

		win32_app::on_key_down(wparam, lparam);
	}

	expected_t<int> app::run()
	{
		return win32_app::run();
	}
}
