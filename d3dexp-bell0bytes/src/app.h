#pragma once

#include "win32_app.h"

namespace d3dexp::bell0bytes
{
	class app : public win32_app
	{
	public:
		app(HINSTANCE instance_h) : win32_app(instance_h) {}

		app(app const&) = delete;
		app(app &&) = delete;

		app& operator=(app const&) = delete;
		app& operator=(app &&) = delete;

		~app() = default;

	public:
		expected_t<void> initialize() override;
		void shutdown(expected_t<void>* expected = nullptr) override;

		expected_t<int> run();
	};
}