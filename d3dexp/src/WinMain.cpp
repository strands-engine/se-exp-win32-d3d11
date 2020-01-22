#include <Windows.h>
#include <windowsx.h>

#include "win32_key_code_converter.h"
#include "win32_debug_message_provider.h"

LRESULT CALLBACK WindowProc(_In_ HWND wnd_h, _In_ UINT msg_id, _In_ WPARAM wparam, _In_ LPARAM lparam)
{
#ifdef _DEBUG
	static d3dexp::win32_debug_message_provider s_msg;
	// OutputDebugString(s_msg.for_generic_event(msg_id, wparam, lparam).c_str());
#endif // _DEBUG

	static d3dexp::win32_key_code_converter s_kc_conv;

	switch (msg_id)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_key_event("key down", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
		break;

	case WM_KEYUP:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_key_event("key up", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
		break;

	case WM_SYSKEYDOWN:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_key_event("sys key down", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
		break;

	case WM_SYSKEYUP:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_key_event("sys key up", s_kc_conv.convert(wparam, lparam)).c_str());
#endif // _DEBUG
		break;

	case WM_CHAR:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_char_event(static_cast<char>(wparam)).c_str());
#endif // _DEBUG
		break;

	case WM_LBUTTONDOWN:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("left btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::lmb).c_str());
#endif // _DEBUG
		break;

	case WM_LBUTTONUP:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("left btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::lmb).c_str());
#endif // _DEBUG
		break;

	case WM_RBUTTONDOWN:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("right btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::rmb).c_str());
#endif // _DEBUG
		break;

	case WM_RBUTTONUP:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("right btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::rmb).c_str());
#endif // _DEBUG
		break;

	case WM_MBUTTONDOWN:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("mid btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::mmb).c_str());
#endif // _DEBUG
		break;

	case WM_MBUTTONUP:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("mid btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::mmb).c_str());
#endif // _DEBUG
		break;

	case WM_XBUTTONDOWN:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("x btn down", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::x1mb).c_str());
#endif // _DEBUG
		break;

	case WM_XBUTTONUP:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("x btn up", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), d3dexp::key_t::x1mb).c_str());
#endif // _DEBUG
		break;

	case WM_MOUSEMOVE:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_mouse_event("mouse move", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)).c_str());
#endif // _DEBUG
		break;

	default:
#ifdef _DEBUG
		OutputDebugString(s_msg.for_generic_event(msg_id, wparam, lparam).c_str());
#endif // _DEBUG
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