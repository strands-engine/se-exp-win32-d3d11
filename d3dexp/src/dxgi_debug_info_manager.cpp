#include "dxgi_debug_info_manager.h"

#include <memory>

#include "win32_window.h"
#include "win32_exception.h"

#pragma comment(lib, "dxguid.lib")

namespace d3dexp
{
	dxgi_debug_info_manager::dxgi_debug_info_manager()
	{
#ifdef _DEBUG
		// loading dxgidebug dll as module
		const auto dxgi_debug_module_h = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		if (dxgi_debug_module_h == nullptr)
		{
			RAISE_WIN32_LAST_ERROR();
		}
		
		// get address of desired query function from the library
		const auto dxgi_debug_interface_p = reinterpret_cast<void*>(GetProcAddress(dxgi_debug_module_h, "DXGIGetDebugInterface"));
		if (dxgi_debug_interface_p == nullptr)
		{
			RAISE_WIN32_LAST_ERROR();
		}

		// specify function's signature and perform necessary cast
		typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);
		const auto dxgi_get_debug_interface = reinterpret_cast<DXGIGetDebugInterface>(dxgi_debug_interface_p);

		// for error handling
		auto hr = dxgi_get_debug_interface(__uuidof(IDXGIInfoQueue), to_pp(m_queue_p));
		if (FAILED(hr))
		{
			RAISE_WIN32_ERROR(hr);
		}
#endif // !_DEBUG
	}

	std::vector<std::string> dxgi_debug_info_manager::get_messages()
	{
		auto messages = std::vector<std::string>{};
#ifdef _DEBUG
		const auto last_message_id = m_queue_p->GetNumStoredMessages(DXGI_DEBUG_ALL);
		for (auto i = m_next_message_id; i < last_message_id; i++)
		{
			auto hr = HRESULT{};
			auto len = SIZE_T{};

			// get size of i-th message in bytes
			hr = m_queue_p->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &len);
			if (FAILED(hr))
			{
				RAISE_WIN32_ERROR(hr);
			}

			// allocate memory for requested message
			auto buf = std::make_unique<byte[]>(len);
			auto msg_p = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(buf.get());

			// get requested message and add its description to the message vector
			hr = m_queue_p->GetMessage(DXGI_DEBUG_ALL, i, msg_p, &len);
			if (FAILED(hr))
			{
				RAISE_WIN32_ERROR(hr);
			}
			messages.emplace_back(msg_p->pDescription);
		}

		mark();

#endif // !_DEBUG
		return messages;
	}

	void dxgi_debug_info_manager::mark() noexcept
	{
#ifdef _DEBUG
		// sets the next message index so that further calls to GetMessages return only new ones
		m_next_message_id = m_queue_p->GetNumStoredMessages(DXGI_DEBUG_ALL);
#endif // !_DEBUG
	}
}