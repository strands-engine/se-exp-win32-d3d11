#pragma once

#include "d3d11_graphics.h"
#include "d3d11_camera.h"
#include "d3d11_colour_shader.h"
#include "d3d11_model.h"

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
		d3d11_camera m_camera;

		std::unique_ptr<d3d11_colour_shader> m_shader_p = nullptr;
		std::unique_ptr<d3d11_model> m_model_p = nullptr;
	};
}