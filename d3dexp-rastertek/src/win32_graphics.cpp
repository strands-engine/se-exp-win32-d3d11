#include "win32_graphics.h"

namespace d3dexp
{
    win32_graphics::win32_graphics(HWND window_h, d3d11_graphics_settings_t& settings) noexcept
        : m_d3dgfx(window_h, settings)
    {}

    bool win32_graphics::frame() noexcept
    {
        if (!render())
        {
            return false;
        }

        return true;
    }

    bool win32_graphics::render() noexcept
    {
        // clear rendering buffers to prepare new frame for drawing
        m_d3dgfx.begin_scene(0.0f, 1.0f, 1.0f, 1.0f);

        // present rendered scene to the screen
        m_d3dgfx.end_scene();

        return true;
    }
}