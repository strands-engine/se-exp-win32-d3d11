#pragma once

#include "win32_header_wrapper.h"
#include <dxgidebug.h>

#include <string>
#include <vector>

namespace d3dexp::chili
{
	class dxgi_debug_info_manager
	{
	public:
		dxgi_debug_info_manager();

		dxgi_debug_info_manager(dxgi_debug_info_manager const&) = delete;
		dxgi_debug_info_manager(dxgi_debug_info_manager &&) = delete;

		dxgi_debug_info_manager& operator=(dxgi_debug_info_manager const&) = delete;
		dxgi_debug_info_manager& operator=(dxgi_debug_info_manager &&) = delete;

		~dxgi_debug_info_manager() noexcept = default;

	public:
		[[nodiscard]] bool has_new_messages() const noexcept;

		void mark() noexcept;
		std::vector<std::string> get_messages();
	

	private:
		unsigned long long m_next_message_id = 0ul;
		com_ptr<IDXGIInfoQueue> m_queue_p = nullptr;
	};
}