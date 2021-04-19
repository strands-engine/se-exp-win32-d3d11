#pragma once

#include <string>
#include <Windows.h>

namespace d3dexp
{
	class win32_window
	{
	public:
		win32_window() noexcept = default;

		win32_window(win32_window const&) = delete;
		win32_window(win32_window &&) = delete;
		
		win32_window& operator=(win32_window const&) = delete;
		win32_window& operator=(win32_window &&) = delete;

		~win32_window() noexcept;

	public:
		[[nodiscard]] bool is_initialized() const noexcept { return m_window_h != NULL; }

		bool initialize(HINSTANCE instance_h, std::string title, std::string class_name, int width, int height) noexcept;

		bool process_messages() noexcept;

	private:
		void register_window_class() noexcept;
		void unregister_window_class() noexcept;

	private:
		std::string m_title;
		std::wstring m_title_w;
		std::string m_class_name;
		std::wstring m_class_name_w;

		HINSTANCE m_instance_h = NULL;
		HWND m_window_h = NULL;

		int m_width = 0;
		int m_height = 0;
	};
}