#include "app.h"

#include <string>

namespace d3dexp::bell0bytes
{
	expected_t<void> app::initialize()
	{
		auto result = win32_app::initialize();
		if (!result)
		{
			return result;
		}

		result = initialize_graphics();
		if (!result)
		{
			return result;
		}

		OutputDebugStringA("App successfully initialized.\n");

		return {};
	}

	void app::shutdown(expected_t<void>* expected)
	{
		if (expected != nullptr && !expected->is_valid())
		{
			try
			{
				expected->throw_if_failed();
			}
			catch (std::exception& e)
			{
				auto msg = std::string{ "Exception thrown in app: " } + e.what() + "\n";
				OutputDebugStringA(msg.c_str());
			}
		}

		OutputDebugStringA("App shutdown successful.\n");
	}

	void app::update(double dt)
	{
		win32_app::update(dt);
	}

	expected_t<int> app::render(double farseer)
	{
		// clear buffers to prepare for drawing new frame
		m_graphics_p->clear_buffers();

		// rendering

		// display fps info
		auto result = m_graphics_2d_p->render_text(d2d1_graphics::context_t::fps, d2d1_graphics::colour_t::black);
		if (!result)
		{
			return std::runtime_error{ "Failed to write fps information!" };
		}

		// RENDER SCENE

		// rendering hello triangle - setting vertex buffer
		auto stride = UINT{ sizeof(vertex_t) };
		auto offset = UINT{ 0u };
		m_graphics_p->context()->IASetVertexBuffers(0u, 1u, m_vertex_buffer_p.GetAddressOf(), &stride, &offset);

		// rendering hello triangle - setting primitive topology
		m_graphics_p->context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// rendering hello triangle - drawing
		m_graphics_p->context()->Draw(3u, 0u);

		// presents drawn frame to front buffer
		return win32_app::render(farseer);
	}

	void app::on_key_down(WPARAM wparam, LPARAM lparam)
	{
		// handle app specific key inputs

		win32_app::on_key_down(wparam, lparam);
	}

	expected_t<int> app::run()
	{
		return win32_app::run();
	}

	expected_t<void> app::initialize_graphics()
	{
		// initialize array of vertices for triangle
		const vertex_t vertices[] =
		{
			{ 0.0f,  0.5f, 0.0f,   0.5f, 1.0f, 0.5f },
			{ 0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f },
			{-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 1.0f },
		};

		// prepare description for vertex buffer
		auto vb_desc = D3D11_BUFFER_DESC{};
		vb_desc.ByteWidth = sizeof(vertex_t) * ARRAYSIZE(vertices);
		vb_desc.Usage = D3D11_USAGE_DEFAULT;
		vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vb_desc.CPUAccessFlags = 0u;
		vb_desc.MiscFlags = 0u;
		vb_desc.StructureByteStride = 0u;

		// setup subresource data descriptor struct
		auto vb_data = D3D11_SUBRESOURCE_DATA{ vertices, 0u, 0u };

		// create vertex buffer with triangle vertex data and prepared description
		auto hr = m_graphics_p->device()->CreateBuffer(&vb_desc, &vb_data, &m_vertex_buffer_p);
		if (FAILED(hr)) return std::runtime_error{ "Unable to create vertex buffer!" };

		return {};
	}
}
