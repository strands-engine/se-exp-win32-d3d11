#pragma once

#include <queue>
#include <array>

#include "win32_keyboard_event.h"

namespace d3dexp::jpres
{
	class win32_keyboard
	{
	public:
		win32_keyboard() noexcept = default;

		win32_keyboard(win32_keyboard const&) = delete;
		win32_keyboard(win32_keyboard &&) = delete;

		win32_keyboard& operator=(win32_keyboard const&) = delete;
		win32_keyboard& operator=(win32_keyboard &&) = delete;

		~win32_keyboard() noexcept = default;

	public:
		[[nodiscard]] bool is_key_pressed(unsigned char keycode) const noexcept { return m_key_states[keycode]; }

		[[nodiscard]] bool is_key_queue_empty() const noexcept { return m_key_queue.empty(); }
		[[nodiscard]] bool is_char_queue_empty() const noexcept { return m_char_queue.empty(); }

		win32_keyboard_event read_key() noexcept;
		unsigned char read_char() noexcept;

		[[nodiscard]] bool is_key_autorepeat_enabled() const noexcept { return m_is_key_autorepeat_enabled; }
		[[nodiscard]] bool is_char_autorepeat_enabled() const noexcept { return m_is_char_autorepeat_enabled; }

		void enable_key_autorepeat() noexcept { m_is_key_autorepeat_enabled = true; }
		void disable_key_autorepeat() noexcept { m_is_key_autorepeat_enabled = false; }
		void enable_char_autorepeat() noexcept { m_is_char_autorepeat_enabled = true; }
		void disable_char_autorepeat() noexcept { m_is_char_autorepeat_enabled = false; }

		void on_key_pressed(unsigned char keycode) noexcept;
		void on_key_released(unsigned char keycode) noexcept;
		void on_char(unsigned char character) noexcept;

	private:
		std::queue<win32_keyboard_event> m_key_queue;
		std::queue<unsigned char> m_char_queue;
	
		std::array<bool, 256> m_key_states = std::array<bool, 256>{};

		bool m_is_key_autorepeat_enabled = false;
		bool m_is_char_autorepeat_enabled = false;
	};
}