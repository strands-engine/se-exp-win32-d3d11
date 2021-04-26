#pragma once

#include "win32_header_wrapper.h"
#include <tuple>
#include <string>
#include <unordered_map>

#include "input_types.h"

namespace d3dexp::chili
{

	class debug_key_info_map
	{
	public:
		debug_key_info_map() noexcept;

		debug_key_info_map(debug_key_info_map const&) = delete;
		debug_key_info_map(debug_key_info_map&&) = delete;

		debug_key_info_map& operator=(debug_key_info_map const&) = delete;
		debug_key_info_map& operator=(debug_key_info_map&&) = delete;

		~debug_key_info_map() = default;

	public:
		[[nodiscard]] std::string operator() (char const * context, key_t key) const noexcept;
		[[nodiscard]] std::string operator() (char const * context, key_t key, std::int16_t x_pos, std::int16_t y_pos) const noexcept;

	private:
		std::unordered_map<key_t, std::pair<std::string, std::string>> m_map;
	};

}