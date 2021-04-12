#include "timer.h"

namespace d3dexp
{
	float timer::mark()
	{
		const auto old_tp = m_last_tp;
		m_last_tp = std::chrono::steady_clock::now();
		return std::chrono::duration<float>(m_last_tp - old_tp).count();
	}
}