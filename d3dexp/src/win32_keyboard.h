#pragma once

#include <queue>
#include <bitset>

#include "input_types.h"

namespace d3dexp {

	class win32_keyboard
	{
		friend class win32_window;

	public:
		class win32_keyboard_event
		{
		public:
			enum class action_t
			{
				pressed,
				released,
				invalid
			};

		public:
			win32_keyboard_event() noexcept : m_action(action_t::invalid), m_key(key_t::undefined) {}
			win32_keyboard_event(action_t action, key_t key) noexcept : m_action(action), m_key(key) {}

		public:
			[[nodiscard]] action_t action() const noexcept { return m_action; }
			[[nodiscard]] key_t key() const noexcept { return m_key; }
			[[nodiscard]] bool is_pressed() const noexcept { return m_action == action_t::pressed; }
			[[nodiscard]] bool is_released() const noexcept { return m_action == action_t::released; }
			[[nodiscard]] bool is_valid() const noexcept { return m_action != action_t::invalid; }

		private:
			action_t m_action;
			key_t m_key;
		};

	public:
		win32_keyboard() noexcept {};

		win32_keyboard(win32_keyboard const&) = delete;
		win32_keyboard(win32_keyboard&&) = delete;

		win32_keyboard& operator=(win32_keyboard const&) = delete;
		win32_keyboard& operator=(win32_keyboard&&) = delete;

		~win32_keyboard() noexcept = default;

	public:
		[[nodiscard]] bool is_key_pressed(key_t key) const noexcept { return m_key_state[static_cast<key_code_t>(key)]; }
		
		win32_keyboard_event read_key() noexcept;
		char read_char() noexcept;

		[[nodiscard]] bool is_key_queue_empty() const noexcept { return m_key_queue.empty(); }
		[[nodiscard]] bool is_char_queue_empty() const noexcept { return m_char_queue.empty(); }

		void clear_key_queue() noexcept { std::queue<win32_keyboard_event>{}.swap(m_key_queue); }
		void clear_char_queue() noexcept { std::queue<char>{}.swap(m_char_queue); }
		void clear() noexcept
		{
			clear_key_queue();
			clear_char_queue();
		}

		[[nodiscard]] bool is_autorepeat_enabled() const noexcept { return m_is_autorepeat_enabled; };
		void enable_autorepeat() noexcept { set_autorepeat(true); }
		void disable_autorepeat() noexcept { set_autorepeat(false); }
		void set_autorepeat(bool is_enabled) noexcept { m_is_autorepeat_enabled = is_enabled; }

	private:
		void on_key_pressed(key_t key) noexcept;
		void on_key_released(key_t key) noexcept;
		void on_char(char c) noexcept;

		void clear_state() noexcept { m_key_state.reset(); }

		template <class T>
		static void trim_queue(std::queue<T> q) noexcept
		{
			while (q.size() > MAX_QUEUE_SIZE)
			{
				q.pop();
			}
		}

	private:
		static constexpr unsigned int KEY_COUNT = 256u;
		static constexpr unsigned int MAX_QUEUE_SIZE = 16u;

	private:
		bool m_is_autorepeat_enabled = false;
		std::bitset<KEY_COUNT> m_key_state;
		std::queue<win32_keyboard_event> m_key_queue;
		std::queue<char> m_char_queue;
	};

}