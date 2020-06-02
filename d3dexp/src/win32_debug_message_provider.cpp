#pragma once

#include "win32_debug_message_provider.h"

#include <sstream>
#include <iomanip>

#include "win32_debug_message_map.h"
#include "debug_key_info_map.h"

namespace d3dexp
{
	win32_debug_message_provider::win32_debug_message_provider() noexcept 
		: msg_map_p(std::make_unique<win32_debug_message_map>()),
		  key_map_p(std::make_unique<debug_key_info_map>())
	{}

	[[nodiscard]] std::string win32_debug_message_provider::for_key_event(char const * context, key_t key) const noexcept
	{
		return (*key_map_p)(context, key);
	}
	
	[[nodiscard]] std::string win32_debug_message_provider::for_mouse_event(char const * context, std::int16_t x_pos, std::int16_t y_pos, key_t mb /* static_cast<key_t>(0xff) */) const noexcept
	{
		return (*key_map_p)(context, mb, x_pos, y_pos);
	}
	
	[[nodiscard]] std::string win32_debug_message_provider::for_char_event(char character) const noexcept
	{
		constexpr int first_column_width = 15;

		std::ostringstream oss;
		oss << std::left << std::setw(first_column_width) << "char" << "['" << character << "']\n";

		return oss.str();
	}

	[[nodiscard]] std::string win32_debug_message_provider::for_generic_event(DWORD msg_id, WPARAM wparam, LPARAM lparam) const noexcept
	{
		return (*msg_map_p)(msg_id, wparam, lparam);
	}

}