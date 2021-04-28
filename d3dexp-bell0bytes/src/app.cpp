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

	expected_t<int> app::run()
	{
		return win32_app::run();
	}
}
