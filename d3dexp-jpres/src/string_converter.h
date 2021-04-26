#pragma once

#include <string>

namespace d3dexp::jpres
{
	class string_converter
	{
	public:
		string_converter() = delete;

	public:
		static std::wstring to_wide(std::string const& s);
	};
}