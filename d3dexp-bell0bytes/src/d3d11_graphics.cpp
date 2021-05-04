#include "d3d11_graphics.h"

#include <stdexcept>
#include <fstream>

#include <lua.hpp>
#pragma comment(lib, "liblua54.a") 
#include <sol/sol.hpp>

#include "string_converter.h"
#include "win32_app.h"

namespace d3dexp::bell0bytes
{
	d3d11_graphics::d3d11_graphics(win32_app * app_p)
		: m_app_p(app_p)
	{
		// load settings from config file
		load_settings();

		// DEVICE AND DEVICE CONTEXT INITIALIZATION
		
		// setup device creation flags
		//  - (NEW!) BGRA support - required for interoperability with D2D
		//  - debug - in debug mode enable debug layer for more verbose messages
		auto device_flags = UINT{ D3D11_CREATE_DEVICE_BGRA_SUPPORT };
#ifdef _DEBUG
		device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

		// for storing actually used feature level
		auto feature_level = D3D_FEATURE_LEVEL{};

		// create device and device context
		auto hr = D3D11CreateDevice(
			nullptr,						// use default device
			D3D_DRIVER_TYPE_HARDWARE,		// default device type to be used
			nullptr,						// no need for library (module) for software device
			device_flags,					// device creation flags as set above
			nullptr,						// no desired feature levels
			0,								// number of desired feature levels
			D3D11_SDK_VERSION,				// sdk version - macro
			&m_device_p,					// pointer to created device
			&feature_level,					// output for actually used feature level
			&m_context_p);					// pointer to created immediate device context

		// handling errors
		if (FAILED(hr))
		{
			OutputDebugStringA("Failed to create D3D11 device and context.");
			throw std::runtime_error{ "Failed to create D3D11 device and context." };
		}
		else if (feature_level < D3D_FEATURE_LEVEL_11_0)
		{
			OutputDebugStringA("D3D Version 11 is not supported.");
			throw std::runtime_error{ "Failed to create D3D11 device and context." };
		}

		// as device and context are available, create remaining resources
		if (!create_resources())
		{
			OutputDebugStringA("Creation of D3D11 resources failed.");
			throw std::runtime_error{ "Creation of D3D11 resources failed." };
		}

		// as device and context are available, initialize rendering pipeline
		if (!initialize_pipeline())
		{
			OutputDebugStringA("Initialization of D3D11 pipeline failed.");
			throw std::runtime_error{ "Initialization of D3D11 pipeline failed." };
		}


		OutputDebugStringA("D3D11 successfully initialized.\n");
	}

	d3d11_graphics::~d3d11_graphics() noexcept
	{
		m_swap_chain_p->SetFullscreenState(FALSE, nullptr);
	}

	bool d3d11_graphics::is_sc_fullscreen() const noexcept
	{
		auto result = BOOL{};
		m_swap_chain_p->GetFullscreenState(&result, nullptr);
		return static_cast<bool>(result);
	}


	void d3d11_graphics::clear_buffers()
	{
		// clear back buffer to given colour
		const float colour[] = { 0.0f, 1.0f, 1.0f, 1.0f };
		m_context_p->ClearRenderTargetView(m_rtv_p.Get(), colour);

		// clear depth buffer to 1 and stencil buffer to 0
		m_context_p->ClearDepthStencilView(m_dsv_p.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

	expected_t<int> d3d11_graphics::present() 
	{
		// present backbuffer to the front - no vsync and do not wait for vblank (causes tearing)
		auto hr = m_swap_chain_p->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
		
		// NOTE: may fail due to GPU being busy - do not consider that a critical error
		if (FAILED(hr) && hr != DXGI_ERROR_WAS_STILL_DRAWING)
		{
			OutputDebugStringA("Failed to present the scene.");
			return std::runtime_error{ "Failed to present the scene." };
		}

		// rebinding buffers - necessary when using swap chain flip model, as it releases target views after each call to present
		m_context_p->OMSetRenderTargets(1u, m_rtv_p.GetAddressOf(), m_dsv_p.Get());

		return 0;
	}

	void d3d11_graphics::increase_resolution()
	{
		m_has_to_change_mode = (m_chosen_display_mode_ix < m_display_modes.size() - 1);
		if (m_has_to_change_mode)
		{ 
			m_chosen_display_mode_ix++;
			on_resize();
		}
	}

	void d3d11_graphics::decrease_resolution()
	{
		m_has_to_change_mode = (m_chosen_display_mode_ix > 0);
		if (m_has_to_change_mode)
		{
			m_chosen_display_mode_ix--;
			on_resize();
		}
	}


	expected_t<void> d3d11_graphics::create_resources()
	{
		// SWAP CHAIN CREATION

		// prepare swap chain description struct
		auto sc_desc = DXGI_SWAP_CHAIN_DESC{};

		sc_desc.BufferDesc.Width = 0;													// back buffer width (0 - take value from window)
		sc_desc.BufferDesc.Height = 0;													// back buffer height (0 - take value from window)
		sc_desc.BufferDesc.RefreshRate.Numerator = 0;									// refresh rate - for windowed w/o vsync we do not care (0/1 - note other values not fitting output mode may cause creation of other buffers and copying instead of buffer swapping)
		sc_desc.BufferDesc.RefreshRate.Denominator = 1;
		sc_desc.BufferDesc.Format = m_requested_pixel_format;							// pixel format type (here: BGRA, 8 normalized bits per channel)
		sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		// unspecified scanline ordering
		sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;						// unspecified scaling mode
		sc_desc.SampleDesc.Count = 1;													// parameters for multisampling anti-aliasing (msaa) - here: disabled
		sc_desc.SampleDesc.Quality = 0;
		sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							// usage type (as back buffer)
		sc_desc.BufferCount = 3;														// the number of buffers in the swap chain (including the front buffer, needed for fullscreen?) - here: triple-buffering
		sc_desc.OutputWindow = m_app_p->window_handle();								// set the main window as output target
		sc_desc.Windowed = TRUE;														// windowed mode (NOTE: for fullscreen it is better to create windowed swap chain and then set it to fullscreen)
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;								// flip mode and discared buffer after presentation (available due to larger number of backbuffers?)
		sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;							// extra options - allow mode switching (windowed <-> fullscreen)

		// retrieve underlying dxgi device from d3d11 device - COM cast
		com_ptr<IDXGIDevice> dxgi_device_p = nullptr;
		auto hr = m_device_p.As(&dxgi_device_p);
		if (FAILED(hr)) return std::runtime_error{ "The Direct3D device was unable to retrieve the underlying DXGI device!" };

		// obtain adapter used by this device
		com_ptr<IDXGIAdapter> dxgi_adapter_p = nullptr;
		hr = dxgi_device_p->GetAdapter(&dxgi_adapter_p);
		if (FAILED(hr)) return std::runtime_error{ "The DXGI Device was unable to get the GPU adapter!" };

		// retrieve dxgi factory that created given adapter interface
		com_ptr<IDXGIFactory> dxgi_factory_p = nullptr;
		hr = dxgi_adapter_p->GetParent(__uuidof(IDXGIFactory), &dxgi_factory_p);
		if (FAILED(hr)) return std::runtime_error{ "The DXGI Adapter was unable to get the factory!" };

		// create swap chain as described above
		hr = dxgi_factory_p->CreateSwapChain(m_device_p.Get(), &sc_desc, &m_swap_chain_p);
		if (FAILED(hr)) return std::runtime_error{ "The creation of the swap chain failed!" };


		// get underlying output for display mode enumeration
		auto output_p = com_ptr<IDXGIOutput>{};
		hr = m_swap_chain_p->GetContainingOutput(&output_p);
		if (FAILED(hr)) return std::runtime_error{ "Unable to retrieve swap chain output." };

		// get array of supported display modes
		auto modes_count = UINT{};
		output_p->GetDisplayModeList(m_requested_pixel_format, 0u, &modes_count, nullptr);
		if (FAILED(hr)) return std::runtime_error{ "Unable to list all supported display modes." };
		m_display_modes.resize(modes_count);
		output_p->GetDisplayModeList(m_requested_pixel_format, 0u, &modes_count, m_display_modes.data());
		if (FAILED(hr)) return std::runtime_error{ "Unable to list all supported display modes." };

		// if currently chosen resolution is not supported default to the smallest one
		auto is_res_supported = false;
		for (auto i = std::size_t{}; i < modes_count; i++)
		{
			if (m_app_p->window_width() == m_display_modes[i].Width && m_app_p->window_height() == m_display_modes[i].Height)
			{
				is_res_supported = true;
				m_chosen_display_mode_ix = i;
				break;
			}
		}

		if (!is_res_supported)
		{
			OutputDebugStringA("Desired resolution not supported. Resizing to samllest one..."); // index remains equal to 0

			hr = m_swap_chain_p->ResizeTarget(&(m_display_modes[m_chosen_display_mode_ix]));
			if (FAILED(hr)) return std::runtime_error("Unable to resize target to a supported display mode.");

			auto result = write_display_mode_to_config();
			if (!result) return std::runtime_error{ "Unable to write t configuration file." };
		}

		// start in fullscreen mode if requested
		if (m_is_starting_in_fullscreen)
		{
			hr = m_swap_chain_p->SetFullscreenState(true, nullptr);
			if (FAILED(hr)) return std::runtime_error{ "Unable to switch to fullscreen mode!" };
			m_is_fullscreen = true;
		}


		// complete other maintenance steps required after every resize
		if (!on_resize())
		{
			return std::runtime_error{ "Direct3D was unable to resize its resources!" };
		}

		return {};
	}

	expected_t<void> d3d11_graphics::initialize_pipeline()
	{
		// loading shader data
		auto vs_result = load_shader(L"D:\\repos\\se-exp-win32-d3d11\\x64\\Debug\\vs_hello_triangle.cso");
		auto ps_result = load_shader(L"D:\\repos\\se-exp-win32-d3d11\\x64\\Debug\\ps_hello_triangle.cso");

		if (!vs_result || !ps_result) return std::runtime_error{ "Failed to load vertex and pixel shader bytecode." };

		// create vertex shader object
		auto hr = m_device_p->CreateVertexShader(vs_result->data, vs_result->size, nullptr, &m_vs_p);
		if (FAILED(hr)) return std::runtime_error{ "Failed to create D3D11 vertex shader object" };

		// create pixel shader object
		hr = m_device_p->CreatePixelShader(ps_result->data, ps_result->size, nullptr, &m_ps_p);
		if (FAILED(hr)) return std::runtime_error{ "Failed to create D3D11 pixel shader object" };

		// bind shaders to pipeline
		m_context_p->VSSetShader(m_vs_p.Get(), nullptr, 0u);
		m_context_p->PSSetShader(m_ps_p.Get(), nullptr, 0u);

		// define vertex input layout
		D3D11_INPUT_ELEMENT_DESC layout[] = 
		{ 
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOUR",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// create input layout object
		hr = m_device_p->CreateInputLayout(layout, ARRAYSIZE(layout), vs_result->data, vs_result->size, &m_layout_p);
		if (FAILED(hr)) return std::runtime_error{ "Failed to create vertex input layout." };

		// bind layout to pipeline
		m_context_p->IASetInputLayout(m_layout_p.Get());

		// release data from shader data buffer
		if (vs_result->data) delete[] vs_result->data;
		if (ps_result->data) delete[] ps_result->data;

		OutputDebugStringA("Initialized D3D11 pipeline.\n");

		return {};
	}

	// NOTE: If window dimensions were not changed, entire d3d stuf fshould not be recreated (???)
	expected_t<void> d3d11_graphics::on_resize()
	{
		auto hr = HRESULT{};

		// FIGURE OUT FULLSCREEN RELATED ISSUES

		// zero out current mode refresh rate (as advised by Microsoft docs)
		auto current_display_mode = m_display_modes[m_chosen_display_mode_ix];
		current_display_mode.RefreshRate.Numerator = 0;
		current_display_mode.RefreshRate.Denominator = 0;
		
		// check if mode changed and perform switch
		if (is_fullscreen() != is_sc_fullscreen())
		{
			// switch modes accordingly
			if (is_sc_fullscreen())
			{
				// switch to fullscreen - MSDN recommends resizing the target before going to fullscreen
				hr = m_swap_chain_p->ResizeTarget(&current_display_mode);
				if (FAILED(hr)) return std::runtime_error{ "Unable to resize target!" };

				// set state to fullscreen
				hr = m_swap_chain_p->SetFullscreenState(true, nullptr);
				if (FAILED(hr)) return std::runtime_error{ "Unable to switch to fullscreen mode!" };
			}
			else
			{
				// switch state to windowed
				hr = m_swap_chain_p->SetFullscreenState(false, nullptr);
				if (FAILED(hr)) return std::runtime_error{ "Unable to switch to fullscreen mode!" };

				// recompute and set new window size according to desired client size
				auto wnd_rect = RECT{ 0, 0, m_app_p->display_mode_width(), m_app_p->display_mode_height() };
				auto awr_res = AdjustWindowRectEx(&wnd_rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
				if (!awr_res) return std::runtime_error{ "Failed to adjust window rectangle!" };
				SetWindowPos(m_app_p->window_handle(), HWND_TOP, 0, 0, wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top, SWP_NOMOVE);
			}

			m_is_fullscreen = !m_is_fullscreen;
		}

		// resize target (again?)
		hr = m_swap_chain_p->ResizeTarget(&current_display_mode);
		if (FAILED(hr)) return std::runtime_error{ "Unable to resize target!" };

		// clear context state
		m_app_p->clear_2d_target();
		m_context_p->ClearState();
		m_rtv_p.Reset();
		m_dsv_p.Reset();

		// resize swap chain buffers to comply with current window size
		hr = m_swap_chain_p->ResizeBuffers(
			0,											// number of buffers (0 - all)												
			0,											// buffer width (0 - determined from the window)
			0,											// buffer height (0 - determined from the window)
			DXGI_FORMAT_UNKNOWN,						// pixel format (UNKNOWN - do not change)
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);	// change flags - allow mode change
		if (FAILED(hr)) return std::runtime_error{ "Direct3D was unable to resize the swap chain!" };

		// recreate render target view for new back buffer texture resource
		auto back_buffer_p = com_ptr<ID3D11Texture2D>{};
		hr = m_swap_chain_p->GetBuffer(0, __uuidof(ID3D11Texture2D), to_pp(back_buffer_p));
		if (FAILED(hr)) return std::runtime_error{ "Direct3D was unable to acquire the back buffer!" };
		hr = m_device_p->CreateRenderTargetView(back_buffer_p.Get(), nullptr, &m_rtv_p);
		if (FAILED(hr)) return std::runtime_error{ "Direct3D was unable to create the render target view!" };

		// recreate depth/stencil buffer and a view for it

		// setup buffer description
		// NOTE: we take description of backbuffer as it shares most option with what we require for deph/stencil buffer
		auto ds_desc = D3D11_TEXTURE2D_DESC{};
		back_buffer_p->GetDesc(&ds_desc);
		ds_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ds_desc.Usage = D3D11_USAGE_DEFAULT;
		ds_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		// create ds buffer and view
		auto ds_buffer_p = com_ptr<ID3D11Texture2D>{};
		hr = m_device_p->CreateTexture2D(&ds_desc, nullptr, &ds_buffer_p);
		if (FAILED(hr)) return std::runtime_error("Direct3D was unable to create a 2D-texture!");
		hr = m_device_p->CreateDepthStencilView(ds_buffer_p.Get(), nullptr, &m_dsv_p);
		if (FAILED(hr)) return std::runtime_error("Direct3D was unable to create the depth and stencil buffer!");

		// bind render target view and dept/stencil view to pipeline (OM)
		m_context_p->OMSetRenderTargets(1u, m_rtv_p.GetAddressOf(), m_dsv_p.Get());

		// setup vievport options and set it in the pipeline (RS)
		auto vp = D3D11_VIEWPORT{};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = static_cast<float>(ds_desc.Width);
		vp.Height = static_cast<float>(ds_desc.Height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		m_context_p->RSSetViewports(1u, &vp);

		// rebind shaders and input layout
		m_context_p->VSSetShader(m_vs_p.Get(), nullptr, 0u);
		m_context_p->PSSetShader(m_ps_p.Get(), nullptr, 0u);
		m_context_p->IASetInputLayout(m_layout_p.Get());

		// adjust d2d target surface appropriately
		auto result = m_app_p->resize_2d_surface();
		if (!result) return std::runtime_error{ "Failed to reside D2D target surface." };

		return {};
	}

	void d3d11_graphics::load_settings()
	{
		auto path = m_app_p->settings_path() + L"\\settings.lua";
		try
		{
			auto lua = sol::state{};
			lua.script_file(string_converter::to_string(path));

			// read from the configuration file, default to 800 x 600
			m_is_starting_in_fullscreen = lua["config"]["fullscreen"].get_or(false);
		}
		catch (std::exception)
		{
			OutputDebugStringA("Failed to load d3d11 settings. Using default windowed mode.");
		}
	}

	expected_t<void> d3d11_graphics::write_display_mode_to_config()
	{
		// BLAH !!!
		return {};
	}


	expected_t<shader_buffer_t> d3d11_graphics::load_shader(std::wstring const& path)
	{
		auto buffer = shader_buffer_t{};

		// open file containing shader bytecode
		auto fin = std::ifstream{ path, std::ios::in | std::ios::binary | std::ios::ate };
		if (fin.is_open())
		{
			// get file size
			buffer.size = fin.tellg();

			// read binary data
			buffer.data = new BYTE[buffer.size];
			fin.seekg(0, std::ios::beg);
			fin.read(reinterpret_cast<char*>(buffer.data), buffer.size);
			fin.close();
		}
		else
		{
			return std::runtime_error{ "Unable to open the compiled shader object!" };
		}

		return buffer;
	}

}
