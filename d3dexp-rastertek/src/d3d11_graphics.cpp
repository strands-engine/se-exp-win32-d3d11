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

		// for fullscreen - look through all available display modes to find one matching screen dimensions and get its refresh rate
		// NOTE: todo enumerate all available refresh rates to choose
		for (auto i = std::size_t{ 0u }; i < modes_count; i++)
		{
			if (display_modes[i].Width == static_cast<UINT>(m_width) && display_modes[i].Height == static_cast<UINT>(m_height))
			{
				m_refresh_rate = display_modes[i].RefreshRate;
				break;
			}
		}

		// cleanup display modes list
		delete[] display_modes;


		// CREATION OF D3D11 DEVICE, IMMEDIATE CONTEXT AND SWAP CHAIN


		/*

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		D3D_FEATURE_LEVEL featureLevel;
		ID3D11Texture2D* backBufferPtr;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		D3D11_RASTERIZER_DESC rasterDesc;
		D3D11_VIEWPORT viewport;
		float fieldOfView, screenAspect;


			// Initialize the swap chain description.
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		// Set to a single back buffer.
		swapChainDesc.BufferCount = 1;

		// Set the width and height of the back buffer.
		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;

		// Set regular 32-bit surface for the back buffer.
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		The next part of the description of the swap chain is the refresh rate.The refresh rate is how many times a second it draws the back buffer to the front buffer.If vsync is set to true in our graphicsclass.h header then this will lock the refresh rate to the system settings(for example 60hz).That means it will only draw the screen 60 times a second(or higher if the system refresh rate is more than 60).However if we set vsync to false then it will draw the screen as many times a second as it can, however this can cause some visual artifacts.

			// Set the refresh rate of the back buffer.
			if (m_vsync_enabled)
			{
				swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
				swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
			}
			else
			{
				swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
				swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			}

		// Set the usage of the back buffer.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// Set the handle for the window to render to.
		swapChainDesc.OutputWindow = hwnd;

		// Turn multisampling off.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// Set to full screen or windowed mode.
		if (fullscreen)
		{
			swapChainDesc.Windowed = false;
		}
		else
		{
			swapChainDesc.Windowed = true;
		}

		// Set the scan line ordering and scaling to unspecified.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Discard the back buffer contents after presenting.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// Don't set the advanced flags.
		swapChainDesc.Flags = 0;
		After setting up the swap chain description we also need to setup one more variable called the feature level.This variable tells DirectX what version we plan to use.Here we set the feature level to 11.0 which is DirectX 11. You can set this to 10 or 9 to use a lower level version of DirectX if you plan on supporting multiple versions or running on lower end hardware.

			// Set the feature level to DirectX 11.
			featureLevel = D3D_FEATURE_LEVEL_11_0;
		Now that the swap chain descriptionand feature level have been filled out we can create the swap chain, the Direct3D device, and the Direct3D device context.The Direct3D deviceand Direct3D device context are very important, they are the interface to all of the Direct3D functions.We will use the deviceand device context for almost everything from this point forward.

			Those of you reading this who are familiar with the previous versions of DirectX will recognize the Direct3D device but will be unfamiliar with the new Direct3D device context.Basically they took the functionality of the Direct3D deviceand split it up into two different devices so you need to use both now.

			Note that if the user does not have a DirectX 11 video card this function call will fail to create the device and device context.Also if you are testing DirectX 11 functionality yourself and don't have a DirectX 11 video card then you can replace D3D_DRIVER_TYPE_HARDWARE with D3D_DRIVER_TYPE_REFERENCE and DirectX will use your CPU to draw instead of the video card hardware. Note that this runs 1/1000 the speed but it is good for people who don't have DirectX 11 video cards yet on all their machines.

			// Create the swap chain, Direct3D device, and Direct3D device context.
			result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
				D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
		if (FAILED(result))
		{
			return false;
		}
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