#include <stdexcept>

#include <Windows.h>

#include "expected.h"
#include "app.h"

#include <lua.hpp>
#pragma comment(lib, "liblua54.a") 

int APIENTRY wWinMain(_In_     HINSTANCE instance_h,			// handle to current instance of an application 
					  _In_opt_ HINSTANCE prev_instance_h,		// handle to previous instance of the same app (relic of old 16-bit Windows 
																//  where instances of the same app had to share the same memory, NULL indicated first copy of app);
																//  in Win32 - always NULL for backwards compatibility with old apps that had to allocate 
																//  resources/register classes only in first instance [cf. https://bell0bytes.eu/hello-world/]
	                  _In_     LPWSTR    argv,					// command line arguments (not separated as in standard main argc-argv)
	                  _In_     int       cmd_show)				// indicates how app window is to be shown/opened (e.g. minimized or maximized)
{
	auto my_app = d3dexp::bell0bytes::app{ instance_h };
	auto init_result = my_app.initialize();

	lua_State* state = luaL_newstate();
	lua_close(state);

	if (init_result)
	{
		auto run_result = my_app.run();

		// cleanup
		// horrifying cast - even reinterpret_cast won't allow it
		// my_app.shutdown(&((d3dexp::bell0bytes::expected_t<void>)run_result));
		my_app.shutdown(); // no error handling
		return *run_result;
	}
	else
	{
		my_app.shutdown(&init_result);
		return -1;
	}
	
	return 0; // should be wparam of WM_QUIT message ( or 0, if not entering message loop)
}