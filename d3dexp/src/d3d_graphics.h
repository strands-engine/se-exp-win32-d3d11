#pragma once

#include "win32_header_wrapper.h"
#include <d3d11.h>

namespace d3dexp
{
	class d3d_graphics
	{
	public:
		d3d_graphics( HWND window_h );

		d3d_graphics(d3d_graphics const&) = delete;
		d3d_graphics(d3d_graphics &&) = delete;

		d3d_graphics& operator=(d3d_graphics const&) = delete;
		d3d_graphics& operator=(d3d_graphics &&) = delete;

		~d3d_graphics() noexcept;

	public:
		void clear_buffer(float r, float g, float b) noexcept;

		void present_frame();

	private:
	private:
		ID3D11Device* m_device_p = nullptr;
		ID3D11DeviceContext* m_context_p = nullptr;
		IDXGISwapChain* m_swap_chain_p = nullptr;
		ID3D11RenderTargetView* m_rtv_p = nullptr;
	};
}