#include "d3d11_graphics.h"

#include <vector>

namespace d3dexp
{
	d3d11_graphics::d3d11_graphics(HWND window_h, d3d11_graphics_settings_t const& settings) noexcept
		: m_width(settings.width), m_height(settings.height), 
		  m_near_plane(settings.screen_near_plane), m_far_plane(settings.screen_far_plane), m_field_of_view(settings.field_of_view),
		  m_is_fullscreen(settings.is_fullscreen), m_is_vsync_enabled(settings.is_vsync_enabled)
	{
		// QUERYING ADAPTER AND SCREEN DATA
		// here: to obtain refresh rate for fullscreen mode
		// later: to list/choose adapters and screen modes

		// create dxgi factory to enumerate adapters
		auto dxgi_factory_p = com_ptr<IDXGIFactory>{};
		auto hr = CreateDXGIFactory(__uuidof(IDXGIFactory), to_pp(dxgi_factory_p));
		if (FAILED(hr))
		{
			OutputDebugString(L"Failed to create DXGI factory.");
			exit(-1);
		}

		// enumerate available graphics adapters
		auto adapter_p = com_ptr<IDXGIAdapter>{};
		auto adapters = std::vector<com_ptr<IDXGIAdapter>>{};
		auto i = UINT{ 0u };
		while (SUCCEEDED(dxgi_factory_p->EnumAdapters(i, &adapter_p)))
		{
			adapters.emplace_back(adapter_p.Get());
			i++;
		}

		// debug output of enumerated adapters
		auto adapter_desc = DXGI_ADAPTER_DESC{};
		for (auto i = std::size_t{ 0u }; i < adapters.size(); i++)
		{
			adapters[i]->GetDesc(&adapter_desc);
			auto msg = std::wstring{ L"ADAPTER #" };
			msg += std::to_wstring(i + 1);
			msg += L": ";
			msg += adapter_desc.Description;
			msg += L" [";
			msg += std::to_wstring(adapter_desc.DedicatedVideoMemory / 1024 / 1024);
			msg += L" MB]\n";
			OutputDebugString(msg.c_str());
		}

		// choose first available adapter and store its description and dedicated video memory
		adapter_p = adapters.front();
		adapter_p->GetDesc(&adapter_desc);
		m_adapter_name = adapter_desc.Description;
		m_adapter_dedicated_memory = adapter_desc.DedicatedVideoMemory;


		// enumerate available outputs of chosed graphics adapter
		auto output_p = com_ptr<IDXGIOutput>{};
		auto outputs = std::vector<com_ptr<IDXGIOutput>>{};
		i = UINT{ 0u };
		while (SUCCEEDED(adapter_p->EnumOutputs(i, &output_p)))
		{
			outputs.emplace_back(output_p.Get());
			i++;
		}

		// debug output of enumerated outputs
		auto output_desc = DXGI_OUTPUT_DESC{};
		for (auto i = std::size_t{ 0u }; i < outputs.size(); i++)
		{
			outputs[i]->GetDesc(&output_desc);
			auto msg = std::wstring{ L"OUTPUT #" };
			msg += std::to_wstring(i + 1);
			msg += L": ";
			msg += output_desc.DeviceName;
			msg += L"\n";
			OutputDebugString(msg.c_str());
		}

		// choose first available output and get all its supported display modes
		output_p = outputs.front();

		// get size of modes array
		const auto desired_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		// const auto desired_format = DXGI_FORMAT_B8G8R8A8_UNORM; - better format [chili + other comments]
		auto modes_count = UINT{};
		hr = output_p->GetDisplayModeList(desired_format, DXGI_ENUM_MODES_INTERLACED, &modes_count, nullptr);
		if (FAILED(hr)) exit(-1);

		// obtain modes array
		auto display_modes = new DXGI_MODE_DESC[modes_count];
		hr = output_p->GetDisplayModeList(desired_format, DXGI_ENUM_MODES_INTERLACED, &modes_count, display_modes);
		if (FAILED(hr)) exit(-1);

		// debug output of enumerated display modes
		for (auto i = std::size_t{ 0u }; i < modes_count; i++)
		{
			auto msg = std::wstring{ L"DISPLAY MODE #" };
			msg += std::to_wstring(i + 1);
			msg += L": ";
			msg += std::to_wstring(display_modes[i].Width);
			msg += L" x ";
			msg += std::to_wstring(display_modes[i].Height);
			msg += L" [";
			msg += std::to_wstring(display_modes[i].RefreshRate.Numerator / static_cast<float>(display_modes[i].RefreshRate.Denominator));
			msg += L" Hz]\n";
			OutputDebugString(msg.c_str());
		}

		// if vsync is enabled - look through all available display modes to find one matching screen dimensions and get its refresh rate
		// NOTE: todo enumerate all available refresh rates to choose
		if (m_is_vsync_enabled)
		{
			for (auto i = std::size_t{ 0u }; i < modes_count; i++)
			{
				if (display_modes[i].Width == static_cast<UINT>(m_width) && display_modes[i].Height == static_cast<UINT>(m_height))
				{
					m_refresh_rate = display_modes[i].RefreshRate;
					break;
				}
			}
		}

		// cleanup display modes list
		delete[] display_modes;


		// CREATION OF D3D11 DEVICE, IMMEDIATE CONTEXT AND SWAP CHAIN

		// setting up the swap chain options
		auto sc_desc = DXGI_SWAP_CHAIN_DESC{};

		sc_desc.BufferCount = 1;													// double buffering setup
		sc_desc.BufferDesc.Width = m_width;											// output buffer width
		sc_desc.BufferDesc.Height = m_height;										// output buffer height
		sc_desc.BufferDesc.Format = desired_format;									// output buffer pixel format - prefer BGRA [chili + other comments]
		sc_desc.BufferDesc.RefreshRate = m_refresh_rate;							// if vsync is enabled set to output RR, oterwise default 0=0/1
		sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;						// set buffer usage to back buffer
		sc_desc.OutputWindow = window_h;											// handle to output window
		sc_desc.SampleDesc.Count = 1;												// disable multisampling
		sc_desc.SampleDesc.Quality = 0;												// disable multisampling
		sc_desc.Windowed = BOOL{ !m_is_fullscreen };								// set mode to windowed or fullscreen
		sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// scanline ordering - unspecified
		sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					// scaling mode - unspecified
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                              // discard back buffer on swap
		sc_desc.Flags = 0;															// no extra option flags

		// choose desired feature level
		D3D_FEATURE_LEVEL desired_feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
		auto used_feature_level = D3D_FEATURE_LEVEL{};

		// create device, device context and swap chain
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,							// use default adapter (optionally pass previously chosed adapter)
			D3D_DRIVER_TYPE_HARDWARE,           // specified to chose default hardware adapter (if precisely given - change to UNKNOWN)
			nullptr,							// no handle to lib for software backend
			0,									// no extra runtime flags
			desired_feature_levels,				// desired feature level
			ARRAYSIZE(desired_feature_levels),	// number of desired feature levels
			D3D11_SDK_VERSION,					// sdk version macro
			&sc_desc,							// options for swap chain setup
			&m_swap_chain_p,					// pointer to created swap chain
			&m_device_p,						// pointer to created device
			&used_feature_level,				// pointer to actually used feature level
			&m_context_p);						// pointer to created immediate device context
		if (FAILED(hr))
		{
			OutputDebugString(L"Failed to initialize D3D11 device, context, and swap chain.");
			exit(-1);
		}

		// debug output of used feature level
		switch (used_feature_level)
		{
		case D3D_FEATURE_LEVEL_11_0:
		{
			OutputDebugString(L"D3D FEATURE LEVEL - 11.0\n");
			break;
		}
		case D3D_FEATURE_LEVEL_11_1:
		{
			OutputDebugString(L"D3D FEATURE LEVEL - 11.1\n");
			break;
		}
		default:
			break;
		}


		// OBTAIN VIEW TO THE RENDER TARGET

		// get pointer to the swap chain back buffer (NOTE: [chili] ID3DREsource vs ID3D11Texture2D)
		auto back_buffer_p = com_ptr<ID3D11Texture2D>{};
		hr = m_swap_chain_p->GetBuffer(0, __uuidof(ID3D11Texture2D), to_pp(back_buffer_p));
		if (FAILED(hr)) exit(-1);

		// create render target view (no extra options)
		hr = m_device_p->CreateRenderTargetView(back_buffer_p.Get(), NULL, &m_rtv_p);
		if (FAILED(hr)) exit(-1);


		// SETUP DEPTH AND STENCIL BUFFERS

		// setup depth and stencil buffer (as 2D texture) options
		auto db_desc = D3D11_TEXTURE2D_DESC{};

		db_desc.Width = m_width;							// buffer width
		db_desc.Height = m_height;							// buffer height
		db_desc.MipLevels = 1;								// mip-mapping levels used
		db_desc.ArraySize = 1;								// number of textures in array
		db_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// 24 bits for depth information (normalized to [0,1]) and 8 bits for {0, ..., 255} integer
		db_desc.SampleDesc.Count = 1;						// no antialiasing/multisampling
		db_desc.SampleDesc.Quality = 0;						// no antialiasing/multisampling
		db_desc.Usage = D3D11_USAGE_DEFAULT;				// buffer usage type (default)
		db_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// buffer binding type (dept/stencil)
		db_desc.CPUAccessFlags = 0;							// no extra cpu acceess options
		db_desc.MiscFlags = 0;								// no extra options

		// create texture used as depth and stencil buffer (no initial data required)
		hr = m_device_p->CreateTexture2D(&db_desc, nullptr, &m_ds_buffer_p);
		if (FAILED(hr)) exit(-1);

		// setup optins for actual depth and stencil state
		auto ds_desc = D3D11_DEPTH_STENCIL_DESC{};

		ds_desc.DepthEnable = true;											// enable usage of depth information
		ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;				// no special masks for depth information writing
		ds_desc.DepthFunc = D3D11_COMPARISON_LESS;							// depth comparison information (standard '<' comparison to keep pixels)
		ds_desc.StencilEnable = true;										// enable usage of stencil information
		ds_desc.StencilReadMask = 0xFF;										// no special mask for reading stencil data
		ds_desc.StencilWriteMask = 0xFF;									// no special mask for writing stencil data
		ds_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;			// stencil behaviour on front facing triangles ...
		ds_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		ds_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;				// stencil behaviour on back facing triangles ...
		ds_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		ds_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		ds_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// create deth and stencil state with given description
		hr = m_device_p->CreateDepthStencilState(&ds_desc, &m_ds_state_p);
		if (FAILED(hr)) exit(-1);

		// bind created dept stencil state to rendering pipeline (OM)
		m_context_p->OMSetDepthStencilState(m_ds_state_p.Get(), 1u);

		// setup options for view to depth and stencil buffer
		auto dv_desc = D3D11_DEPTH_STENCIL_VIEW_DESC{};

		dv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				// same pixel format as for dept/stencil buffer
		dv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;		// dimensionality of buffer set to 2d
		dv_desc.Texture2D.MipSlice = 0;								// no mip slicing

		// create view of depth and stencil buffer with given settings
		hr = m_device_p->CreateDepthStencilView(m_ds_buffer_p.Get(), &dv_desc, &m_ds_view_p);
		if (FAILED(hr)) exit(-1);

		// bind both render target and dept/stencil views to rendering pipeline (OM)
		m_context_p->OMSetRenderTargets(1u, m_rtv_p.GetAddressOf(), m_ds_view_p.Get());


		// SETUP RASTERIZER STATE (default - same as doing nothing) AND VIEWPORT

		// setup rasterizer state options (as in default one provided)
		auto rs_desc = D3D11_RASTERIZER_DESC{};

		rs_desc.AntialiasedLineEnable = false;			// disable antialiasing when drawing lines (?)
		rs_desc.CullMode = D3D11_CULL_BACK;				// face culling mode (standard backface culling)
		rs_desc.DepthBias = 0;							// depth bias (???)
		rs_desc.DepthBiasClamp = 0.0f;					// depth bias clamp (???)
		rs_desc.DepthClipEnable = true;					// enable clamping based on depth comparison
		rs_desc.FillMode = D3D11_FILL_SOLID;			// triangle fill mode (solid vs wireframe)
		rs_desc.FrontCounterClockwise = false;			// do not flip default (ccw) triangle winding direction
		rs_desc.MultisampleEnable = false;				// disable multisampling
		rs_desc.ScissorEnable = false;					// disable scissor checks
		rs_desc.SlopeScaledDepthBias = 0.0f;			// slope scaled depth bias (???)

		// crate rasterizer state based on given description
		hr = m_device_p->CreateRasterizerState(&rs_desc, &m_rasterizer_state_p);
		if (FAILED(hr)) exit(-1);

		// bind created rasterizer state to rendering pipeline
		m_context_p->RSSetState(m_rasterizer_state_p.Get());

		// setup viewport options
		auto vp = D3D11_VIEWPORT{};

		vp.Width = static_cast<float>(m_width);
		vp.Height = static_cast<float>(m_height);
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;

		// bind viewport to rendering pipeline 
		m_context_p->RSSetViewports(1u, &vp);


		// CREATE WORLD AND PROJECTION MATRICES

		// creating fov-based projection matrix used to project 3D scene onto 2D viewport
		m_projection_mx = dxm::XMMatrixPerspectiveFovLH(m_field_of_view, static_cast<float>(m_width) / static_cast<float>(m_height), m_near_plane, m_far_plane);

		// creating alternative matrix for orthographic (perpendicular) projection
		m_orthographic_mx = dxm::XMMatrixOrthographicLH(static_cast<float>(m_width), static_cast<float>(m_height), m_near_plane, m_far_plane);

		// creating world matrix used to convert locations of vertices from object local space to global world space - for now initialized as identity matrix
		m_world_mx = dxm::XMMatrixIdentity();

		// NOTE: also necessary view matrix (transforming vertex position from world space into (camera) view space is provided by camera object
	}

	d3d11_graphics::~d3d11_graphics() noexcept
	{
		// when cleaning up d3d pipeline setup - disable swap chain fullscreen mode if it was used
		if (m_swap_chain_p)
		{
			m_swap_chain_p->SetFullscreenState(FALSE, nullptr);
		}
	}

	void d3d11_graphics::begin_scene(float r, float b, float g, float a) noexcept
	{
		// clears render target and depth/stencil buffers for new scene to be rendered
		// - bacbuffer (rtv) is cleared to given RGBA colour
		// - depth buffer is cleared to 1 (maximal far value)
		// - stencil buffer is cleared to zeros

		const float colour[] = { r, g, b,a };
		m_context_p->ClearRenderTargetView(m_rtv_p.Get(), colour);
		m_context_p->ClearDepthStencilView(m_ds_view_p.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void d3d11_graphics::end_scene() noexcept
	{
		// preset scene drown on the back byffer to the front (using vsync setting
		m_swap_chain_p->Present(static_cast<UINT>(m_is_vsync_enabled), 0);
	}
}