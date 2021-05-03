#include "d2d1_graphics.h"

#include <stdexcept>

#include "win32_app.h"
#include "d3d11_graphics.h"

namespace d3dexp::bell0bytes
{
	d2d1_graphics::d2d1_graphics(win32_app* app_p)
		: m_app_p(app_p)
	{
		// create d2d device and device context
		auto result = create_device();
		if (!result)
		{
			throw std::runtime_error("Failed to initialize Direct2D device!");
		}


		// create the bitmap target to render to
		result = create_bitmap_renderer_target();
		if (!result)
		{
			throw std::runtime_error("Failed to create the bitmap render target for Direct2D!");
		}

		// initialize the text formats
		result = initialize_text_formats();
		if (!result)
		{
			throw std::runtime_error("Failed to create text formats!");
		}

		OutputDebugStringA("Successfully initialized Direct2D.\n");
	}

	ID2D1SolidColorBrush* d2d1_graphics::brush(colour_t colour) const noexcept
	{
		switch (colour)
		{
		case colour_t::white:
			return m_white_brush_p.Get();
		case colour_t::yellow:
			return m_yellow_brush_p.Get();
		case colour_t::black:
			return m_black_brush_p.Get();
		default:
			return nullptr;
		}
	}

	IDWriteTextFormat* d2d1_graphics::text_format(context_t ctx) const noexcept
	{
		switch (ctx)
		{
		case context_t::fps:
			return m_text_format_for_fps_p.Get();
		default:
			return nullptr;
		}
	}

	IDWriteTextLayout* d2d1_graphics::text_layout(context_t ctx) const noexcept
	{
		switch (ctx)
		{
		case context_t::fps:
			return m_text_layout_for_fps_p.Get();
		default:
			return nullptr;
		}

	}

	expected_t<void> d2d1_graphics::create_text_layout(context_t ctx, std::wstring const& text, float width, float height)
	{
		// get reference to appropriate layout pointer 
		auto& layout_p = m_text_layout_for_fps_p;
		switch (ctx)
		{
		case context_t::fps:
			break;
		default:
			return std::runtime_error{ "Unknown text rendering context." };
		}

		// create appropriate layout
		auto hr = m_dwrite_factory_p->CreateTextLayout(
			text.c_str(),									// text to be drawn
			static_cast<UINT32>(text.size()),				// length of text to be drawn
			text_format(ctx),								// text format used
			width,											// max width of the layout
			height,											// max height of the layout
			&layout_p);										// pointer to created layout
		if (FAILED(hr)) return std::runtime_error("Failed to create the text layout for FPS information!");

		return {};
	}

	expected_t<void> d2d1_graphics::render_text(context_t ctx, colour_t colour)
	{
		switch (ctx)
		{
		case context_t::fps:
		{
			if (m_app_p->is_fps_counter_shown() && text_layout(ctx))
			{
				m_context_p->BeginDraw();
				m_context_p->DrawTextLayout(D2D1::Point2F(2.0f, 5.0f), text_layout(ctx), brush(colour));
				auto hr = m_context_p->EndDraw();
				if (FAILED(hr)) return std::runtime_error{ "Unable to draw FPS information!" };
			}

			return {};
		}
		default:
			return std::runtime_error{ "Unknown text rendering context." };
		}

	}

	expected_t<void> d2d1_graphics::create_device()
	{
		// create shared direct write factory
		auto hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory2), &m_dwrite_factory_p);
		if (FAILED(hr)) return std::runtime_error("Unable to create the DirectWrite factory!");

		// create multi-threaded d2d factory with appropriate debug logging level
		auto d2df_opt = D2D1_FACTORY_OPTIONS{};
#ifdef _DEBUG
		d2df_opt.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
		d2df_opt.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory2), &d2df_opt, &m_d2d_factory_p);
		if (FAILED(hr)) return std::runtime_error("Unable to create the Direct2D factory!");

		// creating d2d device and device context based on d3d device
		auto dxgi_device_p = m_app_p->device_as_dxgi();
		hr = m_d2d_factory_p->CreateDevice(dxgi_device_p.Get(), &m_device_p);
		if (FAILED(hr)) return std::runtime_error{ "Unable to create the Direct2D device!" };
		hr = m_device_p->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_context_p);
		if (FAILED(hr)) return std::runtime_error{ "Unable to create the Direct2D device context!" };
		
		return {};
	}

	expected_t<void> d2d1_graphics::create_bitmap_renderer_target()
	{
		// specify properties of desired bitmap
		auto bm_prop = D2D1_BITMAP_PROPERTIES1{};
		bm_prop.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;	// pixel format - here the same as for d3d, i.e. BGRA
		bm_prop.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;     // alpha mode set to ignore alpha values
		bm_prop.dpiX = 96.0f;										// bimap width in device independent dpi
		bm_prop.dpiY = 96.0f;										// bimap height in device independent dpi
		bm_prop.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET |        // options - set bitmap as rendering target and
			D2D1_BITMAP_OPTIONS_CANNOT_DRAW;						//  make it "readonly" for the client
		bm_prop.colorContext = nullptr;								// pointer to colour context (???)

		// get dxgi surface from d3d swap chain backbuffer
		auto dxgi_surface_p = m_app_p->back_buffer_as_surface();

		// create target bitmap and set it as such
		auto bitmap_p = com_ptr<ID2D1Bitmap1>{};
		auto hr = m_context_p->CreateBitmapFromDxgiSurface(dxgi_surface_p.Get(), &bm_prop, &bitmap_p);
		if (FAILED(hr)) return std::runtime_error("Unable to create the Direct2D bitmap from the DXGI surface!");
		m_context_p->SetTarget(bitmap_p.Get());
		
		return { };
	}

	expected_t<void> d2d1_graphics::initialize_text_formats()
	{
		// create standard brushes
		auto hr = m_context_p->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_white_brush_p);
		if (FAILED(hr)) return std::runtime_error{ "Unable to create white brush." };
		hr = m_context_p->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_yellow_brush_p);
		if (FAILED(hr)) return std::runtime_error{ "Unable to create yellow brush." };
		hr = m_context_p->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_black_brush_p);
		if (FAILED(hr)) return std::runtime_error{ "Unable to create black brush." };


		// create text formats

		// (1) for fps info text
		hr = m_dwrite_factory_p->CreateTextFormat(
			L"Lucida Console",								// choose font family by name from default windows font collection
			nullptr,										// no custom font collection - use windows default
			DWRITE_FONT_WEIGHT_LIGHT,						// font weight - light
			DWRITE_FONT_STYLE_NORMAL,						// font style - normal (not italic)
			DWRITE_FONT_STRETCH_NORMAL,						// font stretch - no horizontal stretching
			12.0f,											// pixel (dpi) size
			L"en-GB",										// locale - english
			&m_text_format_for_fps_p);						// pointer to newly created text format
		if (FAILED(hr)) return std::runtime_error("Unable to create text format for FPS information!");
		hr = m_text_format_for_fps_p->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);			// set text alignment as leading
		if (FAILED(hr)) return std::runtime_error("Unable to set text alignment!");
		hr = m_text_format_for_fps_p->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);   // set paragraph alignment as leading
		if (FAILED(hr)) return std::runtime_error("Unable to set paragraph alignment!");

		return { };
	}

}
