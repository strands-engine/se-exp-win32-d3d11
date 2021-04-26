#pragma once

#include "win32_header_wrapper.h"
#include <d3d11.h>

namespace d3dexp::chili
{
	class d3d11_graphics
	{
	public:
		d3d11_graphics( HWND window_h );

		d3d11_graphics(d3d11_graphics const&) = delete;
		d3d11_graphics(d3d11_graphics &&) = delete;

		d3d11_graphics& operator=(d3d11_graphics const&) = delete;
		d3d11_graphics& operator=(d3d11_graphics &&) = delete;

		~d3d11_graphics() noexcept = default;

	public:
		void clear_buffer(float r, float g, float b) noexcept;

		void present_frame();

		void debug_hello_triangle();

	private:
		com_ptr<ID3D11Device> m_device_p = nullptr;
		com_ptr<ID3D11DeviceContext> m_context_p = nullptr;
		com_ptr<IDXGISwapChain> m_swap_chain_p = nullptr;
		com_ptr<ID3D11RenderTargetView> m_rtv_p = nullptr;
	};
}