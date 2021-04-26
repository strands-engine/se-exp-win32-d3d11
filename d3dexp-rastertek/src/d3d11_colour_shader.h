#pragma once

#include <string>

#include "d3d11_header_wrapper.h"

namespace d3dexp::rastertek
{
	class d3d11_colour_shader
	{
	private:
		struct cb_matrix
		{
			dxm::XMMATRIX world;
			dxm::XMMATRIX view;
			dxm::XMMATRIX projection;
		};

	public:
		d3d11_colour_shader(ID3D11Device * device_p, HWND window_h) noexcept;

		d3d11_colour_shader(d3d11_colour_shader const&) = delete;
		d3d11_colour_shader(d3d11_colour_shader &&) = delete;

		d3d11_colour_shader& operator=(d3d11_colour_shader const&) = delete;
		d3d11_colour_shader& operator=(d3d11_colour_shader &&) = delete;

		~d3d11_colour_shader() noexcept = default;

	public:
		bool render(ID3D11DeviceContext* context_p, int index_count, dxm::XMMATRIX world, dxm::XMMATRIX view, dxm::XMMATRIX projection) noexcept;

	private:
		bool initialize_shaders(ID3D11Device* device_p, HWND window_h, std::wstring const& vs_code_path, std::wstring const& ps_code_path) noexcept;
		void output_compiler_error(ID3D10Blob* msg_p, HWND window_h, std::wstring const& path) noexcept;

		bool set_globals(ID3D11DeviceContext* context_p, dxm::XMMATRIX world, dxm::XMMATRIX view, dxm::XMMATRIX projection) noexcept;
		void draw(ID3D11DeviceContext* context_p, int index_count) noexcept;

	private:
		com_ptr<ID3D11VertexShader> m_vs_p = nullptr;
		com_ptr<ID3D11PixelShader> m_ps_p = nullptr;
		com_ptr<ID3D11InputLayout> m_layout_p = nullptr;
		com_ptr<ID3D11Buffer> m_cb_matrix_p = nullptr;
	};
}