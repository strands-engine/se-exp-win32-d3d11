#pragma once

#include <queue>

#include "win32_mouse_event.h"

namespace d3dexp
{
	class win32_mouse
	{
	public:
		win32_mouse() noexcept = default;

		win32_mouse(win32_mouse const&) = delete;
		win32_mouse(win32_mouse&&) = delete;

		win32_mouse& operator=(win32_mouse const&) = delete;
		win32_mouse& operator=(win32_mouse&&) = delete;

		~win32_mouse() noexcept = default;

	public:
		[[nodiscard]] bool is_lmb_pressed(unsigned char keycode) const noexcept { return m_is_lmb_pressed; }
		[[nodiscard]] bool is_rmb_pressed(unsigned char keycode) const noexcept { return m_is_rmb_pressed; }
		[[nodiscard]] bool is_mmb_pressed(unsigned char keycode) const noexcept { return m_is_mmb_pressed; }

		[[nodiscard]] int x_pos() const noexcept { return m_x_pos; }
		[[nodiscard]] int y_pos() const noexcept { return m_y_pos; }
		[[nodiscard]] mouse_pos_t pos() const noexcept { return mouse_pos_t{ m_x_pos, m_y_pos }; }

		[[nodiscard]] bool is_queue_empty() const noexcept { return m_queue.empty(); }

		win32_mouse_event read() noexcept;

		void on_lmb_pressed(int x, int y) noexcept;
		void on_lmb_released(int x, int y) noexcept;
		void on_rmb_pressed(int x, int y) noexcept;
		void on_rmb_released(int x, int y) noexcept;
		void on_mmb_pressed(int x, int y) noexcept;
		void on_mmb_released(int x, int y) noexcept;
		void on_wheel_up(int x, int y) noexcept;
		void on_wheel_down(int x, int y) noexcept;
		void on_move(int x, int y) noexcept;
		void on_move_raw(int x, int y) noexcept;

	private:
		std::queue<win32_mouse_event> m_queue;

		int m_x_pos = 0;
		int m_y_pos = 0;

		bool m_is_lmb_pressed = false;
		bool m_is_rmb_pressed = false;
		bool m_is_mmb_pressed = false;
	};
}