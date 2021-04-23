#pragma once

#include "dxgi_adapter_reader.h"

#include <string>
#include <d3dcompiler.h>

#pragma comment(lib, "D3DCompiler.lib")

namespace d3dexp
{
	class d3d11_vertex_shader
	{
	public:
		d3d11_vertex_shader() noexcept = default;

		d3d11_vertex_shader(d3d11_vertex_shader const&) = delete;
		d3d11_vertex_shader(d3d11_vertex_shader &&) = delete;

		d3d11_vertex_shader& operator=(d3d11_vertex_shader const&) = delete;
		d3d11_vertex_shader& operator=(d3d11_vertex_shader &&) = delete;

		~d3d11_vertex_shader() noexcept = default;

	public:
		bool initialize(com_ptr<ID3D11Device> const& device_p, std::wstring const& path, D3D11_INPUT_ELEMENT_DESC * il_desc_p, UINT il_desc_len) noexcept;

		[[nodiscard]] ID3D11VertexShader* ptr() const noexcept { return m_shader_p.Get(); }
		[[nodiscard]] ID3D11InputLayout* layout() const noexcept { return m_layout_p.Get(); }

		[[nodiscard]] void* bytecode() const noexcept { return m_data_p->GetBufferPointer(); }
		[[nodiscard]] SIZE_T bytecode_size() const noexcept { return m_data_p->GetBufferSize(); }

	private:
		com_ptr<ID3D11VertexShader> m_shader_p = nullptr;
		com_ptr<ID3D11InputLayout> m_layout_p = nullptr;
		com_ptr<ID3D10Blob> m_data_p = nullptr;



	};
}