#include "d3d_graphics.h"

#pragma comment(lib, "d3d11.lib")

namespace d3dexp
{
	d3d_graphics::d3d_graphics(HWND window_h)
	{
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
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                                // frontbuffer discarded after swap
		sc_desc.Flags = 0;                                                            // no extra option flags

		// create: device, device context, front and back buffers and swap chain
		D3D11CreateDeviceAndSwapChain(
			nullptr,                       // use default graphics adapter (provide choice)
			D3D_DRIVER_TYPE_HARDWARE,      // hardware driver type (vs software or reference)
			nullptr,                       // relevant only for software drivers
			0,                             // no extra flags
			nullptr,                       // no preferred feture levels (use highest compatible)
			0,                             // legth of previous array
			D3D11_SDK_VERSION,             // SDK version used
			&sc_desc,                      // swap chain settings (filled above)
			&m_swap_chain_p,               // pointer to newly created swap chain COM object
			&m_device_p,                   // pointer to newly created D3D device
			nullptr,                       // not capturing actually used feature level (get later)
			&m_context_p                   // pointer to newly created D3D (immediate) context			
		);
	}

	d3d_graphics::~d3d_graphics() noexcept
	{
		if (m_context_p) m_context_p->Release();
		if (m_swap_chain_p) m_swap_chain_p->Release();
		if (m_device_p) m_device_p->Release();
	}
}

