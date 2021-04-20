#include "engine.h"

namespace d3dexp
{
	bool engine::initialize(HINSTANCE instance_h, std::string title, std::string class_name, int width, int height) noexcept
	{
		return m_window.initialize(this, instance_h, title, class_name, width, height);
	}
	bool engine::process_messages() noexcept
	{
		return m_window.process_messages();
	}
}