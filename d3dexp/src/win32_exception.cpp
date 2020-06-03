#include "win32_exception.h"

#include <sstream>

namespace d3dexp 
{

char const* win32_exception::what() const noexcept
{
	auto sout = std::ostringstream{};
	sout << type() << " at " << location_desc() << '\n'
		<< "\n[#" << error_code() << "] " << error_desc();
	m_what_buffer = sout.str();
	return m_what_buffer.c_str();
}

std::string win32_exception::get_error_code_desc(HRESULT error_code) noexcept
{
	char * buf_p = nullptr;
	auto msg_len = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>( &buf_p ), 0, nullptr);
	if (msg_len == 0)
	{
		return "Unknown error code";
	}
	auto result = std::string{ buf_p };
	LocalFree(buf_p);
	return result;
}

}
