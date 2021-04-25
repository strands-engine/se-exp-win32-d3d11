#include "d3d11_colour_shader.h"

#include <fstream>

namespace d3dexp
{
    d3d11_colour_shader::d3d11_colour_shader(ID3D11Device* device_p, HWND window_h) noexcept
    {
        if (!initialize_shaders(device_p, window_h, L"D:\\repos\\se-exp-win32-d3d11\\d3dexp-rastertek\\shaders\\vs_colour.hlsl", L"D:\\repos\\se-exp-win32-d3d11\\d3dexp-rastertek\\shaders\\ps_colour.hlsl")) exit(-1);
    }

    bool d3d11_colour_shader::render(ID3D11DeviceContext* context_p, int index_count, dxm::XMMATRIX world, dxm::XMMATRIX view, dxm::XMMATRIX projection) noexcept
    {
        if (!set_globals(context_p, world, view, projection)) return false;
        draw(context_p, index_count);
        return true;
    }

    bool d3d11_colour_shader::initialize_shaders(ID3D11Device* device_p, HWND window_h, std::wstring const& vs_code_path, std::wstring const& ps_code_path) noexcept
    {
		// COMPILE VERTEX AND PIXEL SHADERS
		
		// compiling vertex shader
		auto compiler_error = com_ptr<ID3D10Blob>{};
		auto vs_bytecode = com_ptr<ID3D10Blob>{};
		auto hr = D3DCompileFromFile(
			vs_code_path.c_str(),						// path to file with vertex shader code
			nullptr,									// no extra defines provided to the compiler
			nullptr,									// no extra includes provided to the compiler
			"main",										// name of entry point
			"vs_5_0",									// target hlsl shader type and version
			D3D10_SHADER_ENABLE_STRICTNESS,				// compiler flags - ENABLE_STRICTNESS (???)
			0,											// more compiler flags
			&vs_bytecode,								// pointer to blob storing compiled bytecode
			&compiler_error);							// pointer to blob storing error output from compiler
		if (FAILED(hr))
		{
			// if error message blob was filled, then compilation failed - write its reasons to file
			if (compiler_error)
			{
				output_compiler_error(compiler_error.Get(), window_h, vs_code_path);
			}
			// otherwise, most likely file was not found
			else
			{
				MessageBox(window_h, vs_code_path.c_str(), L"Missing vertex shader file", MB_OK);
			}
			return false;
		}

		// create vertex shader object from obtained bytecode
		hr = device_p->CreateVertexShader(vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), nullptr, &m_vs_p);
		if (FAILED(hr)) return false;

		// compiling pixel shader
		auto ps_bytecode = com_ptr<ID3D10Blob>{};
		hr = D3DCompileFromFile(
			ps_code_path.c_str(),						// path to file with vertex shader code
			nullptr,									// no extra defines provided to the compiler
			nullptr,									// no extra includes provided to the compiler
			"main",										// name of entry point
			"ps_5_0",									// target hlsl shader type and version
			D3D10_SHADER_ENABLE_STRICTNESS,				// compiler flags - ENABLE_STRICTNESS (???)
			0,											// more compiler flags
			&ps_bytecode,								// pointer to blob storing compiled bytecode
			&compiler_error);							// pointer to blob storing error output from compiler
		if (FAILED(hr))
		{
			// if error message blob was filled, then compilation failed - write its reasons to file
			if (compiler_error)
			{
				output_compiler_error(compiler_error.Get(), window_h, vs_code_path);
			}
			// otherwise, most likely file was not found
			else
			{
				MessageBox(window_h, vs_code_path.c_str(), L"Missing pixel shader file", MB_OK);
			}
			return false;
		}

		// create pixel shader object from obtained bytecode
		hr = device_p->CreatePixelShader(ps_bytecode->GetBufferPointer(), ps_bytecode->GetBufferSize(), nullptr, &m_ps_p);
		if (FAILED(hr)) return false;


		// CREATING VERTEX INPUT LAYOUT
		
		// setup array of input layout element descriptions
		D3D11_INPUT_ELEMENT_DESC il_desc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOUR",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		// create input layout object using above settings
		hr = device_p->CreateInputLayout(il_desc, ARRAYSIZE(il_desc), vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), &m_layout_p);
		if (FAILED(hr)) return false;


		// SETUP DYNAMIC CONSTANT BUFFER FOR WVP MATRICES

		// setup constant buffer options
		auto cb_desc = D3D11_BUFFER_DESC{};

		cb_desc.Usage = D3D11_USAGE_DYNAMIC;					// dynamic buffer (we change its data often)
		cb_desc.ByteWidth = sizeof(cb_matrix);					// buffer size in bytes
		cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// binding type as a constant buffer (same values for all vertices)
		cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// we need cpu write access to change buffer data
		cb_desc.MiscFlags = 0;									// no extra flags
		cb_desc.StructureByteStride = 0;						// no stride

		// create constant buffer with above settings
		hr = device_p->CreateBuffer(&cb_desc, nullptr, &m_cb_matrix_p);
		if (FAILED(hr)) return false;


		return true;
    }

    void d3d11_colour_shader::output_compiler_error(ID3D10Blob* msg_p, HWND window_h, std::wstring const& path) noexcept
    {
		// get poiter and size of receiver error message
		const auto msgstr_p = static_cast<char*>(msg_p->GetBufferPointer());
		const auto msgstr_len = msg_p->GetBufferSize();

		// open stream to error log file and write out the error message
		auto fout = std::ofstream{ "shader-error.txt" };
		for (auto i = SIZE_T{ 0u }; i < msgstr_len; i++)
		{
			fout << msgstr_p[i];
		}
		fout.close();

		// show message infoorming about error ad log location
		MessageBox(window_h, L"Error compiling shader. Check 'shader-error.txt' for error message.", path.c_str(), MB_OK);
    }

    bool d3d11_colour_shader::set_globals(ID3D11DeviceContext* context_p, dxm::XMMATRIX world, dxm::XMMATRIX view, dxm::XMMATRIX projection) noexcept
	{
		// UPDATES CONSTANT MATRIX BUFFER DATA

		// for d3d11 it is required to transpose matrices
		world = dxm::XMMatrixTranspose(world);
		view = dxm::XMMatrixTranspose(view);
		projection = dxm::XMMatrixTranspose(projection);

		// lock constant buffer to enable writing to it through subresource
		auto mapped = D3D11_MAPPED_SUBRESOURCE{};
		auto hr = context_p->Map(m_cb_matrix_p.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (FAILED(hr)) return false;

		// update data in mapped subresource
		auto cb_p = reinterpret_cast<cb_matrix*>(mapped.pData);
		cb_p->world = world;
		cb_p->view = view;
		cb_p->projection = projection;

		// unlock constant buffer after write
		context_p->Unmap(m_cb_matrix_p.Get(), 0);

		// rebind constant buffer with updated values to vertex shader
		context_p->VSSetConstantBuffers(0u, 1u, m_cb_matrix_p.GetAddressOf());

		return true;
	}

    void d3d11_colour_shader::draw(ID3D11DeviceContext* context_p, int index_count) noexcept
    {
		// bind proper shaders and layout to rendering pipeline
		context_p->IASetInputLayout(m_layout_p.Get());
		context_p->VSSetShader(m_vs_p.Get(), nullptr, 0u);
		context_p->PSSetShader(m_ps_p.Get(), nullptr, 0u);

		// draw
		context_p->DrawIndexed(index_count, 0u, 0);
		//context_p->Draw(3, 0);
    }
}