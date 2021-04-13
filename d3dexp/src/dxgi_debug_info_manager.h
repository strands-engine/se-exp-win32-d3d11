#pragma once

#include "win32_header_wrapper.h"
#include <dxgidebug.h>

#include <string>
#include <vector>

namespace d3dexp
{
	class dxgi_debug_info_manager
	{
	public:
		dxgi_debug_info_manager();

		dxgi_debug_info_manager(dxgi_debug_info_manager const&) = delete;
		dxgi_debug_info_manager(dxgi_debug_info_manager &&) = delete;

		dxgi_debug_info_manager& operator=(dxgi_debug_info_manager const&) = delete;
		dxgi_debug_info_manager& operator=(dxgi_debug_info_manager &&) = delete;

		~dxgi_debug_info_manager();

	public:
		std::vector<std::string> get_messages();
	
		void mark() noexcept;

	private:
		unsigned long long m_next_message_id = 0ul;
		IDXGIInfoQueue* m_queue_p = nullptr;
	};
}