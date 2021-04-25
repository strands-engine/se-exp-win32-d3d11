#pragma once

#include <memory>
#include <string>

#include "d3d11_header_wrapper.h"

namespace d3dexp
{
	class d3d11_graphics
	{
	public:
		d3d11_graphics(HWND window_h, d3d11_graphics_settings_t const& settings) noexcept;

		d3d11_graphics(d3d11_graphics const&) = delete;
		d3d11_graphics(d3d11_graphics &&) = delete;

		d3d11_graphics& operator=(d3d11_graphics const&) = delete;
		d3d11_graphics& operator=(d3d11_graphics &&) = delete;

		~d3d11_graphics() noexcept;

	public:
		[[nodiscard]] ID3D11Device* device() const noexcept { return m_device_p.Get(); }
		[[nodiscard]] ID3D11DeviceContext* context() const noexcept { return m_context_p.Get(); }

		[[nodiscard]] dxm::XMMATRIX& projection() noexcept { return m_projection_mx; }
		[[nodiscard]] dxm::XMMATRIX const& projection() const noexcept { return m_projection_mx; }
		[[nodiscard]] dxm::XMMATRIX& orthographic() noexcept { return m_orthographic_mx; }
		[[nodiscard]] dxm::XMMATRIX const& orthographic() const noexcept { return m_orthographic_mx; }
		[[nodiscard]] dxm::XMMATRIX& world() noexcept { return m_world_mx; }
		[[nodiscard]] dxm::XMMATRIX const& world() const noexcept { return m_world_mx; }

		void begin_scene(float r, float b, float g, float a) noexcept;
		void end_scene() noexcept;

	private:
		com_ptr<IDXGISwapChain> m_swap_chain_p = nullptr;
		com_ptr<ID3D11Device> m_device_p = nullptr;
		com_ptr<ID3D11DeviceContext> m_context_p = nullptr;
		com_ptr<ID3D11RenderTargetView> m_rtv_p = nullptr;
		
		com_ptr<ID3D11Texture2D> m_ds_buffer_p = nullptr;
		com_ptr<ID3D11DepthStencilState> m_ds_state_p = nullptr;
		com_ptr<ID3D11DepthStencilView> m_ds_view_p = nullptr;
		com_ptr<ID3D11RasterizerState> m_rasterizer_state_p = nullptr;

		dxm::XMMATRIX m_projection_mx;
		dxm::XMMATRIX m_orthographic_mx;
		dxm::XMMATRIX m_world_mx;		

		std::wstring m_adapter_name;
		SIZE_T m_adapter_dedicated_memory;

		DXGI_RATIONAL m_refresh_rate = { 0, 1 };

		int m_width = 0;
		int m_height = 0;
		float m_near_plane = 0.0f;
		float m_far_plane = 0.0f;
		float m_field_of_view = 3.141592654f / 4.0f;
		bool m_is_fullscreen = false;
		bool m_is_vsync_enabled = true;
	};
}