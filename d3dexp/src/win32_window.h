#pragma once

#include "win32_header_wrapper.h"

namespace d3dexp {

	class win32_window
	{
	private:
		// singleton that manages registration and unregistration of common win32 windows class for d3dexp
		class win32_window_class
		{
		private:
			win32_window_class() noexcept;

		public:
			win32_window_class(win32_window_class const&) = delete;
			win32_window_class(win32_window_class&&) = delete;

			win32_window_class& operator=(win32_window_class const&) = delete;
			win32_window_class& operator=(win32_window_class&&) = delete;

			~win32_window_class() noexcept;

		public:
			[[nodiscard]] static constexpr char const* name() noexcept { return s_window_class_name; }
			[[nodiscard]] static HINSTANCE instance() noexcept { return s_instance.m_app_instance_h; }

		private:
			static constexpr char const* s_window_class_name = "oglexp_window_class";
			static win32_window_class s_instance;
			HINSTANCE m_app_instance_h;
		};

	public:
		explicit win32_window(int width, int height, char const* name) noexcept;

		win32_window(win32_window const&) = delete;
		win32_window(win32_window&&) = delete;

		win32_window& operator=(win32_window const&) = delete;
		win32_window& operator=(win32_window&&) = delete;

		~win32_window() noexcept;

	private:
		static LRESULT CALLBACK handle_message_setup(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept;
		static LRESULT CALLBACK handle_message_adapter(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept;
		LRESULT handle_message(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept;

	private:
		int m_cli_width;
		int m_cli_height;
		int m_wnd_width;
		int m_wnd_height;
		HWND m_window_h;
	};

}