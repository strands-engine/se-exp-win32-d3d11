#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>

#include "expected.h"

#pragma comment(lib, "d3d11.lib")

namespace d3dexp::bell0bytes
{
	class win32_app;

#ifndef D3DEXP_COMPTR
#define D3DEXP_COMPTR

	template <typename T>
	using com_ptr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	void** to_pp(com_ptr<T>& p)
	{
		return reinterpret_cast<void**>(p.GetAddressOf());
	}

#endif // !D3DEXP_COMPTR

	struct vertex_t
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
	};

	struct shader_buffer_t
	{
		BYTE* data = nullptr;
		SIZE_T size = 0u;
	};


	class d3d11_graphics
	{
	public:
		friend class win32_app;

	public:
		d3d11_graphics(win32_app * app_p);

		d3d11_graphics(d3d11_graphics const&) = delete;
		d3d11_graphics(d3d11_graphics &&) = delete;

		d3d11_graphics& operator=(d3d11_graphics const&) = delete;
		d3d11_graphics& operator=(d3d11_graphics &&) = delete;

		~d3d11_graphics() noexcept = default;

	public:
		[[nodiscard]] ID3D11Device* device() const noexcept { return m_device_p.Get(); }
		[[nodiscard]] ID3D11DeviceContext* context() const noexcept { return m_context_p.Get(); }

		void clear_buffers();
		expected_t<int> present();

	private:
		expected_t<void> create_resources();
		expected_t<void> initialize_pipeline();
		
		expected_t<void> on_resize();

		static expected_t<shader_buffer_t> load_shader(std::wstring const& path);

	private:
		com_ptr<ID3D11Device> m_device_p = nullptr;
		com_ptr<ID3D11DeviceContext> m_context_p = nullptr;
		com_ptr<IDXGISwapChain> m_swap_chain_p = nullptr;
		
		com_ptr<ID3D11RenderTargetView> m_rtv_p = nullptr;
		com_ptr<ID3D11DepthStencilView> m_dsv_p = nullptr;

		com_ptr<ID3D11VertexShader> m_vs_p = nullptr;
		com_ptr<ID3D11PixelShader> m_ps_p = nullptr;
		
		com_ptr<ID3D11InputLayout> m_layout_p = nullptr;

		win32_app* m_app_p;

		DXGI_FORMAT m_requested_pixel_format = DXGI_FORMAT_B8G8R8A8_UNORM;
	};
}
