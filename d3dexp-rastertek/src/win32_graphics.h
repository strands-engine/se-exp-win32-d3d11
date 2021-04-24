#pragma once

#include "d3d11_graphics.h"

namespace d3dexp
{
	struct win32_graphics_settings_t
	{
		float screen_near_plane = 1000.0f;
		float screen_far_plane = 0.1f;
		int width = 800;
		int height = 600;
		bool is_vsync_enabled = true;
		bool is_fullscreen = false;
	};

	class win32_graphics
	{
	public:
		win32_graphics(HWND window_h, win32_graphics_settings_t& settings) noexcept;

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