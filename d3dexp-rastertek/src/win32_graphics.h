#pragma once

#include "d3d11_graphics.h"

namespace d3dexp
{
	class win32_graphics
	{
	public:
		win32_graphics(HWND window_h, d3d11_graphics_settings_t& settings) noexcept;

		win32_graphics(win32_graphics const&) = delete;
		win32_graphics(win32_graphics &&) = delete;

		win32_graphics& operator=(win32_graphics const&) = delete;
		win32_graphics& operator=(win32_graphics &&) = delete;

		~win32_graphics() noexcept = default;

	public:
		bool frame() noexcept;

	private:
		bool render() noexcept;

	private:
		d3d11_graphics m_d3dgfx;
	};
}