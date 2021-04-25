#include "d3d11_model.h"

namespace d3dexp
{
    d3d11_model::d3d11_model(ID3D11Device* device_p) noexcept
    {
        if (!initialize_buffers(device_p)) exit(-1);
    }

    void d3d11_model::render(ID3D11DeviceContext* context_p) noexcept
    {
        render_buffers(context_p);
    }

    bool d3d11_model::initialize_buffers(ID3D11Device* device_p) noexcept
	{
		// setup vertex and index data and counts
		const vertex_t vertices[] =
		{
			{ 0.0f,  0.5f, 0.0f,    0.5f, 1.0f, 0.5f, 1.0f},
			{ 0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 0.0f, 1.0f},
			{-0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 1.0f, 1.0f},
		};
		const unsigned long indices[] = { 0,1,2 };
		m_vertex_count = ARRAYSIZE(vertices);
		m_index_count = ARRAYSIZE(indices);


		// CREATE VERTEX BUFFER

		// setup settings for vertex buffer
		auto vb_desc = D3D11_BUFFER_DESC{};

		vb_desc.Usage = D3D11_USAGE_DEFAULT;								// default usage type
		vb_desc.ByteWidth = sizeof(vertex_t) * m_vertex_count;              // total byte size (element sise * element count)
		vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// usage as vertex buffer
		vb_desc.CPUAccessFlags = 0;											// no extra cpu access flags
		vb_desc.MiscFlags = 0;												// no extra flags
		vb_desc.StructureByteStride = 0;									// no stride

		// setup subresource data with crated vertex data
		auto vb_data = D3D11_SUBRESOURCE_DATA{};

		vb_data.pSysMem = vertices;
		vb_data.SysMemPitch = 0;
		vb_data.SysMemSlicePitch = 0;

		// create vertex buffer with given vertex data
		auto hr = device_p->CreateBuffer(&vb_desc, &vb_data, &m_vertex_buffer_p);
		if (FAILED(hr)) return false;


		// CREATE INDEX BUFFER

		// setup settings for index buffer
		auto ib_desc = D3D11_BUFFER_DESC{};

		ib_desc.Usage = D3D11_USAGE_DEFAULT;								// default usage type
		ib_desc.ByteWidth = sizeof(unsigned long) * m_index_count;          // total byte size (element sise * element count)
		ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;						// usage as index buffer
		ib_desc.CPUAccessFlags = 0;											// no extra cpu access flags
		ib_desc.MiscFlags = 0;												// no extra flags
		ib_desc.StructureByteStride = 0;									// no stride

		// setup subresource data with crated index data
		auto ib_data = D3D11_SUBRESOURCE_DATA{};

		ib_data.pSysMem = indices;
		ib_data.SysMemPitch = 0;
		ib_data.SysMemSlicePitch = 0;

		// create index buffer with given index data
		auto hr = device_p->CreateBuffer(&ib_desc, &ib_data, &m_index_buffer_p);
		if (FAILED(hr)) return false;


		return true;
	}

    void d3d11_model::render_buffers(ID3D11DeviceContext* context_p) noexcept
    {
		// bind vertex buffer to pipeline
		auto stride = UINT{ sizeof(vertex_t) };
		auto offset = UINT{ 0u };
		context_p->IAGetVertexBuffers(0u, 1u, m_vertex_buffer_p.GetAddressOf(), &stride, &offset);

		// bind index buffer to pieline
		context_p->IASetIndexBuffer(m_index_buffer_p.Get(), DXGI_FORMAT_R32_UINT, 0u);

		// set primitive topology type
		context_p->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
}