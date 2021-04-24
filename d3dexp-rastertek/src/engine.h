#pragma once

#include <string>

#include "win32_window.h"

namespace d3dexp
{
	class engine
	{
	public:
		engine(std::wstring const& app_name, win32_graphics_settings_t& settings) noexcept;

		engine(engine const&) = delete;
		engine(engine &&) = delete;

		engine& operator=(engine const&) = delete;
		engine& operator=(engine &&) = delete;

		~engine() noexcept = default;

	public:
		int run() noexcept;

		bool frame() noexcept;

	private:
		win32_window m_window;
	};
}