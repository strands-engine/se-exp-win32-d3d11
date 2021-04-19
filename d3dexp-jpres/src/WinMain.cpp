#include <Windows.h>

#include "win32_window.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPWSTR argv, _In_ int cmd_show)
{
	auto window = d3dexp::win32_window{};
	window.initialize(instance_h, "Jpres D3DEXP Main Window", "JpresD3DEXP_wndclass", 800, 600);

	while (window.process_messages()) {}

	return 0;
}