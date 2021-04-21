#include "engine.h"

namespace d3dexp
{
	bool engine::initialize(HINSTANCE instance_h, std::string title, std::string class_name, int width, int height) noexcept
	{
		return m_window.initialize(this, instance_h, title, class_name, width, height);
	}

	void engine::frame()
	{
		while (!m_keyboard.is_char_queue_empty())
		{
			auto msg = std::string("Char read: ");
			msg += m_keyboard.read_char();
			msg += '\n';
			OutputDebugStringA(msg.c_str());
		}

		while (!m_keyboard.is_key_queue_empty())
		{
			auto msg = std::string("Key ");
			auto e = m_keyboard.read_key();
			msg += (e.is_pressed() ? "pressed" : "released");
			msg += ": ";
			msg += e.keycode();
			msg += '\n';
			OutputDebugStringA(msg.c_str());
		}

	}

	bool engine::process_messages() noexcept
	{
		return m_window.process_messages();
	}
}