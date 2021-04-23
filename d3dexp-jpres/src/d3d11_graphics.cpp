#include "d3d11_graphics.h"

#include "error_logger.h"

namespace d3dexp
{
	bool d3d11_graphics::initialize(HWND window_h, int width, int height) noexcept
	{
		if (!initialize_d3d11(window_h, width, height))
		{
			return false;
		}

		if (!initialize_shaders())
		{
			return false;
		}
		
		return true;
	}

	void d3d11_graphics::frame() noexcept
	{
		// clear buffer to given colour
		const float background_colour[] = { 0.0f, 1.0f, 1.0f, 1.0f };
		m_context_p->ClearRenderTargetView(m_rtv_p.Get(), background_colour);

		// present back buffer to front
		// NOTE: first argument: 0 - no vsync, 1 - enables vsync
		m_swap_chain_p->Present(0, NULL);
	}

	bool d3d11_graphics::initialize_d3d11(HWND window_h, int width, int height) noexcept
	{
		// DXGI GRAPHICS ADAPTERS

		// enumerate all available graphics adapters
		// NOTE: add enumeration functionality, choosing device by user or auto (by highest dedicated gfx memory)
		auto const& adapters = dxgi_adapter_reader::adapters();

		// check if any adapters are available
		if (adapters.empty())
		{
			error_logger::log("No DXGI adapters found.");
			return false;
		}

		// use default (change later to accomodate choice mechanism)
		auto const& adapter = adapters.front();


		// SWAP CHAIN OPTIONS SETUP

		// creating swap chain description struct
		auto sc_desc = DXGI_SWAP_CHAIN_DESC{};

		sc_desc.BufferDesc.Width = width;                                             // buffer width (window width) (NOTE: if 0, takes window width by default from handle)
		sc_desc.BufferDesc.Height = height;                                           // buffer height (window height) (NOTE: if 0, takes window height by default from handle)
		sc_desc.BufferDesc.RefreshRate.Numerator = 60;                                // refresh rate used - here 60Hz (NOTE: 0 - gives default; for fullscreen should be queried from adapter)
		sc_desc.BufferDesc.RefreshRate.Denominator = 1;                               //                               (NOTE: only applicable if in fullscreen mode with vsync enabled)
		sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                       // pixel format (32-bit, 8 bits per channel RGBA, values in [0.0; 1.0] interval) (vs chili BGRA)
		sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;   // unspecified scanline ordering (for interlacing?)
		sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                   // unspecified buffer scaling mode (none should be required)
		sc_desc.SampleDesc.Count = 1;                                                 // antialiasing sampling count (no antialiasing)
		sc_desc.SampleDesc.Quality = 0;                                               // antialiasing sampling quality level (no antialiasing)
		sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                        // buffer usage as rendering output target
		sc_desc.BufferCount = 1;                                                      // single backbuffer (for double-buffering)
		sc_desc.OutputWindow = window_h;                                              // setting our parent window as output
		sc_desc.Windowed = TRUE;                                                      // use windowed mode (vs fullscreen; for now)
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                                // frontbuffer discarded after swap (can be modernised to use advised flip model if buffer count is 2 or more)
		sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;                       // extra flag - allows window/buffer resizing and change mode from windowed to fullscreen


		// DEV/CTX/SC CREATION

		// create d3d11 device, immediate device context and swap chain
		auto hr = D3D11CreateDeviceAndSwapChain(
			adapter.ptr(),                  // pointer to DXGI adapter to be used (NULL - default in category specified below)
			D3D_DRIVER_TYPE_UNKNOWN,        // driver type (UNKNOWN - since we provided adapter; HARDWARE - to choose proper default if previous is NULL)
			NULL,                           // handle to module (lib) with software driver (if previous set to SOFTWARE)
			NULL,                           // runtime layers flags (currently none used)
			NULL,							// array of feature levels requested
			0,                              // length of above array
			D3D11_SDK_VERSION,              // current sdk version (always this macro)
			&sc_desc,						// struct with swap chain setup options
			&m_swap_chain_p,				// pointer to created swap chain
			&m_device_p,                    // pointer to created device
			NULL,                           // pointer to struct descriibing actually used feature level (currently unused)
			&m_context_p);                  // pointer to created immediate device context
		if (FAILED(hr))
		{
			error_logger::log(hr, "Failed to create d3d11 device and swap chain.");
			return false;
		}


		// RENDER TARGET VIEW
		
		// get pointer to swap chain buffer
		// NOTE: chili uses ID3D11Resource instead of ID3D11Texture2D
		auto back_buffer_p = com_ptr<ID3D11Texture2D>{};
		hr = m_swap_chain_p->GetBuffer(0, __uuidof(ID3D11Texture2D), to_pp(back_buffer_p));
		if (FAILED(hr))
		{
			error_logger::log(hr, "Failed to retrieve back buffer from swap chain.");
			return false;
		}

		// use device to create rtv for swap chain buffer
		hr = m_device_p->CreateRenderTargetView(back_buffer_p.Get(), NULL, &m_rtv_p);
		if (FAILED(hr))
		{
			error_logger::log(hr, "Failed to create render target view for swap chain back buffer.");
			return false;
		}

		// set created render target view as redner target in pipeline's output merger
		m_context_p->OMSetRenderTargets(1, m_rtv_p.GetAddressOf(), nullptr);


		// RS - VIEWPORT

		// create viewport setup struct
		auto viewport = D3D11_VIEWPORT{};

		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);

		// set vievport in rasterizer
		m_context_p->RSSetViewports(1, &viewport);

		return true;
	}

	bool d3d11_graphics::initialize_shaders() noexcept
	{
		// VS - VERTEX SHADER

		// create input layout descriptions array
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",					// semantic name
			  0,							// semantic index
			  DXGI_FORMAT_R32G32_FLOAT,		// data format
			  0,							// input slot index
			  0,							// byte offset - use D3D11_APPEND_ALIGNED_ELEMRNT macro
			  D3D11_INPUT_PER_VERTEX_DATA,  // change for instancing
			  0 }                           // change for instancing
		};

		// initialize vertex shader loaded from given path with created input layout
		if (!m_vs.initialize(m_device_p, L"..\\x64\\Debug\\vs_hello_triangle.cso", layout, ARRAYSIZE(layout)))
		{
			return false;
		}

		return true;
	}
}