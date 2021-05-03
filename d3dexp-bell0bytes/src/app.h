#pragma once

#include "win32_app.h"

namespace d3dexp::bell0bytes
{
	class app : public win32_app
	{
	public:
		friend d3d11_graphics;

	public:
		app(HINSTANCE instance_h) : win32_app(instance_h) {}

		app(app const&) = delete;
		app(app &&) = delete;

		app& operator=(app const&) = delete;
		app& operator=(app &&) = delete;

		~app() = default;

	public:
		expected_t<void> initialize() override;
		void shutdown(expected_t<void>* expected = nullptr) override;

		void update(double dt) override;
		expected_t<int> render(double farseer) override;

		void on_key_down(WPARAM wparam, LPARAM lparam) override;

		expected_t<int> run();

	private:
		expected_t<void> initialize_graphics();

	private:
		com_ptr<ID3D11Buffer> m_vertex_buffer_p = nullptr;
	};
}