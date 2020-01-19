#pragma once

#include <Windows.h>
#include <string>
#include <unordered_map>

namespace d3dexp {

	class win32_debug_message_map
	{
	public:
		win32_debug_message_map() noexcept;
		std::string operator() (DWORD msg_id, WPARAM wparam, LPARAM lparam) const noexcept;

	private:
		std::unordered_map<DWORD, std::string> m_map;
	};

}