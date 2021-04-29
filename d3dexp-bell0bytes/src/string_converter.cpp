#include "string_converter.h"


namespace d3dexp::bell0bytes
{
	// NOTE: originally used deprecated in c++17 <codecvt> header; changed to [jpres] version
	std::string string_converter::to_string(std::wstring const& wstr)
	{
		return std::string(wstr.begin(), wstr.end());
	}

	// NOTE: originally used deprecated in c++17 <codecvt> header; changed to [jpres] version
	std::wstring string_converter::to_wstring(std::string const& str)
	{
		return std::wstring(str.begin(), str.end());
	}
}