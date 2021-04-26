#pragma once

#include "win32_header_wrapper.h"

#include <memory>

#include "input_types.h"
#include "win32_debug_message_map.h"
#include "debug_key_info_map.h"


namespace d3dexp::chili
{
	class win32_debug_message_provider
	{
	public:
		win32_debug_message_provider() noexcept;

		win32_debug_message_provider(win32_debug_message_provider const&) = delete;
		win32_debug_message_provider(win32_debug_message_provider &&) = delete;

		win32_debug_message_provider& operator=(win32_debug_message_provider const&) = delete;
		win32_debug_message_provider& operator=(win32_debug_message_provider &&) = delete;

		~win32_debug_message_provider() noexcept = default;

	public:
		[[nodiscard]] std::string for_key_event(char const * context, key_t key) const noexcept;
		[[nodiscard]] std::string for_char_event(char character) const noexcept;
		[[nodiscard]] std::string for_mouse_event(char const* context, std::int16_t x_pos, std::int16_t y_pos, key_t mb = static_cast<key_t>(0xff)) const noexcept;
		[[nodiscard]] std::string for_mouse_wheel_event(std::int16_t x_pos, std::int16_t y_pos, std::int16_t delta) const noexcept;

		[[nodiscard]] std::string for_generic_event(DWORD msg_id, WPARAM wparam, LPARAM lparam) const noexcept;

	private:
		std::unique_ptr<win32_debug_message_map> msg_map_p;
		std::unique_ptr<debug_key_info_map> key_map_p;
	};
}