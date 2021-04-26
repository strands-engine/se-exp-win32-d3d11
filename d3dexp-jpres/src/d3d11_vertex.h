#pragma once

#include <DirectXMath.h>
namespace dxm = DirectX;

namespace d3dexp::jpres
{
	struct d3d11_vertex_t
	{
		d3d11_vertex_t() noexcept = default;
		d3d11_vertex_t(float x, float y, float z, float r, float g, float b) : position(x, y, z), colour(r, g, b) {}

		dxm::XMFLOAT3 position = {};
		dxm::XMFLOAT3 colour = {};
	};
}