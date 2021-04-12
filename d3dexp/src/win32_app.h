#pragma once

#include "timer.h"
#include "win32_window.h"

namespace d3dexp
{
	class win32_app
	{
	public:
		win32_app();

		win32_app(win32_app const&) = delete;
		win32_app(win32_app &&) = delete;

		win32_app& operator=(win32_app const&) = delete;
		win32_app& operator=(win32_app &&) = delete;

		~win32_app() = default;

	public:
		int run();

	private:
		void frame();

	private:
		win32_window m_window;
		timer m_timer;
	};
}