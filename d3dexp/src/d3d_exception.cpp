#include "d3d_exception.h"

#include <sstream>

namespace d3dexp
{

	char const* d3d_exception::what() const noexcept
	{
		auto sout = std::ostringstream{};
		sout << type() << " at " << location_desc();
		m_what_buffer = sout.str();
		return m_what_buffer.c_str();
	}

	std::string d3dexp::d3d_exception::location_desc() const noexcept
	{
		auto sout = std::ostringstream{};
		sout << file_name() << ':' << line_no();
		return sout.str();
	}

}