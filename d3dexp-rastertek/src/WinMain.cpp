#include "engine.h"


int APIENTRY wWinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPWSTR argv, _In_ int cmd_show)
{
	auto settings = d3dexp::rastertek::d3d11_graphics_settings_t{};
	//settings.is_fullscreen = true;
	return d3dexp::rastertek::engine{L"Rastertek D3DEXP", settings}.run();
}
