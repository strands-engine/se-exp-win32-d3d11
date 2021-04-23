#pragma once

#include "d3d11_graphics.h"

namespace d3dexp
{
	class win32_graphics
	{
	public:
		win32_graphics() noexcept = default;

		win32_graphics(win32_graphics const&) = delete;
		win32_graphics(win32_graphics &&) = delete;

		win32_graphics& operator=(win32_graphics const&) = delete;
		win32_graphics& operator=(win32_graphics &&) = delete;

		~win32_graphics() noexcept = default;

	private:
		d3d11_graphics m_d3dgfx;
	};
}