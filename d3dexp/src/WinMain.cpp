#include <Windows.h>

#include "win32_debug_message_map.h"

LRESULT CALLBACK WindowProc(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam)
{
#ifdef _DEBUG
	static d3dexp::win32_debug_message_map s_msg_map;
	OutputDebugString(s_msg_map(msg_id, wparam, lparam).c_str());
#endif // DEBUG

	switch (msg_id)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wparam == 'F')
		{
			SetWindowText(wnd_h, "Respect!");
		}
		break;

	case WM_KEYUP:
		if (wparam == 'F')
		{
			SetWindowText(wnd_h, "No Respect.");
		}
		break;
	}
	return DefWindowProc(wnd_h, msg_id, wparam, lparam);
}

int CALLBACK WinMain(_In_ HINSTANCE instance_h, _In_opt_ HINSTANCE prev_instance_h, _In_ LPSTR argv, _In_ int cmd_show)
{
	// new win32 window class registration 
	const auto win32_window_class_name = "D3D EXP Main Window";
	WNDCLASSEX wndcl = {};

	wndcl.cbSize = sizeof(wndcl);
	wndcl.style = CS_OWNDC;
	wndcl.lpfnWndProc = WindowProc;
	
	wndcl.cbClsExtra = 0;
	wndcl.cbWndExtra = 0;

	wndcl.hInstance = instance_h;

	wndcl.hIcon = nullptr;
	wndcl.hCursor = nullptr;
	wndcl.hbrBackground = nullptr;
	wndcl.lpszMenuName = nullptr;

	wndcl.lpszClassName = win32_window_class_name;

	wndcl.hIconSm = nullptr;

	RegisterClassEx(&wndcl);

	// create win32 window instance
	const auto main_window_title = "D3DEXP";

	HWND wnd_h = CreateWindowEx(
		0, win32_window_class_name, main_window_title,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, 640, 480,
		nullptr, nullptr, instance_h, nullptr);

	// show created window instance
	ShowWindow(wnd_h, SW_SHOW);

	// basic message pump
	MSG msg;
	BOOL result;
	while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
	return result < 0 ? result : static_cast<int>(msg.wParam);
}