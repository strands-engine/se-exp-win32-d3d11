#pragma once

#include <string>

namespace d3dexp::bell0bytes
{
	class string_converter
	{
	public:
		string_converter() = delete;

	public:
		// use deprecated header (in c++17)
		static std::string to_string(std::wstring const& wstr);
		static std::wstring to_wstring(std::string const& str);
	};
}