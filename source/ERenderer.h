/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>

//directXheaders
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include "TriangleMesh.h"
#include "Camera.h"
#include "Texture.h"
#include "SceneGraph.h"


struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow, SceneGraph* scene ,Camera* pCamera);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(bool isRight);

	private:

		//universal
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;
		Camera* m_pCamera;
		SceneGraph* m_pScene;

		//DirectX
		HRESULT InitializeDirectX();
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_DepthStencilView;
		ID3D11Buffer* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11RasterizerState* m_pRasterizerFront;

		bool m_IsInitialized;


		// rasterizer
		std::vector<float> m_depthBuffer{};
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
	};
}

#endif