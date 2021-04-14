#pragma once

#include "win32_header_wrapper.h"

#include "win32_exception.h"
#include "dxgi_debug_info_manager.h"

namespace d3dexp
{
	class d3d11_exception : public d3d_exception
	{
	public:
		d3d11_exception(int line_no, char const* file_name, HRESULT error_code) noexcept
			: d3d_exception(line_no, file_name), m_error_code(error_code), m_dxgi_error_info(s_dxgi_debug_info_manager.get_messages())
		{}

	public:
		[[nodiscard]] virtual char const* what() const noexcept;
		[[nodiscard]] virtual char const* type() const noexcept { return "D3D11 Exception"; }
		[[nodiscard]] HRESULT error_code() const noexcept { return m_error_code; }
		[[nodiscard]] std::string error_desc() const noexcept { return win32_exception::get_error_code_desc(m_error_code); }
		[[nodiscard]] std::vector<std::string> dxgi_debug_info() const noexcept { return m_dxgi_error_info; }

		[[nodiscard]] static bool has_new_debug_info() noexcept { return s_dxgi_debug_info_manager.has_new_messages(); }
		
		static void mark_debug_info() noexcept { return s_dxgi_debug_info_manager.mark(); }

	private:
		static dxgi_debug_info_manager s_dxgi_debug_info_manager;

		HRESULT m_error_code;
		std::vector<std::string> m_dxgi_error_info;

	};

	class d3d11_device_removed_exception : public d3d11_exception
	{
		using d3d11_exception::d3d11_exception;

	public:
		[[nodiscard]] char const* type() const noexcept override { return "D3D11 Device Removed Exception"; }
	};

}

// helper macros
#define RAISE_D3D11_ERROR( error_code ) throw d3dexp::d3d11_exception(__LINE__, __FILE__, (error_code))
#define RAISE_D3D11_ERROR_IF_FAILED(hrcall) d3dexp::d3d11_exception::mark_debug_info(); if (FAILED(hr = (hrcall))) RAISE_D3D11_ERROR( hr )
#define RAISE_D3D11_DEVICE_REMOVED(dev_p) throw d3dexp::d3d11_device_removed_exception(__LINE__, __FILE__, dev_p->GetDeviceRemovedReason())
#define RAISE_D3D11_ON_DEBUG_INFO(call) \
	d3dexp::d3d11_exception::mark_debug_info(); call; \
	if (d3dexp::d3d11_exception::has_new_debug_info()) RAISE_D3D11_ERROR(0)

