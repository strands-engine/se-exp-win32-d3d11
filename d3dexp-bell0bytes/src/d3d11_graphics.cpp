#include "d3d11_graphics.h"

#include <stdexcept>

#include "win32_app.h"

namespace d3dexp::bell0bytes
{
	d3d11_graphics::d3d11_graphics(win32_app * app_p)
		: m_app_p(app_p)
	{
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

		OutputDebugStringA("D3D11 successfully initialized");
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

		return 0;
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

		// complete other maintenance steps required after every resize
		if (!on_resize())
		{
			return std::runtime_error{ "Direct3D was unable to resize its resources!" };
		}

		return {};
	}

	expected_t<void> d3d11_graphics::on_resize()
	{
		// resize swap chain buffers to comply with current window size
		auto hr = m_swap_chain_p->ResizeBuffers(
			0,											// number of buffers (0 - all)												
			0,											// buffer width (0 - determined from the window)
			0,											// buffer height (0 - determined from the window)
			DXGI_FORMAT_UNKNOWN,						// pixel format (UNKNOWN - do not change)
			0);											// change flags - no changes
		if (FAILED(hr)) return std::runtime_error{ "Direct3D was unable to resize the swap chain!" };

		return {};
	}

}
