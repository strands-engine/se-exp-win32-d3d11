#include "engine.h"


int APIENTRY wWinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPWSTR argv, _In_ int cmd_show)
{
	auto settings = d3dexp::d3d11_graphics_settings_t{};
	settings.is_fullscreen = false;
	return d3dexp::engine{L"Rastertek D3DEXP", settings}.run();
}
