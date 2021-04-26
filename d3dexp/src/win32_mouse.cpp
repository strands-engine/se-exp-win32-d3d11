#include "win32_mouse.h"

#include <Windows.h>

namespace d3dexp::chili
{

	win32_mouse::win32_mouse_event win32_mouse::read() noexcept
	{
		if (!is_queue_empty())
		{
			auto result = m_queue.front();
			m_queue.pop();
			return result;
		}
		return win32_mouse::win32_mouse_event{};
	}

	void win32_mouse::on_lmb_pressed(int x, int y) noexcept
	{
		m_is_lmb_pressed = true;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::lmb_pressed, *this });
		trim_queue();
	}

	void win32_mouse::on_rmb_pressed(int x, int y) noexcept
	{
		m_is_rmb_pressed = true;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::rmb_pressed, *this });
		trim_queue();
	}

	void win32_mouse::on_mmb_pressed(int x, int y) noexcept
	{
		m_is_mmb_pressed = true;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::mmb_pressed, *this });
		trim_queue();
	}

	void win32_mouse::on_xmb_pressed(int x, int y) noexcept
	{
		m_is_xmb_pressed = true;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::xmb_pressed, *this });
		trim_queue();
	}

	void win32_mouse::on_lmb_released(int x, int y) noexcept
	{
		m_is_lmb_pressed = false;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::lmb_released, *this });
		trim_queue();
	}

	void win32_mouse::on_rmb_released(int x, int y) noexcept
	{
		m_is_rmb_pressed = false;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::rmb_released, *this });
		trim_queue();
	}

	void win32_mouse::on_mmb_released(int x, int y) noexcept
	{
		m_is_mmb_pressed = false;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::mmb_released, *this });
		trim_queue();
	}

	void win32_mouse::on_xmb_released(int x, int y) noexcept
	{
		m_is_xmb_pressed = false;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::xmb_released, *this });
		trim_queue();
	}

	void win32_mouse::on_wheel_up(int x, int y) noexcept
	{
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::wheel_up, *this });
		trim_queue();
	}

	void win32_mouse::on_wheel_down(int x, int y) noexcept
	{
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::wheel_down, *this });
		trim_queue();
	}

	void win32_mouse::on_wheel_delta(int x, int y, int delta) noexcept
	{
#ifdef D3DEXP_SIMPLIFIED_MOUSE_WHEEL
		if (delta > 0)
		{
			on_wheel_up(x, y);
		}
		else if (delta < 0)
		{
			on_wheel_down(x, y);
		}
#endif
		m_wheel_delta_residue += delta;

		while (m_wheel_delta_residue >= WHEEL_DELTA)
		{
			m_wheel_delta_residue -= WHEEL_DELTA;
			on_wheel_up(x, y);
		}
		while (m_wheel_delta_residue <= -WHEEL_DELTA)
		{
			m_wheel_delta_residue += WHEEL_DELTA;
			on_wheel_down(x, y);
		}
	}

	void win32_mouse::on_leave() noexcept
	{
		m_is_in_window = false;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::leave, *this });
		trim_queue();
	}

	void win32_mouse::on_enter() noexcept
	{
		m_is_in_window = true;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::enter, *this });
		trim_queue();
	}

	void win32_mouse::on_move(int x, int y) noexcept
	{
		m_x = x;
		m_y = y;
		m_queue.emplace(win32_mouse::win32_mouse_event{ win32_mouse::win32_mouse_event::action_t::move, *this });
		trim_queue();
	}

}