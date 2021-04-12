#include "win32_app.h"

#include <sstream>
#include <iomanip>

#include "win32_exception.h"

namespace d3dexp
{
	win32_app::win32_app() : m_window(800, 600, "D3DEXP Main Window") {}

	int win32_app::run()
	{
		// main application loop
		while (true)
		{
			const auto error_code = win32_window::process_messages();
			if (error_code)
			{
				return error_code.value();
			}
			frame();
		}
	}

	void win32_app::frame()
	{
		const float t = m_timer.peek();
		std::ostringstream oss;
		oss << "Time elapsed: " << std::fixed << std::setprecision(2) << std::setfill('0') << t << "s.";
		m_window.set_title(oss.str());
	}
}
