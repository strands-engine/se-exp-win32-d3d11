#include <stdexcept>

#include <Windows.h>

#include "expected.h"

d3dexp::bell0bytes::expected_t<int> we_hate_four(int x)
{
	if (x == 4)
	{
		return d3dexp::bell0bytes::expected_t<int>::from_exception(std::invalid_argument("We hate 4!"));
	}
	return x;
}



int APIENTRY wWinMain(_In_     HINSTANCE instance_h,			// handle to current instance of an application 
					  _In_opt_ HINSTANCE prev_instance_h,		// handle to previous instance of the same app (relic of old 16-bit Windows 
																//  where instances of the same app had to share the same memory, NULL indicated first copy of app);
																//  in Win32 - always NULL for backwards compatibility with old apps that had to allocate 
																//  resources/register classes only in first instance [cf. https://bell0bytes.eu/hello-world/]
	                  _In_     LPWSTR    argv,					// command line arguments (not separated as in standard main argc-argv)
	                  _In_     int       cmd_show)				// indicates how app window is to be shown/opened (e.g. minimized or maximized)
{
	const auto res = we_hate_four(4);
	return !res ? static_cast<int>(res.has_exception<std::invalid_argument>()) : *res;
	
	
	//return 0; // should be wparam of WM_QUIT message ( or 0, if not entering message loop)
}
