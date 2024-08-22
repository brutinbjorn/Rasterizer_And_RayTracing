#include "pch.h"
#include "BaseEffect.h"
#include <sstream>



//BaseEffect::BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
//{
//}

BaseEffect::BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile, const std::string& diffusePath)
	//://BaseEffect(pDevice, assetFile)
{
	m_pDiffuseTexture = new Texture(pDevice, diffusePath);

	m_pEffect = LoadEffect(pDevice, assetFile);
	m_pMatWorldPositionVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldPositionVariable->IsValid())
		std::wcout << L"m_pMatWorldPostionVariable not valid\n";

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid\n";

	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pViewInverseVariable)
		std::wcout << L"m_pViewInverse not valid";

	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"Variable gDiffuseMap not found\n";

	
	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("DefaultTechnique"));
	if (!m_pTechniques.back()->IsValid())
		std::wcout << L"Technique not valid\n";

	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("LinearTechnique"));
	if (!m_pTechniques.back()->IsValid())
		std::wcout << L"Technique not valid\n";

	m_pTechniques.push_back(m_pEffect->GetTechniqueByName("AnisotropicTechnique"));
	if (!m_pTechniques.back()->IsValid())
		std::wcout << L"Technique not valid\n";

	setResource();
}



BaseEffect::~BaseEffect()
{

	if (m_pMatWorldViewProjVariable)
		m_pMatWorldViewProjVariable->Release();

	for (size_t i = 0; i < m_pTechniques.size(); i++)
	{
		if (m_pTechniques[i])
			m_pTechniques[i]->Release();
	}

	if (m_pEffect)
		m_pEffect->Release();

	delete m_pDiffuseTexture;
}

void BaseEffect::update(
	Elite::FMatrix4& worldViewMatrix,
	Elite::FMatrix4& worldMatrix,
	Elite::FMatrix4& viewMatrix)
{
	HRESULT result = S_OK;
	result = m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<float*>(&worldViewMatrix));
	if (FAILED(result))
		std::cout << "WorldViewMatrix error\n";

	result = m_pMatWorldPositionVariable->SetMatrix(reinterpret_cast<float*>(&worldMatrix));
	if (FAILED(result))
		std::cout << "worldMatrix Error\n";

	result = m_pViewInverseVariable->SetMatrix(reinterpret_cast<float*>(&viewMatrix));
	if (FAILED(result))
		std::cout << "viewMatrix Error\n";
}
//
//void BaseEffect::setResource(ID3D11ShaderResourceView* pResourceDiffuse)
//{
//
//	if (m_pDiffuseMapVariable->IsValid())
//		m_pDiffuseMapVariable->SetResource(pResourceDiffuse);
//}

void BaseEffect::setResource()
{
	if (m_pDiffuseMapVariable->IsValid())
		m_pDiffuseMapVariable->SetResource(m_pDiffuseTexture->GetShader());
}

void BaseEffect::NextTechnique()
{
	m_currentTechnique++;
	if (m_currentTechnique >= m_pTechniques.size())
	{
		m_currentTechnique = 0;
	}
	std::cout << "current technique: " << m_currentTechnique <<std::endl;
}

ID3DX11Effect* BaseEffect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectTechnique* BaseEffect::GetEffectTechinque()
{
	return m_pTechniques[m_currentTechnique];
}

const RGBColor BaseEffect::PixalShading(const Vertex& v, const FVector3& view) const
{
	RGBColor temp = m_pDiffuseTexture->Sample(v.uv);
	return temp;
}



ID3DX11Effect* BaseEffect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined (_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: failed to create effect from file! \npath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}
	return pEffect;
}
