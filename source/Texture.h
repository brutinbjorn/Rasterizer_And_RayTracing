#pragma once

#include <SDL_image.h>
#include <d3dx11effect.h>
class Texture
{
public:
	Texture() = default;
	Texture(ID3D11Device* pDevice,const std::string& filePath);
	~Texture();
	ID3D11ShaderResourceView* GetShader();
	Elite::RGBColor Sample(const Elite::FVector2& uv) const;

private:
	SDL_Surface* m_pSurface = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
	ID3D11Texture2D* m_pTexture = nullptr;
};