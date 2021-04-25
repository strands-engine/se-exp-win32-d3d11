#include "win32_graphics.h"

namespace d3dexp
{
    win32_graphics::win32_graphics(HWND window_h, d3d11_graphics_settings_t& settings) noexcept
        : m_d3dgfx(window_h, settings),
        m_camera({ 0.0f, 0.0f, -0.5f }, {})
    {
        m_model_p = std::make_unique<d3d11_model>(m_d3dgfx.device());
        m_shader_p = std::make_unique<d3d11_colour_shader>(m_d3dgfx.device(), window_h);
    }

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

        // update camera pos/rot here if needed
        // ------

        // bind model data to pipeline
        m_model_p->render(m_d3dgfx.context());

        // render model using set up shader
        if (!m_shader_p->render(m_d3dgfx.context(), m_model_p->index_count(), m_d3dgfx.world(), m_camera.view(), m_d3dgfx.projection())) return false;

        // present rendered scene to the screen
        m_d3dgfx.end_scene();

        return true;
    }
}