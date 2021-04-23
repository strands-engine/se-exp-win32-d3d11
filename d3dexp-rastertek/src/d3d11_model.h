#pragma once

namespace d3dexp
{
	class d3d11_model
	{
	public:
		d3d11_model() noexcept = default;

		d3d11_model(d3d11_model const&) = delete;
		d3d11_model(d3d11_model &&) = delete;

		d3d11_model& operator=(d3d11_model const&) = delete;
		d3d11_model& operator=(d3d11_model &&) = delete;

		~d3d11_model() noexcept = default;
	};
}