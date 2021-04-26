#include "d3d11_graphics.h"

#include <d3dcompiler.h>

#include "d3d11_exception.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace d3dexp::chili
{
	d3d11_graphics::d3d11_graphics(HWND window_h)
	{
		// for error handling
		auto hr = HRESULT{};

		// configuring swap chain options
		auto sc_desc = DXGI_SWAP_CHAIN_DESC{};

		sc_desc.BufferDesc.Width = 0;                                                 // buffer width (default from window)
		sc_desc.BufferDesc.Height = 0;                                                // buffer height (default from window)
		sc_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;                       // pixel format (32-bit, 8 bits per channel BGRA, values in [0.0; 1.0] interval)
		sc_desc.BufferDesc.RefreshRate.Numerator = 0;                                 // refresh rate used (default; to be queried from adapter for fullscreen)
		sc_desc.BufferDesc.RefreshRate.Denominator = 0;
		sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                   // unspecified buffer scaling mode (none should be required)
		sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;   // unspecified scanline ordering (for interlacing?)
		sc_desc.SampleDesc.Count = 1;                                                 // antialiasing sampling count (no antialiasing)
		sc_desc.SampleDesc.Quality = 0;                                               // antialiasing sampling quality level (no antialiasing)
		sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                        // buffer usage as rendering output target
		sc_desc.BufferCount = 1;                                                      // single backbuffer (for double-buffering)
		sc_desc.OutputWindow = window_h;                                              // setting our parent window as output
		sc_desc.Windowed = TRUE;                                                      // use windowed mode (vs fullscreen; for now)
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                                // frontbuffer discarded after swap (can be modernised to use advised flip model if buffer count is 2 or more)
		sc_desc.Flags = 0;                                                            // no extra option flags

		// set initialziation flags
		auto flags = UINT{};
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !_DEBUG

		// create: device, device context, front and back buffers and swap chain
		RAISE_D3D11_ERROR_IF_FAILED(
			D3D11CreateDeviceAndSwapChain(
				nullptr,                       // use default graphics adapter (provide choice)
				D3D_DRIVER_TYPE_HARDWARE,      // hardware driver type (vs software or reference)
				nullptr,                       // relevant only for software drivers
				flags,                         // extra device creation flags (set above)
				nullptr,                       // no preferred feture levels (use highest compatible)
				0,                             // legth of previous array
				D3D11_SDK_VERSION,             // SDK version used
				&sc_desc,                      // swap chain settings (filled above)
				&m_swap_chain_p,               // pointer to newly created swap chain COM object
				&m_device_p,                   // pointer to newly created D3D device
				nullptr,                       // not capturing actually used feature level (get later)
				&m_context_p                   // pointer to newly created D3D (immediate) context			
			)
		);

		// acquiring pointer to render target view
		// getting texture subresource of swap chain
		auto back_buffer_p = com_ptr<ID3D11Resource>{};
		RAISE_D3D11_ERROR_IF_FAILED(
			m_swap_chain_p->GetBuffer(0, __uuidof(ID3D11Resource), to_pp(back_buffer_p))
		);
//#pragma warning(disable:6387)
		RAISE_D3D11_ERROR_IF_FAILED(
			m_device_p->CreateRenderTargetView(back_buffer_p.Get(), nullptr, &m_rtv_p)
		);
//#pragma warning(default:6387)
	}

	void d3d11_graphics::clear_buffer(float r, float g, float b) noexcept
	{
		// clear backbuffer to given colour
		const float colour[] = { r, g, b, 1.0f };
		m_context_p->ClearRenderTargetView(m_rtv_p.Get(), colour);
	}

	void d3d11_graphics::present_frame()
	{
		// present backbuffer to the front (1 - sync rate 1 frame per screen refresh rate; 0 - no additional flags)
		auto hr = m_swap_chain_p->Present(1u, 0u);

		// check for errors - especially removed device error
		if (FAILED(hr))
		{
			if (hr == DXGI_ERROR_DEVICE_REMOVED)
			{
				RAISE_D3D11_DEVICE_REMOVED(m_device_p);
			}
			RAISE_D3D11_ERROR(hr);
		}
	}

	void d3d11_graphics::debug_hello_triangle()
	{
		// for error handling
		auto hr = HRESULT{};

		// creation of vertex buffer

		// setup of vertex structs and subresource data
		struct vertex_t
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;
			struct
			{
				float r;
				float g;
				float b;
			} colour;
		};

		const vertex_t vertices[] =
		{
			{ 0.0f,  0.5f, 0.0f,    0.5f, 1.0f, 0.5f},
			{ 0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 0.0f},
			{-0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 1.0f},
		};

		auto vb_data = D3D11_SUBRESOURCE_DATA{};
		vb_data.pSysMem = vertices;

		// confuguring vertex buffer options
		auto vb_desc = D3D11_BUFFER_DESC{};

		vb_desc.Usage = D3D11_USAGE_DEFAULT;                 // default usage type (inaccessible by cpu)
		vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;        // specify buffer as vertex buffer
		vb_desc.CPUAccessFlags = 0u;                         // data inaccessible by cpu
		vb_desc.ByteWidth = sizeof(vertices);                // size of all data given
		vb_desc.StructureByteStride = sizeof(vertex_t);      // size of the single vertex data struct
		vb_desc.MiscFlags = 0u;                              // no extra options

		// create vertex buffer object
		auto vertex_buffer_p = com_ptr<ID3D11Buffer>{};
		RAISE_D3D11_ERROR_IF_FAILED(m_device_p->CreateBuffer(&vb_desc, &vb_data, &vertex_buffer_p));

		// bind vertex buffer to input assembler
		auto stride = static_cast<UINT>(sizeof(vertex_t));
		auto offset = UINT{ 0u };
		m_context_p->IASetVertexBuffers(
			0u,                                              // start slot (here only one, so irrelevant, just leave 0 value
			1u,                                              // number of buffers used - here is 1
			vertex_buffer_p.GetAddressOf(),                  // array of actual vertex data resource
			&stride,                                         // array of element strides in each vertex resource
			&offset                                          // array of element offsets in each vertex resource
		);


		// creation of index buffer
		const unsigned short indices[] = { 0, 1, 2 };

		auto ib_data = D3D11_SUBRESOURCE_DATA{};
		ib_data.pSysMem = indices;

		// confuguring index buffer options
		auto ib_desc = D3D11_BUFFER_DESC{};

		ib_desc.Usage = D3D11_USAGE_DEFAULT;                 // default usage type (inaccessible by cpu)
		ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;         // specify buffer as index buffer
		ib_desc.CPUAccessFlags = 0u;                         // data inaccessible by cpu
		ib_desc.ByteWidth = sizeof(indices);                 // size of all data given
		ib_desc.StructureByteStride = sizeof(unsigned int);  // size of the single vertex data struct
		ib_desc.MiscFlags = 0u;                              // no extra options

		// create index buffer object
		auto index_buffer_p = com_ptr<ID3D11Buffer>{};
		RAISE_D3D11_ERROR_IF_FAILED(m_device_p->CreateBuffer(&ib_desc, &ib_data, &index_buffer_p));

		// bind index buffer to input assembler
		m_context_p->IASetIndexBuffer(
			index_buffer_p.Get(),                            // actual index data resource
			DXGI_FORMAT_R16_UINT,                            // index format (16bit unsigned integers)
			0u                                               // offset
		);


		// setting primitive topology type in input assembler
		m_context_p->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		// loading and creating vertex shader
		auto vs_blob_p = com_ptr<ID3DBlob>{};
		RAISE_D3D11_ERROR_IF_FAILED(D3DReadFileToBlob(L"D:\\repos\\se-exp-win32-d3d11\\bin\\x64-Debug\\d3dexp-chili\\vs_hello_triangle.cso", &vs_blob_p));
		auto vertex_shader_p = com_ptr<ID3D11VertexShader>{};
		RAISE_D3D11_ERROR_IF_FAILED(
			m_device_p->CreateVertexShader(vs_blob_p->GetBufferPointer(), vs_blob_p->GetBufferSize(), nullptr, &vertex_shader_p)
		);

		// bind vertex shader to pipeline
		m_context_p->VSSetShader(vertex_shader_p.Get(), nullptr, 0u);


		// loading and creating pixel shader
		auto ps_blob_p = com_ptr<ID3DBlob>{};
		RAISE_D3D11_ERROR_IF_FAILED(D3DReadFileToBlob(L"D:\\repos\\se-exp-win32-d3d11\\bin\\x64-Debug\\d3dexp-chili\\ps_hello_triangle.cso", &ps_blob_p));
		auto pixel_shader_p = com_ptr<ID3D11PixelShader>{};
		RAISE_D3D11_ERROR_IF_FAILED(
			m_device_p->CreatePixelShader(ps_blob_p->GetBufferPointer(), ps_blob_p->GetBufferSize(), nullptr, &pixel_shader_p)
		);

		// bind vertex shader to pipeline
		m_context_p->PSSetShader(pixel_shader_p.Get(), nullptr, 0u);


		// setup input layout for given data
		const UINT il_len = 2;
		D3D11_INPUT_ELEMENT_DESC il_desc[il_len];

		// vertex position data
		il_desc[0].SemanticName = "POSITION";
		il_desc[0].SemanticIndex = 0u;
		il_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		il_desc[0].InputSlot = 0u;
		il_desc[0].AlignedByteOffset = 0u;
		il_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		il_desc[0].InstanceDataStepRate = 0u;

		// vertex colour data
		il_desc[1].SemanticName = "COLOUR";
		il_desc[1].SemanticIndex = 0u;
		il_desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		il_desc[1].InputSlot = 0u;
		il_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		il_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		il_desc[1].InstanceDataStepRate = 0u;

		// create input layout object
		auto input_layout_p = com_ptr<ID3D11InputLayout>{};
		RAISE_D3D11_ERROR_IF_FAILED(
			m_device_p->CreateInputLayout(il_desc, il_len, vs_blob_p->GetBufferPointer(), vs_blob_p->GetBufferSize(), &input_layout_p));

		// bind input layout to input assembler
		m_context_p->IASetInputLayout(input_layout_p.Get());


		// bind render target view to output merger
		m_context_p->OMSetRenderTargets(1u, m_rtv_p.GetAddressOf(), nullptr);


		// specify viewport settings
		auto viewport = D3D11_VIEWPORT{};

		viewport.Width =  800.0f;
		viewport.Height = 600.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		// bind viewport to rasterizer
		m_context_p->RSSetViewports(1u, &viewport);


		// actual draw call (indexed)
		RAISE_D3D11_ON_DEBUG_INFO(m_context_p->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0));
		//RAISE_D3D11_ON_DEBUG_INFO(m_context_p->Draw(static_cast<UINT>(std::size(vertices)), 0u));
	}
}

