#pragma once

#include<vector>

#include <d3d11.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DXGI.lib")

namespace d3dexp
{
	template <typename T>
	using com_ptr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	void** to_pp(com_ptr<T>& p)
	{
		return reinterpret_cast<void**>(p.GetAddressOf());
	}

	class dxgi_adapter_data
	{
		// NOTE: To be expanded to access useful device data
	public:
		explicit dxgi_adapter_data(IDXGIAdapter* adapter_p) noexcept;

	private:
		DXGI_ADAPTER_DESC m_description;
		com_ptr<IDXGIAdapter> m_adapter_p;
	};

	class dxgi_adapter_reader
	{
	public:
		dxgi_adapter_reader() = delete;

	public:
		static std::vector<dxgi_adapter_data> const& adapters() noexcept;
		static void reload_adapter_data() noexcept;

	private:
		static std::vector<dxgi_adapter_data> s_adapters;
	};
}