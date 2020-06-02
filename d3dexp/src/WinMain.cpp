#include "win32_header_wrapper.h"
#include "win32_window.h"


int CALLBACK WinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPSTR argv, _In_ int cmd_show)
{
	// new window creation
	auto window = d3dexp::win32_window(800, 600, "D3DEXP Main Window");

	// main loop
	auto msg = MSG{};
	auto result = BOOL{};
	while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return result < 0 ? result : static_cast<int>(msg.wParam);
}