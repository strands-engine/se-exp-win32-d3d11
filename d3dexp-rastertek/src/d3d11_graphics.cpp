#include "d3d11_graphics.h"

#include <vector>

namespace d3dexp
{
	d3d11_graphics::d3d11_graphics(HWND window_h, d3d11_graphics_settings_t const& settings) noexcept
		: m_width(settings.width), m_height(settings.height), 
		  m_near_plane(settings.screen_near_plane), m_far_plane(settings.screen_far_plane),
		  m_is_fullscreen(settings.is_fullscreen), m_is_vsync_enabled(settings.is_vsync_enabled)
	{
		// QUERYING ADAPTER AND SCREEN DATA
		// here: to obtain refresh rate for fullscreen mode
		// later: to list/choose adapters and screen modes

		// create dxgi factory to enumerate adapters
		auto dxgi_factory_p = com_ptr<IDXGIFactory>{};
		auto hr = CreateDXGIFactory(__uuidof(IDXGIFactory), to_pp(dxgi_factory_p));
		if (FAILED(hr))
		{
			OutputDebugString(L"Failed to create DXGI factory.");
			exit(-1);
		}

		// enumerate available graphics adapters
		auto adapter_p = com_ptr<IDXGIAdapter>{};
		auto adapters = std::vector<com_ptr<IDXGIAdapter>>{};
		auto i = UINT{ 0u };
		while (SUCCEEDED(dxgi_factory_p->EnumAdapters(i, &adapter_p)))
		{
			adapters.emplace_back(adapter_p.Get());
			i++;
		}

		// debug output of enumerated adapters
		auto adapter_desc = DXGI_ADAPTER_DESC{};
		for (auto i = std::size_t{ 0u }; i < adapters.size(); i++)
		{
			adapters[i]->GetDesc(&adapter_desc);
			auto msg = std::wstring{ L"ADAPTER #" };
			msg += std::to_wstring(i + 1);
			msg += L": ";
			msg += adapter_desc.Description;
			msg += L" [";
			msg += std::to_wstring(adapter_desc.DedicatedVideoMemory / 1024 / 1024);
			msg += L" MB]\n";
			OutputDebugString(msg.c_str());
		}

		// choose first available adapter and store its description and dedicated video memory
		adapter_p = adapters.front();
		adapter_p->GetDesc(&adapter_desc);
		m_adapter_name = adapter_desc.Description;
		m_adapter_dedicated_memory = adapter_desc.DedicatedVideoMemory;


		// enumerate available outputs of chosed graphics adapter
		auto output_p = com_ptr<IDXGIOutput>{};
		auto outputs = std::vector<com_ptr<IDXGIOutput>>{};
		i = UINT{ 0u };
		while (SUCCEEDED(adapter_p->EnumOutputs(i, &output_p)))
		{
			outputs.emplace_back(output_p.Get());
			i++;
		}

		// debug output of enumerated outputs
		auto output_desc = DXGI_OUTPUT_DESC{};
		for (auto i = std::size_t{ 0u }; i < outputs.size(); i++)
		{
			outputs[i]->GetDesc(&output_desc);
			auto msg = std::wstring{ L"OUTPUT #" };
			msg += std::to_wstring(i + 1);
			msg += L": ";
			msg += output_desc.DeviceName;
			msg += L"\n";
			OutputDebugString(msg.c_str());
		}

		// choose first available output and get all its supported display modes
		output_p = outputs.front();

		// get size of modes array
		const auto desired_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		// const auto desired_format = DXGI_FORMAT_B8G8R8A8_UNORM; - better format [chili + other comments]
		auto modes_count = UINT{};
		hr = output_p->GetDisplayModeList(desired_format, DXGI_ENUM_MODES_INTERLACED, &modes_count, nullptr);
		if (FAILED(hr)) exit(-1);

		// obtain modes array
		auto display_modes = new DXGI_MODE_DESC[modes_count];
		hr = output_p->GetDisplayModeList(desired_format, DXGI_ENUM_MODES_INTERLACED, &modes_count, display_modes);
		if (FAILED(hr)) exit(-1);

		// debug output of enumerated display modes
		for (auto i = std::size_t{ 0u }; i < modes_count; i++)
		{
			auto msg = std::wstring{ L"DISPLAY MODE #" };
			msg += std::to_wstring(i + 1);
			msg += L": ";
			msg += std::to_wstring(display_modes[i].Width);
			msg += L" x ";
			msg += std::to_wstring(display_modes[i].Height);
			msg += L" [";
			msg += std::to_wstring(display_modes[i].RefreshRate.Numerator / static_cast<float>(display_modes[i].RefreshRate.Denominator));
			msg += L" Hz]\n";
			OutputDebugString(msg.c_str());
		}

		// if vsync is enabled - look through all available display modes to find one matching screen dimensions and get its refresh rate
		// NOTE: todo enumerate all available refresh rates to choose
		if (m_is_vsync_enabled)
		{
			for (auto i = std::size_t{ 0u }; i < modes_count; i++)
			{
				if (display_modes[i].Width == static_cast<UINT>(m_width) && display_modes[i].Height == static_cast<UINT>(m_height))
				{
					m_refresh_rate = display_modes[i].RefreshRate;
					break;
				}
			}
		}

		// cleanup display modes list
		delete[] display_modes;


		// CREATION OF D3D11 DEVICE, IMMEDIATE CONTEXT AND SWAP CHAIN

		// setting up the swap chain options
		auto sc_desc = DXGI_SWAP_CHAIN_DESC{};

		sc_desc.BufferCount = 1;													// double buffering setup
		sc_desc.BufferDesc.Width = m_width;											// output buffer width
		sc_desc.BufferDesc.Height = m_height;										// output buffer height
		sc_desc.BufferDesc.Format = desired_format;									// output buffer pixel format - prefer BGRA [chili + other comments]
		sc_desc.BufferDesc.RefreshRate = m_refresh_rate;							// if vsync is enabled set to output RR, oterwise default 0=0/1
		sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;						// set buffer usage to back buffer
		sc_desc.OutputWindow = window_h;											// handle to output window
		sc_desc.SampleDesc.Count = 1;												// disable multisampling
		sc_desc.SampleDesc.Quality = 0;												// disable multisampling
		sc_desc.Windowed = BOOL{ !m_is_fullscreen };								// set mode to windowed or fullscreen
		sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// scanline ordering - unspecified
		sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					// scaling mode - unspecified
		sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                              // discard back buffer on swap
		sc_desc.Flags = 0;															// no extra option flags

		// choose desired feature level
		D3D_FEATURE_LEVEL desired_feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
		auto used_feature_level = D3D_FEATURE_LEVEL{};

		// create device, device context and swap chain
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,							// use default adapter (optionally pass previously chosed adapter)
			D3D_DRIVER_TYPE_HARDWARE,           // specified to chose default hardware adapter (if precisely given - change to UNKNOWN)
			nullptr,							// no handle to lib for software backend
			0,									// no extra runtime flags
			desired_feature_levels,				// desired feature level
			ARRAYSIZE(desired_feature_levels),	// number of desired feature levels
			D3D11_SDK_VERSION,					// sdk version macro
			&sc_desc,							// options for swap chain setup
			&m_swap_chain_p,					// pointer to created swap chain
			&m_device_p,						// pointer to created device
			&used_feature_level,				// pointer to actually used feature level
			&m_context_p);						// pointer to created immediate device context
		if (FAILED(hr))
		{
			OutputDebugString(L"Failed to initialize D3D11 device, context, and swap chain.");
			exit(-1);
		}

		// debug output of used feature level
		switch (used_feature_level)
		{
		case D3D_FEATURE_LEVEL_11_0:
		{
			OutputDebugString(L"D3D FEATURE LEVEL - 11.0\n");
			break;
		}
		case D3D_FEATURE_LEVEL_11_1:
		{
			OutputDebugString(L"D3D FEATURE LEVEL - 11.1\n");
			break;
		}
		default:
			break;
		}


		/*

		ID3D11Texture2D* backBufferPtr;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_RASTERIZER_DESC rasterDesc;
		D3D11_VIEWPORT viewport;
		float fieldOfView, screenAspect;


		Sometimes this call to create the device will fail if the primary video card is not compatible with DirectX 11. Some machines may have the primary card as a DirectX 10 video card and the secondary card as a DirectX 11 video card.Also some hybrid graphics cards work that way with the primary being the low power Intel card and the secondary being the high power Nvidia card.To get around this you will need to not use the default deviceand instead enumerate all the video cards in the machineand have the user choose which one to useand then specify that card when creating the device.

			Now that we have a swap chain we need to get a pointer to the back bufferand then attach it to the swap chain.We'll use the CreateRenderTargetView function to attach the back buffer to our swap chain.

			// Get the pointer to the back buffer.
			result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if (FAILED(result))
		{
			return false;
		}

		// Create the render target view with the back buffer pointer.
		result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
		if (FAILED(result))
		{
			return false;
		}

		// Release pointer to the back buffer as we no longer need it.
		backBufferPtr->Release();
		backBufferPtr = 0;
		We will also need to set up a depth buffer description.We'll use this to create a depth buffer so that our polygons can be rendered properly in 3D space. At the same time we will attach a stencil buffer to our depth buffer. The stencil buffer can be used to achieve effects such as motion blur, volumetric shadows, and other things.

			// Initialize the description of the depth buffer.
			ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		// Set up the description of the depth buffer.
		depthBufferDesc.Width = screenWidth;
		depthBufferDesc.Height = screenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		Now we create the depth / stencil buffer using that description.You will notice we use the CreateTexture2D function to make the buffers, hence the buffer is just a 2D texture.The reason for this is that once your polygons are sortedand then rasterized they just end up being colored pixels in this 2D buffer.Then this 2D buffer is drawn to the screen.

			// Create the texture for the depth buffer using the filled out description.
			result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
		if (FAILED(result))
		{
			return false;
		}
		Now we need to setup the depth stencil description.This allows us to control what type of depth test Direct3D will do for each pixel.

			// Initialize the description of the stencil state.
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		With the description filled out we can now create a depth stencil state.

			// Create the depth stencil state.
			result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(result))
		{
			return false;
		}
		With the created depth stencil state we can now set it so that it takes effect.Notice we use the device context to set it.

			// Set the depth stencil state.
			m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
		The next thing we need to create is the description of the view of the depth stencil buffer.We do this so that Direct3D knows to use the depth buffer as a depth stencil texture.After filling out the description we then call the function CreateDepthStencilView to create it.

			// Initialize the depth stencil view.
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		// Set up the depth stencil view description.
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// Create the depth stencil view.
		result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result))
		{
			return false;
		}
		With that created we can now call OMSetRenderTargets.This will bind the render target viewand the depth stencil buffer to the output render pipeline.This way the graphics that the pipeline renders will get drawn to our back buffer that we previously created.With the graphics written to the back buffer we can then swap it to the frontand display our graphics on the user's screen.

			// Bind the render target view and depth stencil buffer to the output render pipeline.
			m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
		Now that the render targets are setup we can continue on to some extra functions that will give us more control over our scenes for future tutorials.First thing is we'll create is a rasterizer state. This will give us control over how polygons are rendered. We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons. By default DirectX already has a rasterizer state set up and working the exact same as the one below but you have no control to change it unless you set up one yourself.

			// Setup the raster description which will determine how and what polygons will be drawn.
			rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
		if (FAILED(result))
		{
			return false;
		}

		// Now set the rasterizer state.
		m_deviceContext->RSSetState(m_rasterState);
		The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space.Set this to be the entire size of the window.

			// Setup the viewport for rendering.
			viewport.Width = (float)screenWidth;
		viewport.Height = (float)screenHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;

		// Create the viewport.
		m_deviceContext->RSSetViewports(1, &viewport);
		Now we will create the projection matrix.The projection matrix is used to translate the 3D scene into the 2D viewport space that we previously created.We will need to keep a copy of this matrix so that we can pass it to our shaders that will be used to render our scenes.

			// Setup the projection matrix.
			fieldOfView = 3.141592654f / 4.0f;
		screenAspect = (float)screenWidth / (float)screenHeight;

		// Create the projection matrix for 3D rendering.
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
		We will also create another matrix called the world matrix.This matrix is used to convert the vertices of our objects into vertices in the 3D scene.This matrix will also be used to rotate, translate, and scale our objects in 3D space.From the start we will just initialize the matrix to the identity matrix and keep a copy of it in this object.The copy will be needed to be passed to the shaders for rendering also.

			// Initialize the world matrix to the identity matrix.
			m_worldMatrix = XMMatrixIdentity();
		This is where you would generally create a view matrix.The view matrix is used to calculate the position of where we are looking at the scene from.You can think of it as a cameraand you only view the scene through this camera.Because of its purpose I am going to create it in a camera class in later tutorials since logically it fits better thereand just skip it for now.

			And the final thing we will setup in the Initialize function is an orthographic projection matrix.This matrix is used for rendering 2D elements like user interfaces on the screen allowing us to skip the 3D rendering.You will see this used in later tutorials when we look at rendering 2D graphics and fonts to the screen.

			// Create an orthographic projection matrix for 2D rendering.
			m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

		return true;

		*/

	}

	void d3d11_graphics::begin_scene(float r, float b, float g, float a) noexcept
	{
	}

	void d3d11_graphics::end_scene() noexcept
	{
	}
}