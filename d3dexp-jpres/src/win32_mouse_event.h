#pragma once

namespace d3dexp
{
	struct mouse_pos_t
	{
		int x = 0;
		int y = 0;
	};


	class win32_mouse_event
	{
	public:
		enum class action_t
		{
			lmb_pressed,
			lmb_released,
			rmb_pressed,
			rmb_released,
			mmb_pressed,
			mmb_released,
			wheel_up,
			wheel_down,
			move,
			move_raw,
			invalid
		};

	public:
		win32_mouse_event() noexcept = default;
		explicit win32_mouse_event(action_t action, int x, int y) : m_action(action), m_x_pos(x), m_y_pos(y) {}

	public:
		[[nodiscard]] bool is_valid() const noexcept { return m_action != action_t::invalid; }

		[[nodiscard]] action_t action() const noexcept { return m_action; }

		[[nodiscard]] int x_pos() const noexcept { return m_x_pos; }
		[[nodiscard]] int y_pos() const noexcept { return m_y_pos; }
		[[nodiscard]] mouse_pos_t pos() const noexcept { return mouse_pos_t{m_x_pos, m_y_pos}; }

	private:
		action_t m_action = action_t::invalid;
		int m_x_pos = 0;
		int m_y_pos = 0;
	};
}