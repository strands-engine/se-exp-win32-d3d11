#pragma once

#include <Windows.h>
#include <tuple>
#include <string>
#include <unordered_map>

#include "input_types.h"

namespace d3dexp {

	class debug_key_info_map
	{
	public:
		debug_key_info_map() noexcept;
		std::string operator() (char const * context, key_t key) const noexcept;

	private:
		std::unordered_map<key_t, std::pair<std::string, std::string>> m_map;
	};

}