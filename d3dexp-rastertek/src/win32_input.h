#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace d3dexp
{
	class win32_input
	{
	public:
		win32_input() noexcept = default;

		win32_input(win32_input const&) = delete;
		win32_input(win32_input &&) = delete;

		win32_input& operator=(win32_input const&) = delete;
		win32_input& operator=(win32_input &&) = delete;

		~win32_input() noexcept = default;
	};
}