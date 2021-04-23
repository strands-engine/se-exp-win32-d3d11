#pragma once

#include "win32_window.h"

namespace d3dexp
{
	class engine
	{
	public:
		engine() noexcept = default;

		engine(engine const&) = delete;
		engine(engine &&) = delete;

		engine& operator=(engine const&) = delete;
		engine& operator=(engine &&) = delete;

		~engine() noexcept = default;

	public:
		int run() noexcept;

	private:
		win32_window m_window;
	};
}