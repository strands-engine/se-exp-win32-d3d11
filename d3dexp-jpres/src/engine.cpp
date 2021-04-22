#include "engine.h"

namespace d3dexp
{
	bool engine::initialize(HINSTANCE instance_h, std::string title, std::string class_name, int width, int height) noexcept
	{
		if (!m_window.initialize(this, instance_h, title, class_name, width, height))
		{
			return false;
		}

		if (!m_graphics.initialize(m_window.handle(), width, height))
		{
			return false;
		}

		return true;
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
			msg += std::to_string(static_cast<int>(e.keycode()));
			msg += '\n';
			OutputDebugStringA(msg.c_str());
		}

		while (!m_mouse.is_queue_empty())
		{
			auto msg = std::string("Mouse ");
			auto e = m_mouse.read();
			switch (e.action())
			{
			case win32_mouse_event::action_t::lmb_pressed:
				msg += "+LMB";
				break;
			case win32_mouse_event::action_t::lmb_released:
				msg += "-LMB";
				break;
			case win32_mouse_event::action_t::rmb_pressed:
				msg += "+RMB";
				break;
			case win32_mouse_event::action_t::rmb_released:
				msg += "-RMB";
				break;
			case win32_mouse_event::action_t::mmb_pressed:
				msg += "+MMB";
				break;
			case win32_mouse_event::action_t::mmb_released:
				msg += "-MMB";
				break;
			case win32_mouse_event::action_t::wheel_up:
				msg += "^ WH";
				break;
			case win32_mouse_event::action_t::wheel_down:
				msg += "_ WH";
				break;
			case win32_mouse_event::action_t::move:
				msg += "MOVE";
				break;
			case win32_mouse_event::action_t::move_raw:
				msg += "rawM";
				break;
			default:
				break;
			}

			msg += " at (";
			if (e.action() == win32_mouse_event::action_t::move_raw) msg += 'd';
			msg += "x: ";
			msg += std::to_string(e.x_pos());
			msg += ", ";
			if (e.action() == win32_mouse_event::action_t::move_raw) msg += 'd';
			msg += "y: ";
			msg += std::to_string(e.y_pos());
			msg += ")\n";
			OutputDebugStringA(msg.c_str());
		}

	}

	bool engine::process_messages() noexcept
	{
		return m_window.process_messages();
	}
}