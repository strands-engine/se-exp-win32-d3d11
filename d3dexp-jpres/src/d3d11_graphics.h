#pragma once

#include "dxgi_adapter_reader.h"

namespace d3dexp
{
	class d3d11_graphics
	{
	public:
		d3d11_graphics() noexcept = default;

		d3d11_graphics(d3d11_graphics const&) = delete;
		d3d11_graphics(d3d11_graphics &&) = delete;

		d3d11_graphics& operator=(d3d11_graphics const&) = delete;
		d3d11_graphics& operator=(d3d11_graphics &&) = delete;

		~d3d11_graphics() noexcept = default;

	public:
		bool initialize(HWND window_h, int width, int height) noexcept;

		void frame() noexcept;

	private:
		bool initialize_d3d11(HWND window_h, int width, int height) noexcept;
		bool initialize_shaders() noexcept;

	private:
		com_ptr<ID3D11Device> m_device_p;
		com_ptr<ID3D11DeviceContext> m_context_p;
		com_ptr<IDXGISwapChain> m_swap_chain_p;
		com_ptr<ID3D11RenderTargetView> m_rtv_p;

		com_ptr<ID3D11InputLayout> m_input_layout_p;
		com_ptr<ID3D10Blob> m_vx_shader_data_p;
	};
}