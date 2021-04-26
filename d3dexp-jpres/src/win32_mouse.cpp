#include "win32_mouse.h"

namespace d3dexp::jpres
{
	win32_mouse_event win32_mouse::read() noexcept
	{
		if (is_queue_empty())
		{
			return win32_mouse_event{};
		}

		const auto evt = win32_mouse_event{ m_queue.front() };
		m_queue.pop();
		return evt;
	}

	void win32_mouse::on_lmb_pressed(int x, int y) noexcept
	{
		m_is_lmb_pressed = true;
		m_queue.emplace(win32_mouse_event::action_t::lmb_pressed, x, y);
	}

	void win32_mouse::on_lmb_released(int x, int y) noexcept
	{
		m_is_lmb_pressed = false;
		m_queue.emplace(win32_mouse_event::action_t::lmb_released, x, y);
	}

	void win32_mouse::on_rmb_pressed(int x, int y) noexcept
	{
		m_is_rmb_pressed = true;
		m_queue.emplace(win32_mouse_event::action_t::rmb_pressed, x, y);
	}

	void win32_mouse::on_rmb_released(int x, int y) noexcept
	{
		m_is_rmb_pressed = false;
		m_queue.emplace(win32_mouse_event::action_t::rmb_released, x, y);
	}

	void win32_mouse::on_mmb_pressed(int x, int y) noexcept
	{
		m_is_mmb_pressed = true;
		m_queue.emplace(win32_mouse_event::action_t::mmb_pressed, x, y);
	}

	void win32_mouse::on_mmb_released(int x, int y) noexcept
	{
		m_is_mmb_pressed = false;
		m_queue.emplace(win32_mouse_event::action_t::mmb_released, x, y);
	}

	void win32_mouse::on_wheel_up(int x, int y) noexcept
	{
		m_queue.emplace(win32_mouse_event::action_t::wheel_up, x, y);
	}

	void win32_mouse::on_wheel_down(int x, int y) noexcept
	{
		m_queue.emplace(win32_mouse_event::action_t::wheel_down, x, y);
	}

	void win32_mouse::on_move(int x, int y) noexcept
	{
		m_queue.emplace(win32_mouse_event::action_t::move, x, y);
	}

	void win32_mouse::on_move_raw(int x, int y) noexcept
	{
		m_queue.emplace(win32_mouse_event::action_t::move_raw, x, y);
	}
}