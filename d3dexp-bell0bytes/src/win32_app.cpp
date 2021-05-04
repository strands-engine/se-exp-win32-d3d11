#include "win32_app.h"

// windows
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Pathcch.h>

#include <sstream>
#include <iomanip>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "string_converter.h"

namespace d3dexp::bell0bytes
{
	win32_app::~win32_app()
	{
		shutdown();
	}

	// NOTE 1: consider separate update functions for per-frame and fixed time steps
	// NOTE 2: farseer loop - do as many fixed time updates as they fit into single frame:
	//                        -> accumulating frame time (t_a) - how far ahead the next rendered frame is - it is passed to render normalized by fixed_dt to be in [0, 1) range (rendering function needs to look a bit into future ???) - interpolation of positions/velocities
	// NOTE 3: possible issues - failing vsync / multithreading
	// NOTE 4: ppossible further sources-
	//           ->	Game Programming Algorithms and Techniques, by Sanjay Madhav
	//           ->	Game Programming Patterns, by Robert Nystrom
	//           ->	Tricks of the Windows Game Programming Gurus, by André LaMothe
	expected_t<int> win32_app::run()
	{
		// setup main app timer
		m_timer.reset();

		auto accumulated_frame_time = double{}; // time spent on updates before going to rendering
		auto update_loop_count = int{};        // number of updates performed before render
		auto is_running = true;

		auto msg = MSG{};

		// main loop
		while (is_running)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
				{
					is_running = false;
				}
			}

			// tick tock
			m_timer.update();

			if (!m_is_paused)
			{
#ifdef _DEBUG
				calculate_frame_stats();
#endif
				// acquire input

				// accumulate time elapsed since last frame
				accumulated_frame_time += m_timer.delta_time();

				// update objects with fixed delta time as often as possible
				update_loop_count = 0;
				while (accumulated_frame_time >= m_fixed_delta_time && update_loop_count < m_max_skipped_frames)
				{
					update(m_fixed_delta_time);
					accumulated_frame_time -= m_fixed_delta_time;
					update_loop_count++;
				}

				// generate output (peeking into future?)
				render(accumulated_frame_time / m_fixed_delta_time);

			}
		}
		return static_cast<int>(msg.wParam);
	}

	com_ptr<IDXGIDevice> win32_app::device_as_dxgi() const
	{
		auto dev_p = com_ptr<IDXGIDevice>{};
		auto hr = m_graphics_p->m_device_p->QueryInterface(__uuidof(IDXGIDevice), &dev_p);
		if (FAILED(hr)) throw std::runtime_error("Cannot qurery DXGI device.");
		return dev_p;
	}

	com_ptr<IDXGISurface> win32_app::back_buffer_as_surface() const
	{
		auto surface_p = com_ptr<IDXGISurface>{};
		auto hr = m_graphics_p->m_swap_chain_p->GetBuffer(0u, __uuidof(IDXGISurface), to_pp(surface_p));
		if (FAILED(hr)) throw std::runtime_error("Cannot qurery DXGI surface from back buffer.");
		return surface_p;
	}

	expected_t<void> win32_app::resize_2d_surface()
	{
		if (m_graphics_2d_p)
		{
			auto result = m_graphics_2d_p->create_bitmap_renderer_target();
			if (!result) return std::runtime_error{ "Failed to adjust window size for D2D." };
		}
		return {};
	}


	expected_t<void> win32_app::initialize()
	{
		if (!setup_mydocs_path())
		{
			OutputDebugStringA("Failed to establish path to 'My Documents' folder.");
			return std::runtime_error("Failed to establish path to 'My Documents' folder.");
		}

		if (!check_settings_file())
		{
			OutputDebugStringA("Missing or invalid settings file. Using defaults.");
		}

		try
		{
			m_window_p = std::make_unique<win32_window>(this);
		}
		catch (std::runtime_error&)
		{
			return std::runtime_error("Failed to create main window.");
		}

		try
		{
			m_graphics_p = std::make_unique<d3d11_graphics>(this);
		}
		catch (std::runtime_error&)
		{
			return std::runtime_error("Failed to initialize D3D11 graphics.");
		}

		try
		{
			m_graphics_2d_p = std::make_unique<d2d1_graphics>(this);
		}
		catch (std::runtime_error&)
		{
			return std::runtime_error("Failed to initialize D2D1 graphics.");
		}

		m_has_started = true;
		OutputDebugStringA("Win32 app successfully initialized.\n");
		return {};
	}

	void win32_app::shutdown(expected_t<void>* expected)
	{
		OutputDebugStringA("Win32 app successfully shutdown.\n");
	}

	bool win32_app::setup_mydocs_path()
	{
		PWSTR path_p = nullptr;
		auto hr = SHGetKnownFolderPath(FOLDERID_Documents, NULL, NULL, &path_p);

		// debug mode only: make sure the operation succeeded
#ifdef _DEBUG
		if (FAILED(hr))
		{
			return false;
		}
#endif
		// store the path
		m_mydocs_path = path_p;

		// delete the wstring pointer to avoid memory leak
		::CoTaskMemFree(static_cast<void*>(path_p));

		m_settings_path = L"D:\\repos\\se-exp-win32-d3d11\\d3dexp-bell0bytes\\config";

		return true;
	}

	void win32_app::update(double dt)
	{

	}

	expected_t<int> win32_app::render(double farseer)
	{
		// after drawing present the scene to front buffer
		auto result = m_graphics_p->present();
		if (!result)
		{
			return std::runtime_error{ "Failed to present the scene!" };
		}
		return 0;
	}

	void win32_app::on_key_down(WPARAM wparam, LPARAM lparam)
	{
		switch (wparam)
		{

		case VK_ESCAPE:
		{
			PostMessage(m_window_p->handle(), WM_CLOSE, 0, 0);
			break;
		}
		case VK_F1:
		{
			m_is_fps_counter_shown = !m_is_fps_counter_shown;
			break;
		}
		case VK_PRIOR:
		{
			// page up -> chose higher resolution
			m_graphics_p->increase_resolution();
			break;
		}
		case VK_NEXT:
		{
			// page down -> chose lower resolution
			m_graphics_p->decrease_resolution();
			break;
		}
		default:
			break;
		}
	}

	void win32_app::on_resize()
	{
		OutputDebugStringA("Window being resized!\n");
		if (m_has_started)
		{
			m_graphics_p->on_resize();
		}
	}

	bool win32_app::check_settings_file()
	{
		auto path = m_settings_path + L"\\settings.lua";

		// check if the log file is accessible
		auto fin = std::ifstream{ path.c_str() };

		if (fin.good())
		{
			// the file exists and can be read
			if (fin.peek() == std::ifstream::traits_type::eof())
			{
				// the file is empty, create it
				try
				{
					fin.close();

					auto fout = std::ofstream{ path.c_str() };
					fout << "config =\n{ \n\twindow = { width = 800, height = 600 }\n}";
				}
				catch (std::runtime_error)
				{
					return false;
				}
			}
		}
		else
		{
			// the file does not exist --> create it
			try
			{
				fin.close();

				auto fout = std::ofstream{ path.c_str() };
				fout << "config =\n{ \n\twindow = { width = 800, height = 600 }\n}";
			}
			catch (std::runtime_error)
			{
				return false;
			}
		}

		m_is_settings_file_valid = true;
		return true;
	}

	void win32_app::calculate_frame_stats()
	{
		static auto frame_count = int{};
		static auto elapsed_time = double{};

		frame_count++;

		if ((m_timer.total_time() - elapsed_time) >= 1.0)
		{
			// update statistics
			m_frames_per_second = frame_count;
			m_milliseconds_per_frame = 1000.0 / static_cast<double>(m_frames_per_second);

			if (m_is_fps_counter_shown)
			{
				// prepare fps information message
				auto ss = std::wostringstream{};
				ss.precision(6);
				ss << L"FPS: " << m_frames_per_second << std::endl;
				ss << L"mSPF: " << m_milliseconds_per_frame << std::endl;

				// create fps information text layout
				m_graphics_2d_p->create_text_layout(
					d2d1_graphics::context_t::fps, 
					ss.str(), 
					static_cast<float>(m_window_p->m_width), 
					static_cast<float>(m_window_p->m_height))
				.throw_if_failed();
			}

			// reset static counters
			frame_count = 0;
			elapsed_time += 1.0;

		}
	}

}

