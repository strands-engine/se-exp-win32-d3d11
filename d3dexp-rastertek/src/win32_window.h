#pragma once

#include <memory>

#include "win32_input.h"
#include "win32_graphics.h"

namespace d3dexp
{
	class win32_window
	{
	public:
		win32_window() noexcept = default;

		win32_window(win32_window const&) = delete;
		win32_window(win32_window &&) = delete;

		win32_window& operator=(win32_window const&) = delete;
		win32_window& operator=(win32_window &&) = delete;

		~win32_window() noexcept = default;

	private:
		win32_input m_input;

		std::unique_ptr<win32_graphics> m_graphics_p = nullptr;
	};
}