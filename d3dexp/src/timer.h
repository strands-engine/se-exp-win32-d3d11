#pragma once

#include <chrono>

namespace d3dexp::chili
{
	class timer
	{
	public:
		timer() : m_last_tp(std::chrono::steady_clock::now()) {}

	public:
		float peek() const { return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_last_tp).count(); }
		
		float mark();

	private:
		std::chrono::steady_clock::time_point m_last_tp;
	};
}