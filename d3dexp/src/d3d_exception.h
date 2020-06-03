#pragma once

#include <exception>
#include <string>

namespace d3dexp 
{
	class d3d_exception : public std::exception 
	{
	public:
		d3d_exception(int line_no, char const* file_name) noexcept 
			: m_line_no(line_no), m_file_name(file_name) {};

	public:
		[[nodiscard]] char const * what() const noexcept override;
		[[nodiscard]] virtual char const* type() const noexcept { return "D3DEXP Exception"; }
		[[nodiscard]] int line_no() const noexcept { return m_line_no;  }
		[[nodiscard]] std::string const& file_name() const noexcept { return m_file_name; }
		[[nodiscard]] std::string location_desc() const noexcept;

	protected:
		mutable std::string m_what_buffer;

	private:
		int m_line_no;
		std::string m_file_name;
	};
}