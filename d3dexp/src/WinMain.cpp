#include <sstream>

#include "win32_header_wrapper.h"
#include "win32_app.h"
#include "d3d_exception.h"


int CALLBACK WinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPSTR argv, _In_ int cmd_show)
{
	try
	{
		return d3dexp::chili::win32_app{}.run();
	}
	catch (d3dexp::chili::d3d_exception const& e)
	{
		MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONERROR);
	}
	catch (std::exception const& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, "No error details available.", "Unknown Exception", MB_OK | MB_ICONERROR);
	}

	return -1;
}