#pragma once

#include <string>
#include <Windows.h>

namespace d3dexp
{
	class error_logger
	{
	public:
		error_logger() = delete;

	public:
		static void log(std::string const& message);
		static void log(HRESULT hr, std::string const& message);
	};
}
