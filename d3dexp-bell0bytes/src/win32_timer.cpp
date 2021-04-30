#include "win32_timer.h"

namespace d3dexp::bell0bytes
{
	win32_timer::win32_timer()
	{
		// get performance counter frequency (ticks per second)
		auto ticks_per_second = long long int{};
		if (QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&ticks_per_second)))
		{
			m_secs_per_tick = 1.0 / static_cast<double>(ticks_per_second);
		}
		else
		{
			throw std::runtime_error{ "This hardware does not support a high-precision timer!" };
		}
	}

	double win32_timer::total_time() const noexcept
	{
		return static_cast<double>((m_is_paused ? m_paused_time : m_current_time) - m_start_time - m_total_idle_time) * m_secs_per_tick;
	}

	expected_t<void> win32_timer::start()
	{
		if (m_is_paused)
		{
			auto now = long long int{};
			if (QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now)))
			{
				m_total_idle_time += (now - m_paused_time);
				m_previous_time = now;
				m_paused_time = 0LL;
				m_is_paused = false;

				return {};
			}
			else
			{
				return std::runtime_error{ "Unable to query a high-precision timer!" };
			}
		}

		return {};
	}

	expected_t<void> win32_timer::reset()
	{
		auto now = long long int{};
		if (QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now)))
		{
			m_start_time = now;
			m_previous_time = now;
			m_paused_time = 0LL;
			m_is_paused = false;

			return {};
		}
		else
		{
			return std::runtime_error{ "Unable to query a high-precision timer!" };
		}
	}

	expected_t<void> win32_timer::pause()
	{
		if (!m_is_paused)
		{
			auto now = long long int{};
			if (QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now)))
			{
				m_paused_time = now;
				m_is_paused = true;

				return {};
			}
			else
			{
				return std::runtime_error{ "Unable to query a high-precision timer!" };
			}
		}

		return {};
	}

	expected_t<void> win32_timer::update()
	{
		if (m_is_paused)
		{
			m_delta_time = 0.0;
			return {};
		}
		else
		{
			if (QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_current_time)))
			{
				m_delta_time = static_cast<double>(m_current_time - m_previous_time) * m_secs_per_tick;
				m_delta_time = m_delta_time > 0.0 ? m_delta_time : 0.0;
				m_previous_time = m_current_time;

				return {};
			}
			else
			{
				return std::runtime_error{ "Unable to query a high-precision timer!" };
			}
		}
	}
}
