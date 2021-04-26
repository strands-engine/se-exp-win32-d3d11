#pragma once

#include "d3d11_header_wrapper.h"

namespace d3dexp::rastertek
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
		d3d11_model(ID3D11Device * device_p) noexcept;

		d3d11_model(d3d11_model const&) = delete;
		d3d11_model(d3d11_model &&) = delete;

		d3d11_model& operator=(d3d11_model const&) = delete;
		d3d11_model& operator=(d3d11_model &&) = delete;

		~d3d11_model() noexcept = default;

	public:
		[[nodiscard]] int vertex_count() const noexcept { return m_vertex_count; }
		[[nodiscard]] int index_count() const noexcept { return m_index_count; }

		void render(ID3D11DeviceContext* context_p) noexcept;

	private:
		bool initialize_buffers(ID3D11Device* device_p) noexcept;

		void render_buffers(ID3D11DeviceContext* context_p) noexcept;

	private:
		com_ptr<ID3D11Buffer> m_vertex_buffer_p = nullptr;
		com_ptr<ID3D11Buffer> m_index_buffer_p = nullptr;

		int m_vertex_count = 0;			// unsigned int preferred
		int m_index_count = 0;
	};
}