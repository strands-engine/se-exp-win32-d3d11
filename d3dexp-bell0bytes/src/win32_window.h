#pragma once

#include <Windows.h>
#include "expected.h"

namespace d3dexp::bell0bytes
{
	class win32_app;

	// horrible coupling with win32_app :(

	class win32_window
	{
	public:
		friend class win32_app;

	public:
		win32_window(win32_app* app_p);

		win32_window(win32_window const&) = delete;
		win32_window(win32_window &&) = delete;

		win32_window& operator=(win32_window const&) = delete;
		win32_window& operator=(win32_window &&) = delete;

		~win32_window() = default;

	public:
		[[nodiscard]] HWND handle() const { return m_window_h; }

		virtual LRESULT CALLBACK wnd_proc(HWND window_h, unsigned int msg, WPARAM wparam, LPARAM lparam);

	private:
		expected_t<void> initialize();

		void load_settings();

	private:
		HWND m_window_h = NULL;
		win32_app* m_app_p = nullptr;

		int m_width = 800;
		int m_height = 600;

		bool m_is_minimized = false;
		bool m_is_maximized = false;
		bool m_is_resizing = false;
	};
}