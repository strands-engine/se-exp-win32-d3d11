#pragma once

#include <array>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace d3dexp
{
	class win32_input
	{
	public:
		win32_input() noexcept = default;

		win32_input(win32_input const&) = delete;
		win32_input(win32_input &&) = delete;

		win32_input& operator=(win32_input const&) = delete;
		win32_input& operator=(win32_input &&) = delete;

		~win32_input() noexcept = default;

	public:
		[[nodiscard]] bool is_key_pressed(unsigned char keycode) const noexcept { return m_key_states[keycode]; }

		void on_key_pressed(unsigned char keycode) noexcept { m_key_states[keycode] = true; }
		void on_key_releassed(unsigned char keycode) noexcept { m_key_states[keycode] = false; }

	private:
		std::array<bool, 256> m_key_states = std::array<bool, 256>{};
	};
}