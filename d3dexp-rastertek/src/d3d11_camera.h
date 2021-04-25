#pragma once

#include "d3d11_header_wrapper.h"

namespace d3dexp
{
	class d3d11_camera
	{
	public:
		d3d11_camera() noexcept = default;
		d3d11_camera(dxm::XMFLOAT3 position, dxm::XMFLOAT3 rotation) noexcept : m_position(position), m_rotation(rotation) { update_view_mx(); }

		d3d11_camera(d3d11_camera const&) = delete;
		d3d11_camera(d3d11_camera &&) = delete;
		
		d3d11_camera& operator=(d3d11_camera const&) = delete;
		d3d11_camera& operator=(d3d11_camera &&) = delete;

		~d3d11_camera() noexcept = default;

	public:
		[[nodiscard]] dxm::XMFLOAT3 const& position() const noexcept { return m_position; }
		[[nodiscard]] dxm::XMFLOAT3& position() noexcept { return m_position; }
		[[nodiscard]] dxm::XMFLOAT3 const& rotation() const noexcept { return m_rotation; }
		[[nodiscard]] dxm::XMFLOAT3& rotation() noexcept { return m_rotation; }
		[[nodiscard]] dxm::XMMATRIX const& view() const noexcept { return m_view_mx; }
		[[nodiscard]] dxm::XMMATRIX& view() noexcept { return m_view_mx; }

		void set_position(float x, float y, float z) noexcept { m_position = { x, y, z }; update_view_mx(); }
		void set_position(dxm::XMFLOAT3 position) noexcept { m_position = position; update_view_mx(); }
		void set_rotation(float x, float y, float z) noexcept { m_rotation = { x, y, z }; update_view_mx(); }
		void set_rotation(dxm::XMFLOAT3 rotation) noexcept { m_rotation = rotation; update_view_mx(); }

		void update(dxm::XMFLOAT3 position, dxm::XMFLOAT3 rotation) noexcept { m_position = position; m_rotation = rotation; update_view_mx(); }

	private:
		void update_view_mx() noexcept;

	private:
		dxm::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
		dxm::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };
		dxm::XMMATRIX m_view_mx = {};
	};
}