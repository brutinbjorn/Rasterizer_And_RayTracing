#include "pch.h"
#include "Texture.h"

#include <iostream>



Texture::Texture(ID3D11Device* pDevice, const std::string& filePath)
	:m_pSurface(IMG_Load(filePath.c_str()))
{

	if (m_pSurface == nullptr)
	{
		std::cout << "could not load image" << std::endl;
	}
	else
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = m_pSurface->w;
		desc.Height = m_pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = m_pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData,&m_pTexture);
		if (FAILED(hr))
			std::cout << "texture error\n";

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pShaderResourceView);
		if (FAILED(hr))
			std::cout << "texture error\n";

	}

}

Texture::~Texture()
{
	if (m_pShaderResourceView)
		m_pShaderResourceView->Release();

	if (m_pTexture)
		m_pTexture->Release();
	SDL_FreeSurface(m_pSurface);
	
}

ID3D11ShaderResourceView* Texture::GetShader()
{
	return m_pShaderResourceView;
}


Elite::RGBColor Texture::Sample(const Elite::FVector2& uv) const
{
	if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
	{
		return Elite::RGBColor();
	}
	if (!m_pSurface)
	{
		return Elite::RGBColor();
	}

	int totalwidth = m_pSurface->w;
	int totalheight = m_pSurface->h;
	uint32_t xPos = uv.x * totalwidth;
	uint32_t yPos = uv.y * totalheight;
	uint32_t pos = { yPos * totalwidth + xPos };


	uint32_t* pixels = (Uint32*)m_pSurface->pixels;

	uint32_t pixal = pixels[yPos * m_pSurface->w + xPos];


	SDL_Color rgb{};

	SDL_GetRGB(pixal, m_pSurface->format, &rgb.r, &rgb.g, &rgb.b);


	Elite::RGBColor color
	{
		float(uint8_t(rgb.r)) / 255.f,
		float(uint8_t(rgb.g)) / 255.f,
		float(uint8_t(rgb.b)) / 255.f
	};

	color.Clamp();
	return color;
}
