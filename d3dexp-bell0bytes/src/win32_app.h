#pragma once

#include <memory>
#include <Windows.h>

#include "win32_window.h"
#include "win32_timer.h"
#include "d3d11_graphics.h"
#include "d2d1_graphics.h"

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

	public:
		[[nodiscard]] HWND window_handle() const noexcept { return m_window_p->handle(); }
		[[nodiscard]] com_ptr<IDXGIDevice> device_as_dxgi() const;
		[[nodiscard]] com_ptr<IDXGISurface> back_buffer_as_surface() const;
		[[nodiscard]] bool is_fps_counter_shown() const noexcept { return m_is_fps_counter_shown; }

		[[nodiscard]] int window_width() const noexcept { return m_window_p->m_width; }
		[[nodiscard]] int window_height() const noexcept { return m_window_p->m_height; }

		[[nodiscard]] int display_mode_width() const noexcept { return m_graphics_p->m_display_modes[m_graphics_p->m_chosen_display_mode_ix].Width; }
		[[nodiscard]] int display_mode_height() const noexcept { return m_graphics_p->m_display_modes[m_graphics_p->m_chosen_display_mode_ix].Height; }

		[[nodiscard]] std::wstring const& settings_path() const noexcept { return m_settings_path; }

		void clear_2d_target() { if(m_graphics_2d_p) m_graphics_2d_p->m_context_p->SetTarget(nullptr); }
		expected_t<void> resize_2d_surface();

	protected:
		virtual expected_t<void> initialize();
		virtual void shutdown(expected_t<void>* expected = nullptr);

		virtual void update(double dt);
		virtual expected_t<int> render(double farseer);

		virtual void on_key_down(WPARAM wparam, LPARAM lparam);

		void on_resize();


	private:
		bool setup_mydocs_path();
		bool check_settings_file();

		void calculate_frame_stats();

	protected:
		HINSTANCE m_instance_h = NULL;
		std::unique_ptr<win32_window> m_window_p = nullptr;
		std::unique_ptr<d3d11_graphics> m_graphics_p = nullptr;
		std::unique_ptr<d2d1_graphics> m_graphics_2d_p = nullptr;
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
		bool m_is_fps_counter_shown = false;

	protected:
		bool m_is_paused = false;
		bool m_has_started = false;
	};
}
