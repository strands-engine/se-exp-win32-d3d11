#pragma once

#include <shared_mutex>

#include <Windows.h>
#include <wrl/client.h>

#include <d2d1_2.h>                // d2d1_3.h is available
#include <dwrite_2.h>              // dwrite_3.h is available

#include "expected.h"

#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

namespace d3dexp::bell0bytes
{
	class win32_app;

#ifndef D3DEXP_COMPTR
#define D3DEXP_COMPTR

	template <typename T>
	using com_ptr = Microsoft::WRL::ComPtr<T>;

	template <typename T>
	void** to_pp(com_ptr<T>& p)
	{
		return reinterpret_cast<void**>(p.GetAddressOf());
	}

#endif // !D3DEXP_COMPTR

	class d2d1_graphics
	{
	public:
		friend class win32_app;
		friend class d3d11_graphics;

		enum class colour_t
		{
			white,
			yellow,
			black
		};

		enum class context_t
		{
			fps
		};

	public:
		d2d1_graphics(win32_app* app_p);

		d2d1_graphics(d2d1_graphics const&) = delete;
		d2d1_graphics(d2d1_graphics&&) = delete;

		d2d1_graphics& operator=(d2d1_graphics const&) = delete;
		d2d1_graphics& operator=(d2d1_graphics&&) = delete;

		~d2d1_graphics() noexcept = default;

	public:
		[[nodiscard]] ID2D1SolidColorBrush* brush(colour_t colour) const noexcept;
		[[nodiscard]] IDWriteTextFormat* text_format(context_t ctx) const noexcept;
		[[nodiscard]] IDWriteTextLayout* text_layout(context_t ctx) const noexcept;

		expected_t<void> create_text_layout(context_t ctx, std::wstring const& text, float width, float height);

		expected_t<void> render_text(context_t ctx, colour_t colour);

	private:
		expected_t<void> create_device();
		expected_t<void> create_bitmap_renderer_target();
		expected_t<void> initialize_text_formats();

	private:
		com_ptr<IDWriteFactory2> m_dwrite_factory_p = nullptr;
		com_ptr<ID2D1Factory2> m_d2d_factory_p = nullptr;
		com_ptr<ID2D1Device1> m_device_p = nullptr;
		com_ptr<ID2D1DeviceContext1> m_context_p = nullptr;

		win32_app* m_app_p = nullptr;

		com_ptr<ID2D1SolidColorBrush> m_white_brush_p = nullptr;
		com_ptr<ID2D1SolidColorBrush> m_yellow_brush_p = nullptr;
		com_ptr<ID2D1SolidColorBrush> m_black_brush_p = nullptr;

		com_ptr<IDWriteTextFormat> m_text_format_for_fps_p = nullptr;
		
		com_ptr<IDWriteTextLayout> m_text_layout_for_fps_p = nullptr;
	};

}
