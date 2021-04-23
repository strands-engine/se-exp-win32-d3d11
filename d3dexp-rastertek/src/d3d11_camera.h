#pragma once

namespace d3dexp
{
	class d3d11_camera
	{
	public:
		d3d11_camera() noexcept = default;

		d3d11_camera(d3d11_camera const&) = delete;
		d3d11_camera(d3d11_camera &&) = delete;
		
		d3d11_camera& operator=(d3d11_camera const&) = delete;
		d3d11_camera& operator=(d3d11_camera &&) = delete;

		~d3d11_camera() noexcept = default;
	};
}