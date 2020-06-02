#pragma once

#include "win32_header_wrapper.h"
#include <string>
#include <unordered_map>

namespace d3dexp {

	class win32_debug_message_map
	{
	public:
		win32_debug_message_map() noexcept;

		win32_debug_message_map(win32_debug_message_map const&) = delete;
		win32_debug_message_map(win32_debug_message_map&&) = delete;

		win32_debug_message_map& operator=(win32_debug_message_map const&) = delete;
		win32_debug_message_map& operator=(win32_debug_message_map&&) = delete;

		~win32_debug_message_map() = default;

	public:
		[[nodiscard]] std::string operator() (DWORD msg_id, WPARAM wparam, LPARAM lparam) const noexcept;

	private:
		std::unordered_map<DWORD, std::string> m_map;
	};

}