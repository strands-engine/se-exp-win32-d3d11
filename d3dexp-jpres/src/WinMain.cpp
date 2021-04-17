#include <Windows.h>

#include "error_logger.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPWSTR argv, _In_ int cmd_show)
{
	d3dexp::error_logger::log(S_OK, "Test message");

	return 0;
}