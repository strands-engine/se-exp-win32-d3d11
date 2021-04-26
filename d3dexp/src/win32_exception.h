#pragma once

#include "win32_header_wrapper.h"
#include "d3d_exception.h"

namespace d3dexp::chili
{
	class win32_exception : public d3d_exception
	{
	public:
		win32_exception(int line_no, char const* file_name, HRESULT error_code) noexcept
			: d3d_exception(line_no, file_name), m_error_code(error_code) {}

	public:
		[[nodiscard]] virtual char const* what() const noexcept;
		[[nodiscard]] virtual char const* type() const noexcept { return "Win32 Exception"; }
		[[nodiscard]] HRESULT error_code() const noexcept { return m_error_code; }
		[[nodiscard]] std::string error_desc() const noexcept { return get_error_code_desc(m_error_code); }

		[[nodiscard]] static std::string get_error_code_desc(HRESULT error_code) noexcept;

	private:
		HRESULT m_error_code;
	};
}



// helper macros
#define RAISE_WIN32_ERROR( error_code ) throw d3dexp::chili::win32_exception{ __LINE__, __FILE__, (error_code) }
#define RAISE_WIN32_LAST_ERROR() RAISE_WIN32_ERROR( static_cast<HRESULT>( GetLastError() ) )
