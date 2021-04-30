#include "win32_app.h"

// windows
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Pathcch.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

#include <fstream>
#include <stdexcept>

namespace d3dexp::bell0bytes
{
	win32_app::~win32_app()
	{
		shutdown();
	}

	expected_t<int> win32_app::run()
	{
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

			if (!m_is_paused)
			{
				// insert app logic here
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
		auto fin = std::ifstream{path.c_str()};

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

	void win32_app::on_resize()
	{
		OutputDebugStringA("Window being resized!\n");
	}

}
