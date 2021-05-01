#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

namespace d3dexp::bell0bytes
{
	template <typename T>
	using com_ptr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	void** to_pp(com_ptr<T>& p)
	{
		return reinterpret_cast<void**>(p.GetAddressOf());
	}

	class d3d11_graphics
	{
	public:
		d3d11_graphics();

		d3d11_graphics(d3d11_graphics const&) = delete;
		d3d11_graphics(d3d11_graphics &&) = delete;

		d3d11_graphics& operator=(d3d11_graphics const&) = delete;
		d3d11_graphics& operator=(d3d11_graphics &&) = delete;

		~d3d11_graphics() noexcept = default;

	public:

	private:

	private:
		com_ptr<ID3D11Device> m_device_p = nullptr;
		com_ptr<ID3D11DeviceContext> m_context_p = nullptr;
	};
}