#pragma once

namespace d3dexp::jpres
{
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
		win32_keyboard_event() noexcept = default;
		explicit win32_keyboard_event(action_t action, unsigned char keycode) : m_action(action), m_keycode(keycode) {}

	public:
		[[nodiscard]] bool is_pressed() const noexcept { return m_action == action_t::pressed; }
		[[nodiscard]] bool is_released() const noexcept { return m_action == action_t::released; }
		[[nodiscard]] bool is_valid() const noexcept { return m_action != action_t::invalid; }

		[[nodiscard]] unsigned char keycode() const noexcept { return m_keycode; }

	private:
		action_t m_action = action_t::invalid;
		unsigned char m_keycode = 0u;
	};
}