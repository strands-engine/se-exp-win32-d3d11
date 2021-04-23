#include "d3d11_shaders.h"

#include "error_logger.h"

namespace d3dexp
{
	bool d3d11_vertex_shader::initialize(com_ptr<ID3D11Device> const& device_p, std::wstring const& path, D3D11_INPUT_ELEMENT_DESC* il_desc_p, UINT il_desc_len) noexcept
	{
		// loading compiled shader bytecode
		auto hr = D3DReadFileToBlob(path.c_str(), &m_data_p);
		if (FAILED(hr))
		{
			error_logger::log(hr, std::wstring{ L"Failed to load vertex shader from: " } + path);
			return false;
		}

		// creating respective vertex shader object
		hr = device_p->CreateVertexShader(bytecode(), bytecode_size(), nullptr, &m_shader_p);
		if (FAILED(hr))
		{
			error_logger::log(hr, std::wstring{ L"Failed to create vertex shader from: " } + path);
			return false;
		}

		// create input layout from given description
		hr = device_p->CreateInputLayout(
			il_desc_p,						// input layout descriptions array
			il_desc_len,					// number of elements in descriptions array
			bytecode(),						// pointer to vertex shader bytecode
			bytecode_size(),				// size of vertex shader bytecode
			&m_layout_p);					// pointer to input layout being created
		if (FAILED(hr))
		{
			error_logger::log(hr, std::wstring{ L"Failed to create input layout for: " } + path);
			return false;
		}

		return true;
	}
}