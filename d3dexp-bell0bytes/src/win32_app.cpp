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
#include <stdexcept>

#include "string_converter.h"

namespace d3dexp::bell0bytes
{
	win32_app::~win32_app()
	{
		shutdown();
	}

	expected_t<int> win32_app::run()
	{
		m_timer.reset();

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

			m_timer.update();

			if (!m_is_paused)
			{
#ifdef _DEBUG
				calculate_frame_stats();
				debug_show_frame_stats(m_window_p->handle());
#endif
				// process input

				// update game logic
				update(m_timer.delta_time());

				// generate output
			}
		}
		return static_cast<int>(msg.wParam);
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

			// reset static counters
			frame_count = 0;
			elapsed_time += 1.0;

			m_is_frame_stats_updated = true;
		}
	}

	void win32_app::debug_show_frame_stats(HWND window_h)
	{
		if (m_is_frame_stats_updated)
		{
			auto ss = std::stringstream{};
			ss << m_frames_per_second;
			ss << " FPS (frame time: ";
			ss << std::fixed << std::setprecision(4) << m_milliseconds_per_frame;
			ss << " ms)";

			SetWindowText(window_h, string_converter::to_wstring(ss.str()).c_str());

			m_is_frame_stats_updated = false;
		}
	}

	void win32_app::update(double dt)
	{

	}

	void win32_app::on_resize()
	{
		OutputDebugStringA("Window being resized!\n");
	}

}
