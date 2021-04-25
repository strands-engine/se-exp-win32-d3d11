#pragma once

#include "d3d11_header_wrapper.h"

namespace d3dexp
{
	class d3d11_model
	{
	private:
		struct vertex_t
		{
			dxm::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			dxm::XMFLOAT4 colour = { 0.0f, 0.0f, 0.0f, 0.0f };

			vertex_t() noexcept = default;
			vertex_t(float x, float y, float z, float r, float g, float b, float a) noexcept
				: position(x, y, z), colour(r, g, b, a) 
			{}
		};

	public:
		d3d11_model() noexcept = default;

		d3d11_model(d3d11_model const&) = delete;
		d3d11_model(d3d11_model &&) = delete;

		d3d11_model& operator=(d3d11_model const&) = delete;
		d3d11_model& operator=(d3d11_model &&) = delete;

		~d3d11_model() noexcept = default;

	public:
	private:
	private:


	};
}