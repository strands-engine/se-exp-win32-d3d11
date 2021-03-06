#pragma once

#include "win32_window.h"
#include "win32_keyboard.h"
#include "win32_mouse.h"
#include "d3d11_graphics.h"

namespace d3dexp::jpres
{
	class win32_window_container
	{
	public:
		win32_window_container() noexcept;

		win32_window_container(win32_window_container const&) = delete;
		win32_window_container(win32_window_container &&) = delete;

		win32_window_container& operator=(win32_window_container const&) = delete;
		win32_window_container& operator=(win32_window_container &&) = delete;

		~win32_window_container() noexcept = default;

	public:
		LRESULT window_proc(HWND window_h, UINT msg, WPARAM wparam, LPARAM lparam);

	protected:
		win32_window m_window;
		win32_keyboard m_keyboard;
		win32_mouse m_mouse;
		d3d11_graphics m_graphics;
	};
}