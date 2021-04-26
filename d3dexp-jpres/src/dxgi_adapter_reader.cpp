#include "dxgi_adapter_reader.h"

#include "error_logger.h"

namespace d3dexp::jpres
{
	dxgi_adapter_data::dxgi_adapter_data(IDXGIAdapter* adapter_p) noexcept
		: m_adapter_p(adapter_p)
	{
		// load and store description of given adapter
		auto hr = m_adapter_p->GetDesc(&m_description);
		if (FAILED(hr))
		{
			error_logger::log(hr, "Failed to load description for DXGI adapter.");
		}
	}

	std::vector<dxgi_adapter_data> dxgi_adapter_reader::s_adapters;

	std::vector<dxgi_adapter_data> const& dxgi_adapter_reader::adapters() noexcept
	{
		if (s_adapters.empty())
		{
			reload_adapter_data();
		}
		return s_adapters;
	}

	void dxgi_adapter_reader::reload_adapter_data() noexcept
	{
		auto dxgi_factory_p = com_ptr<IDXGIFactory>{};

		// create dxgi factory to enumerate available adapters
		auto hr = CreateDXGIFactory(__uuidof(IDXGIFactory), to_pp(dxgi_factory_p));
		if (FAILED(hr))
		{
			error_logger::log(hr, "Failed to create DXGI factory for enumerating adapters.");
			exit(-1);
		}

		// use created factory to enumerate available adapters
		auto adapter_p = com_ptr<IDXGIAdapter>{};
		auto i = UINT{ 0u };
		while (SUCCEEDED(dxgi_factory_p->EnumAdapters(i, &adapter_p)))
		{
			s_adapters.emplace_back(adapter_p.Get());
			i++;
		}
	}
}