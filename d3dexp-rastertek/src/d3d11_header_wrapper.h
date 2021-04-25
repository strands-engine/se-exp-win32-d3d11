#pragma once

#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace dxm = DirectX;

namespace d3dexp
{
	template <typename T>
	using com_ptr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	void** to_pp(com_ptr<T>& p)
	{
		return reinterpret_cast<void**>(p.GetAddressOf());
	}

	struct d3d11_graphics_settings_t
	{
		float screen_near_plane = 1000.0f;
		float screen_far_plane = 0.1f;
		float field_of_view = 3.141592654f / 4.0f;
		int width = 800;
		int height = 600;
		bool is_vsync_enabled = true;
		bool is_fullscreen = false;
	};
}