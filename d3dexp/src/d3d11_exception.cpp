#include "d3d11_exception.h"

#include <sstream>

namespace d3dexp::chili
{
	dxgi_debug_info_manager d3d11_exception::s_dxgi_debug_info_manager = dxgi_debug_info_manager{};

	char const* d3d11_exception::what() const noexcept
	{
		auto sout = std::ostringstream{};
		sout << type() << " at " << location_desc() << '\n'
			<< "\n[#" << error_code() << "] " << error_desc();

		if (!m_dxgi_error_info.empty())
		{
			sout << "\n\nDXGI DEBUG INFO DETAILS:\n\n";
			for (auto const& msg : m_dxgi_error_info)
			{
				sout << msg << "\n\n";
			}
		}

		m_what_buffer = sout.str();
		return m_what_buffer.c_str();
	}

}
