#pragma once

#include "win32_window_container.h"

namespace d3dexp
{
	class engine : public win32_window_container
	{
	public:
		engine() noexcept = default;

		engine(engine const&) = delete;
		engine(engine &&) = delete;

		engine& operator=(engine const&) = delete;
		engine& operator=(engine &&) = delete;

		~engine() noexcept = default;

	public:
		bool initialize(HINSTANCE instance_h, std::string title, std::string class_name, int width, int height) noexcept;
		
		void frame();
		bool process_messages() noexcept;
	};
}