#include "d3d11_graphics.h"

#include <stdexcept>

namespace d3dexp::bell0bytes
{
	d3d11_graphics::d3d11_graphics()
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

		OutputDebugStringA("D3D11 successfully initialized");
	}
}
