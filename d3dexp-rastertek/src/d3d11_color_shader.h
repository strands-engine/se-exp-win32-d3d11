#pragma once

namespace d3dexp
{
	class d3d11_color_shader
	{
	public:
		d3d11_color_shader() noexcept = default;

		d3d11_color_shader(d3d11_color_shader const&) = delete;
		d3d11_color_shader(d3d11_color_shader &&) = delete;

		d3d11_color_shader& operator=(d3d11_color_shader const&) = delete;
		d3d11_color_shader& operator=(d3d11_color_shader &&) = delete;

		~d3d11_color_shader() noexcept = default;
	};
}