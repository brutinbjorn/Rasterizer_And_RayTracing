#include "pch.h"

#include "ERenderer.h"
#include "Effect.h"
#include <ostream>

Elite::Renderer::Renderer(SDL_Window * pWindow, SceneGraph* scene,  Camera* pCamera)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
	, m_IsInitialized{ false }
	, m_pCamera{pCamera}
	, m_pScene{scene}
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);

	//Initialize DirectX pipeline
	//...
	if (InitializeDirectX() == 0)
		m_IsInitialized = true;
	if (m_IsInitialized)
	{
		std::cout << "DirectX is ready\n" << std::endl;
	}


	// depthBuffer
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
	for (size_t i = 0; i < uint64_t(width) * height; i++)
	{
		m_depthBuffer.push_back(FLT_MAX);
	}


	Effect* PhongEffect = new Effect(m_pDevice, L"Resources/PosCol3D.fx",
		"Resources/vehicle_diffuse.png", "Resources/vehicle_normal.png",
		"Resources/vehicle_specular.png", "Resources/vehicle_gloss.png");

	TriangleMesh* VehicleMesh = (new TriangleMesh(
		FPoint3(0.f,0.f,0.f),m_pDevice,
		"Resources/vehicle.obj",
		PhongEffect));

	BaseEffect* baseEffect = new BaseEffect(m_pDevice, L"Resources/FlatShading.fx",
		"Resources/fireFX_diffuse.png");

	TriangleMesh* FlameMesh = (new TriangleMesh(
		FPoint3(0.f, 0.f, 0.f), m_pDevice,
		"Resources/fireFX.obj",baseEffect
	));


	m_pScene->AddObjectToGraph(VehicleMesh);	
	m_pScene->AddObjectToGraph(FlameMesh);
}

Elite::Renderer::~Renderer()
{

	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();
	

	if (m_pRenderTargetBuffer)
		m_pRenderTargetBuffer->Release();
	

	if (m_DepthStencilView)
		m_DepthStencilView->Release();

	if (m_pDepthStencilBuffer)
		m_pDepthStencilBuffer->Release();
	
	if (m_pSwapChain)
		m_pSwapChain->Release();

	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}
	if (m_pDevice)
		m_pDevice->Release();

}

void Elite::Renderer::Render(bool isRight)
{
	if (!m_IsInitialized) 
		return;

	//Clear Buffers
	//directX
	if (!isRight)
	{
		RGBColor clearColor = RGBColor(0.f, 0.f, 0.3f);
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	if (isRight)
	{
		for (uint32_t i = 0; i < uint32_t(m_Height * m_Width); i++) // reset pixals and depthbuffer;
		{
			m_depthBuffer[i] = FLT_MAX;
			m_pBackBufferPixels[i] = Elite::GetSDL_ARGBColor({ 0.2f,0.5f,0.2f });
		}
	}

	// VertexTransformation
	FMatrix4 viewMatrix = m_pCamera->getViewMatrix(isRight);

	if (!isRight)
	{
		for (size_t i = 0; i < m_pScene->GetShapeCount(); i++)
		{	
			FMatrix4 worldMatrix = m_pScene->GetShape(i)->getWorldMatrix();
			FMatrix4 worldViewProjectionMatrix = m_pCamera->getProjectionMatrix(isRight) * Inverse(viewMatrix) * worldMatrix;
			//update
			//..
			m_pScene->GetShape(i)->Update( m_Width, m_Height,isRight,worldViewProjectionMatrix, worldMatrix, viewMatrix);
			//render
			//...
			m_pScene->GetShape(i)->Render(m_pDeviceContext, m_Width, m_Height, isRight, m_pCamera, m_pBackBufferPixels, m_depthBuffer);
		}
	}
	else
	{
		FMatrix4 worldMatrix = m_pScene->GetShape(0)->getWorldMatrix();
		FMatrix4 worldViewProjectionMatrix = m_pCamera->getProjectionMatrix(isRight) * Inverse(viewMatrix) * worldMatrix;
		//update
		//..
		m_pScene->GetShape(0)->Update(m_Width, m_Height, isRight, worldViewProjectionMatrix, worldMatrix, viewMatrix);
		//render
		//...
		m_pScene->GetShape(0)->Render(m_pDeviceContext, m_Width, m_Height, isRight, m_pCamera, m_pBackBufferPixels, m_depthBuffer);
	}

	if (!isRight)
		m_pSwapChain->Present(0, 0);

	if (isRight)
	{
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);
	}

}

HRESULT Elite::Renderer::InitializeDirectX()
{

	//create Device and Device Context, using hardware accelation
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	uint32_t createDeviceFlag = 0;

#if defined(DEBUG) || defined (_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(0,D3D_DRIVER_TYPE_HARDWARE,0,createDeviceFlag,0,0,D3D11_SDK_VERSION, &m_pDevice,&featureLevel, &m_pDeviceContext);
	if (FAILED(result))
		return result;

	// Create DXGI Factory to create SwapChain based on hardware
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
	if (FAILED(result))
		return result;

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//Get the handle HWND from the SDLBuffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	//Create SwapChain and hook it into the Handle of the SDL Window
	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result))
		return result;

	//Create the Depth/Stencil buffer and View
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result))
		return result;

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
	if (FAILED(result))
		return result;
	

	//Create RenderTargetView
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result))
		return result;
	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result))
		return result;

	//bind the views to the output merger stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_DepthStencilView);

	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);


	return result;
}


