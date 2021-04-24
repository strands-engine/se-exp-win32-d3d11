#pragma once

#include <memory>
#include <string>

#include "win32_input.h"
#include "win32_graphics.h"

namespace d3dexp
{
	class win32_window
	{
	public:
		win32_window(std::wstring const& title, d3d11_graphics_settings_t& settings) noexcept;

		win32_window(win32_window const&) = delete;
		win32_window(win32_window &&) = delete;

		win32_window& operator=(win32_window const&) = delete;
		win32_window& operator=(win32_window &&) = delete;

		~win32_window() noexcept;

	public:
		[[nodiscard]] win32_input const& input() const noexcept { return m_input; }
		[[nodiscard]] win32_input& input() noexcept { return m_input; }

		[[nodiscard]] win32_graphics const& graphics() const noexcept { return *m_graphics_p; }
		[[nodiscard]] win32_graphics& graphics() noexcept { return *m_graphics_p; }

		LRESULT CALLBACK message_handler(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam);

	private:
		win32_input m_input;

		std::unique_ptr<win32_graphics> m_graphics_p = nullptr;

		std::wstring m_title;

		HWND m_window_h = nullptr;
		HINSTANCE m_instance_h = nullptr;

		int m_width = 0;        // do not update on window resizing
		int m_height = 0;
		int m_pos_x = 0;        // do not update on window moving
		int m_pos_y = 0;

		bool m_is_fullscreen = false;
	};

	static LRESULT CALLBACK wnd_proc(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam);

	static win32_window* g_window_p = nullptr;

}