#include "win32_keyboard.h"

namespace d3dexp 
{
	win32_keyboard_event win32_keyboard::read_key() noexcept
	{
		if (is_key_queue_empty())
		{
			return win32_keyboard_event{};
		}

		auto evt = m_key_queue.front();
		m_key_queue.pop();
		return evt;
	}

	unsigned char win32_keyboard::read_char() noexcept
	{
		if (is_char_queue_empty())
		{
			return 0u;
		}

		auto c = m_char_queue.front();
		m_char_queue.pop();
		return c;
	}

	void win32_keyboard::on_key_pressed(unsigned char keycode) noexcept
	{
		m_key_states[keycode] = true;
		m_key_queue.emplace(win32_keyboard_event::action_t::pressed, keycode);
	}

	void win32_keyboard::on_key_released(unsigned char keycode) noexcept
	{
		m_key_states[keycode] = false;
		m_key_queue.emplace(win32_keyboard_event::action_t::released, keycode);
	}

	void win32_keyboard::on_char(unsigned char character) noexcept
	{
		m_char_queue.emplace(character);
	}
}