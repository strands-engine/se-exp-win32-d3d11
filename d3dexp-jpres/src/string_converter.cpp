#include "string_converter.h"

namespace d3dexp
{
	std::wstring string_converter::to_wide(std::string const& s)
	{
		return std::wstring(s.begin(), s.end());
	}
}