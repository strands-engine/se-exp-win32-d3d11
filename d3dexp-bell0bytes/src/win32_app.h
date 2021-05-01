#pragma once

#include <memory>
#include <Windows.h>

#include "win32_window.h"
#include "win32_timer.h"

namespace d3dexp::bell0bytes
{
	class win32_app
	{
	public:
		friend class win32_window;

	protected:
		win32_app(HINSTANCE instance_h) : m_instance_h(instance_h) {}

		win32_app(win32_app const&) = delete;
		win32_app(win32_app &&) = delete;

		win32_app& operator=(win32_app const&) = delete;
		win32_app& operator=(win32_app &&) = delete;

		~win32_app();

	protected:
		expected_t<int> run();

	protected:
		virtual expected_t<void> initialize();
		virtual void shutdown(expected_t<void>* expected = nullptr);

		virtual void update(double dt);
		virtual void render(double farseer);

		virtual void on_key_down(WPARAM wparam, LPARAM lparam);

		void on_resize();


	private:
		bool setup_mydocs_path();
		bool check_settings_file();

		void calculate_frame_stats();
		void debug_show_frame_stats(HWND window_h);

	protected:
		HINSTANCE m_instance_h = NULL;
		std::unique_ptr<win32_window> m_window_p = nullptr;
		win32_timer m_timer;

	private:
		std::wstring m_mydocs_path = std::wstring{};
		std::wstring m_settings_path = std::wstring{};

		double m_fixed_delta_time = 1.0 / 240.0;      // fixed time frame for 240 fps
		double m_max_skipped_frames = 10.0;

		double m_milliseconds_per_frame = 0.0;
		int m_frames_per_second = 0;

		bool m_is_frame_stats_updated = true;
		bool m_is_settings_file_valid = false;

	protected:
		bool m_is_paused = false;
		bool m_has_started = false;
	};
}
