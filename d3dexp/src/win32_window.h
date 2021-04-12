#pragma once

#include "win32_header_wrapper.h"

#include <optional>

#include "win32_keyboard.h"
#include "win32_mouse.h"

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
			[[nodiscard]] static HICON load_icon_resource(int icon_id, int size) noexcept;

		private:
			static constexpr char const* s_window_class_name = "oglexp_window_class";
			static win32_window_class s_instance;
			HINSTANCE m_app_instance_h;
		};

	public:
		explicit win32_window(int width, int height, char const* name);

		win32_window(win32_window const&) = delete;
		win32_window(win32_window&&) = delete;

		win32_window& operator=(win32_window const&) = delete;
		win32_window& operator=(win32_window&&) = delete;

		~win32_window() noexcept;

	public:
		[[nodiscard]] std::string const& title() const noexcept { return m_title; }
		[[nodiscard]] int width() const noexcept { return m_cli_width; }
		[[nodiscard]] int height() const noexcept { return m_cli_height; }

		[[nodiscard]] win32_keyboard& keyboard() noexcept { return m_keyboard; }
		[[nodiscard]] win32_keyboard const& keyboard() const noexcept { return m_keyboard; }

		[[nodiscard]] win32_mouse& mouse() noexcept { return m_mouse; }
		[[nodiscard]] win32_mouse const& mouse() const noexcept { return m_mouse; }

		void set_title(std::string const& title);

		static std::optional<int> process_messages();

	private:
		static LRESULT CALLBACK handle_message_setup(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept;
		static LRESULT CALLBACK handle_message_adapter(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept;
		LRESULT handle_message(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam) noexcept;

	private:
		std::string m_title;
		int m_cli_width;
		int m_cli_height;
		int m_wnd_width;
		int m_wnd_height;
		HWND m_window_h;

		win32_keyboard m_keyboard;
		win32_mouse m_mouse;
	};

}