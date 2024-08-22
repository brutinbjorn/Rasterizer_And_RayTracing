#pragma once
#include <vector>
#include <string.h>
#include <string>
#include "BaseEffect.h"

using namespace Elite;
class Effect : public BaseEffect
{
public:

	Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const std::string& diffusePath,
		const std::string& normalPath, const std::string& specular, const std::string& glossPath);

	~Effect();

	virtual const RGBColor PixalShading(const Vertex& v, const FVector3& view)const;
private:
	void setResource();

	Texture* m_pNormalTexture = nullptr;
	Texture* m_pSpecularTexture = nullptr;
	Texture* m_pGlossTexture = nullptr;

	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;

};

