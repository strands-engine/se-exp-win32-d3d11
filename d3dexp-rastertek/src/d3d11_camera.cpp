#include "d3d11_camera.h"

namespace d3dexp::rastertek
{
    void d3d11_camera::update_view_mx() noexcept
	{
		// calculating view matrix based on following vectors: position, up direction and look-at point
		auto up = dxm::XMFLOAT3{ 0.0f, 1.0f, 0.0f };
		auto look_at = dxm::XMFLOAT3{ 0.0f, 0.0f, 1.0f };

		// load vectors for sse operations
		auto pos_v = dxm::XMLoadFloat3(&m_position);
		auto up_v = dxm::XMLoadFloat3(&up);
		auto look_at_v = dxm::XMLoadFloat3(&look_at);

		// rotation matrix based on yaw/pitch/roll - scaled to radians
		const float deg_to_rad = 0.0174532925f;
		auto rot_m = dxm::XMMatrixRotationRollPitchYaw(m_rotation.x * deg_to_rad, m_rotation.y * deg_to_rad, m_rotation.z * deg_to_rad);

		// transform look_at and up vectors using obtained rottion matrix
		look_at_v = dxm::XMVector3TransformCoord(look_at_v, rot_m);
		up_v = dxm::XMVector3TransformCoord(up_v, rot_m);

		// translate camera position along look at direction to obtain look-at point
		look_at_v = dxm::XMVectorAdd(look_at_v, pos_v);

		// calculate view matrix from obtained vectors
		m_view_mx = dxm::XMMatrixLookAtLH(pos_v, look_at_v, up_v);
	}
}