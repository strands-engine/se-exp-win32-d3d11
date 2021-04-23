#pragma once

#include <DirectXMath.h>
namespace dxm = DirectX;

namespace d3dexp
{
	struct d3d11_vertex
	{
		d3d11_vertex() noexcept = default;
		d3d11_vertex(float x, float y) : position(x, y) {}

		dxm::XMFLOAT2 position = {};
	};
}