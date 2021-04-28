#pragma once

#include <memory>
#include <Windows.h>

#include "win32_window.h"

namespace d3dexp::bell0bytes
{
	class win32_app
	{
	public:
		friend class win32_window;

	protected:
		win32_app(HINSTANCE instance_h) : m_instance_h(instance_h) {}

		win32_app(win32_app const&) = delete;
		win32_app(win32_app &&) = delete;

		win32_app& operator=(win32_app const&) = delete;
		win32_app& operator=(win32_app &&) = delete;

		~win32_app();

	protected:
		expected_t<int> run();

	protected:
		virtual expected_t<void> initialize();
		virtual void shutdown(expected_t<void>* expected = nullptr);

	protected:
		HINSTANCE m_instance_h = NULL;
		std::unique_ptr<win32_window> m_window_p = nullptr;
	};
}
