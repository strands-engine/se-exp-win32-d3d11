#include "d3d11_graphics.h"

#include "d3d11_exception.h"

#pragma comment(lib, "d3d11.lib")

namespace d3dexp
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
}

