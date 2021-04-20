#include <Windows.h>

#include "engine.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPWSTR argv, _In_ int cmd_show)
{
	auto engine = d3dexp::engine{};
	engine.initialize(instance_h, "Jpres D3DEXP Main Window", "JpresD3DEXP_wndclass", 800, 600);

	while (engine.process_messages()) {}

	return 0;
}