#include "d3d11_graphics.h"

namespace d3dexp
{
	bool d3d11_graphics::initialize(HWND window_h, int width, int height) noexcept
	{
		if (!initialize_d3d11(window_h, width, height))
		{
			return false;
		}

		return true;
	}

	bool d3d11_graphics::initialize_d3d11(HWND window_h, int width, int height) noexcept
	{
		// enumerate all available graphics adapters
		// NOTE: add enumeration functionality, choosing device by user or auto (by highest dedicated gfx memory)
		auto const& adapters = dxgi_adapter_reader::adapters();

		// use default
		auto const& adaper = adapters.front();

		return true;
	}
}