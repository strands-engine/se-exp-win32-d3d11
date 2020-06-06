#include "win32_keyboard.h"

namespace d3dexp {

	win32_keyboard::win32_keyboard_event win32_keyboard::read_key() noexcept
	{
		if (!is_key_queue_empty())
		{
			auto result = m_key_queue.front();
			m_key_queue.pop();
			return result;
		}
		return win32_keyboard::win32_keyboard_event{};
	}

	char win32_keyboard::read_char() noexcept
	{
		if (!is_char_queue_empty())
		{
			auto result = m_char_queue.front();
			m_char_queue.pop();
			return result;
		}
		return '\0';
	}

	void win32_keyboard::on_key_pressed(key_t key) noexcept
	{
		m_key_state[static_cast<key_code_t>(key)] = true;
		m_key_queue.emplace(win32_keyboard_event::action_t::pressed, key);
		trim_queue(m_key_queue);
	}

	void win32_keyboard::on_key_released(key_t key) noexcept
	{
		m_key_state[static_cast<key_code_t>(key)] = false;
		m_key_queue.emplace(win32_keyboard_event::action_t::released, key);
		trim_queue(m_key_queue);
	}

	void win32_keyboard::on_char(char c) noexcept
	{
		m_char_queue.emplace(c);
		trim_queue(m_char_queue);
	}

}