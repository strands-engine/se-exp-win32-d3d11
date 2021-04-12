#pragma once

#include <queue>

#include "input_types.h"

namespace d3dexp {

	class win32_mouse
	{
		friend class win32_window;

	public:
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
				xmb_pressed,
				xmb_released,
				wheel_up,
				wheel_down,
				move,
				invalid
			};

		public:
			win32_mouse_event() noexcept = default;
			win32_mouse_event(action_t action, win32_mouse const& parent) noexcept : 
				m_action(action), 
				m_x(parent.m_x),
				m_y(parent.m_y),
				m_is_lmb_pressed(parent.m_is_lmb_pressed),
				m_is_rmb_pressed(parent.m_is_rmb_pressed),
				m_is_mmb_pressed(parent.m_is_mmb_pressed),
				m_is_xmb_pressed(parent.m_is_xmb_pressed)
			{}

		public:
			[[nodiscard]] action_t action() const noexcept { return m_action; }
			[[nodiscard]] std::pair<int, int> pos() { return std::make_pair(m_x, m_y); }
			[[nodiscard]] int x_pos() const noexcept { return m_x; }
			[[nodiscard]] int y_pos() const noexcept { return m_y; }
			[[nodiscard]] bool is_lmb_pressed() const noexcept { return m_is_lmb_pressed; }
			[[nodiscard]] bool is_rmb_pressed() const noexcept { return m_is_rmb_pressed; }
			[[nodiscard]] bool is_mmb_pressed() const noexcept { return m_is_mmb_pressed; }
			[[nodiscard]] bool is_xmb_pressed() const noexcept { return m_is_xmb_pressed; }
			[[nodiscard]] bool is_valid() const noexcept { return m_action != action_t::invalid; }

		private:
			action_t m_action = action_t::invalid;
			int m_x = 0;
			int m_y = 0;
			bool m_is_lmb_pressed = false;
			bool m_is_rmb_pressed = false;
			bool m_is_mmb_pressed = false;
			bool m_is_xmb_pressed = false;
		};

	public:
		win32_mouse() noexcept = default;

		win32_mouse(win32_mouse const&) = delete;
		win32_mouse(win32_mouse&&) = delete;

		win32_mouse& operator=(win32_mouse const&) = delete;
		win32_mouse& operator=(win32_mouse&&) = delete;

		~win32_mouse() noexcept = default;

	public:
		[[nodiscard]] bool is_lmb_pressed() const noexcept { return m_is_lmb_pressed; }
		[[nodiscard]] bool is_rmb_pressed() const noexcept { return m_is_rmb_pressed; }
		[[nodiscard]] bool is_mmb_pressed() const noexcept { return m_is_mmb_pressed; }
		[[nodiscard]] bool is_xmb_pressed() const noexcept { return m_is_xmb_pressed; }

		[[nodiscard]] std::pair<int, int> pos() { return std::make_pair(m_x, m_y); }
		[[nodiscard]] int x_pos() { return m_x; }
		[[nodiscard]] int y_pos() { return m_y; }

		win32_mouse_event read() noexcept;
		
		[[nodiscard]] bool is_queue_empty() const noexcept { return m_queue.empty(); }

		void clear_queue() noexcept { std::queue<win32_mouse_event>{}.swap(m_queue); }
		void clear() noexcept { clear_queue(); }

	private:
		void on_lmb_pressed(int x, int y) noexcept;
		void on_rmb_pressed(int x, int y) noexcept;
		void on_mmb_pressed(int x, int y) noexcept;
		void on_xmb_pressed(int x, int y) noexcept;
		
		void on_lmb_released(int x, int y) noexcept;
		void on_rmb_released(int x, int y) noexcept;
		void on_mmb_released(int x, int y) noexcept;
		void on_xmb_released(int x, int y) noexcept;
		
		void on_wheel_up(int x, int y) noexcept;
		void on_wheel_down(int x, int y) noexcept;

		void on_move(int x, int y) noexcept;

		void trim_queue() noexcept { while (m_queue.size() > MAX_QUEUE_SIZE) { m_queue.pop(); } }

	private:
		static constexpr unsigned int MAX_QUEUE_SIZE = 16u;

	private:
		std::queue<win32_mouse_event> m_queue;
		int m_x = 0;
		int m_y = 0;
		bool m_is_lmb_pressed = false;
		bool m_is_rmb_pressed = false;
		bool m_is_mmb_pressed = false;
		bool m_is_xmb_pressed = false;
	};

}