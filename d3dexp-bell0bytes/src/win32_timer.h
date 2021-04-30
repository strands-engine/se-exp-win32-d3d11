#pragma once

#include <Windows.h>

#include "expected.h"

namespace d3dexp::bell0bytes
{
	class win32_timer
	{
	public:
		win32_timer();

	public:
		[[nodiscard]] double delta_time() const noexcept { return m_delta_time; }
		[[nodiscard]] double total_time() const noexcept;

		expected_t<void> start();
		expected_t<void> reset();
		expected_t<void> pause();
		expected_t<void> update();

	private:
		long long int m_start_time = 0;
		long long int m_total_idle_time = 0;
		long long int m_paused_time = 0;
		long long int m_current_time = 0;
		long long int m_previous_time = 0;

		double m_secs_per_tick = 0.0;
		double m_delta_time = 0.0;

		bool m_is_paused = false;
	};
}



/*

class Timer
{
public:
	// constructor
	Timer();

	// getters: return time measured in seconds
	double getTotalTime() const;		// returns the total time the game has been running (minus paused time)
	double getDeltaTime() const;		// returns the time between two frames

	// methods
	util::Expected<void> start();		// starts the timer, called each time the game is unpaused
	util::Expected<void> reset();		// sets the counter to zero, called once before message loop
	util::Expected<void> tick();		// called every frame, lets the time tick
	util::Expected<void> stop();		// called when the game is paused
};

*/