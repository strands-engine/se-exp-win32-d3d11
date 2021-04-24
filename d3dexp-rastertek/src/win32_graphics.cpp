#include "win32_graphics.h"

namespace d3dexp
{
    win32_graphics::win32_graphics(HWND window_h, win32_graphics_settings_t& settings) noexcept
    {
    }

    bool win32_graphics::frame() noexcept
    {
        return true;
    }

    bool win32_graphics::render() noexcept
    {
        return true;
    }
}