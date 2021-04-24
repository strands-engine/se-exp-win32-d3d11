#pragma once

#include <memory>

#include <Windows.h>

#include "d3d11_camera.h"
#include "d3d11_color_shader.h"
#include "d3d11_model.h"

namespace d3dexp
{
	class d3d11_graphics
	{
	public:
		d3d11_graphics() noexcept = default;

		d3d11_graphics(d3d11_graphics const&) = delete;
		d3d11_graphics(d3d11_graphics &&) = delete;

		d3d11_graphics& operator=(d3d11_graphics const&) = delete;
		d3d11_graphics& operator=(d3d11_graphics &&) = delete;

		~d3d11_graphics() noexcept = default;

	private:
		d3d11_camera m_camera;

		std::unique_ptr<d3d11_color_shader> m_shader_p = nullptr;
		std::unique_ptr<d3d11_model> m_model_p = nullptr;
	};
}