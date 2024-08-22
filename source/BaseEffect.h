#pragma once
#include <vector>
#include "Texture.h"
#include "ERGBColor.h"
#include "Structs.h"
#include "EMath.h"

using namespace Elite;
class BaseEffect
{
public:

	BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile, const std::string& diffusePath);
	virtual ~BaseEffect();
	void update(Elite::FMatrix4& worldViewMatrix, Elite::FMatrix4& worldMatrix, Elite::FMatrix4& ViewMatrix);
	void NextTechnique();
	virtual ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetEffectTechinque();
	virtual const RGBColor PixalShading(const Vertex& v, const FVector3& view) const;
protected:
	void setResource();
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	ID3DX11Effect* m_pEffect;
	std::vector<ID3DX11EffectTechnique*> m_pTechniques;

	Texture* m_pDiffuseTexture;

	int m_currentTechnique{0};
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pMatWorldPositionVariable;
	ID3DX11EffectMatrixVariable* m_pViewInverseVariable;

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;

};

