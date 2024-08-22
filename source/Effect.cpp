#include "pch.h"
#include "Effect.h"
#include <sstream>
#include "LightManager.h"
#include "BRDF.h"


Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const std::string& diffusePath,
	const std::string& normalPath, const std::string& specularPath, const std::string& glossPath)
	:BaseEffect(pDevice,assetFile,diffusePath)
	, m_pNormalTexture{ new Texture(pDevice, normalPath) }
	,m_pSpecularTexture { new Texture(pDevice, specularPath)}
	, m_pGlossTexture{ new Texture(pDevice, glossPath) }
{
	m_pGlossMapVariable = m_pEffect->GetVariableByName("gGlossMap")->AsShaderResource();
	if (!m_pGlossMapVariable->IsValid())
		std::wcout << L"Variable gGlossMap not found\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"Variable gSpecularMap not found\n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"Variable gSpecularMap not found\n";

	
	setResource();
}

Effect::~Effect()
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->Release();
	
	if (m_pGlossMapVariable)
		m_pGlossMapVariable->Release();

	if (m_pSpecularMapVariable)
		m_pSpecularMapVariable->Release();

	if (m_pNormalMapVariable)
		m_pNormalMapVariable->Release();

	if (m_pMatWorldViewProjVariable)
		m_pMatWorldViewProjVariable->Release();

	if (m_pMatWorldPositionVariable)
		m_pMatWorldPositionVariable->Release();

	if (m_pViewInverseVariable)
		m_pViewInverseVariable->Release();

	delete m_pGlossTexture;
	delete m_pNormalTexture;
	delete m_pSpecularTexture;

}


const RGBColor Effect::PixalShading(const Vertex& v, const FVector3& view) const
{
	RGBColor DiffuseSample = m_pDiffuseTexture->Sample(v.uv);
	RGBColor normalSample = m_pNormalTexture->Sample(v.uv);
	RGBColor glossSample = m_pGlossTexture->Sample(v.uv);
	RGBColor specSample = m_pSpecularTexture->Sample(v.uv);

	FVector3 binormal = Cross(v.normal, v.tangent);
	FMatrix3 tangentSpaceAxis = FMatrix3(v.tangent, binormal, v.normal);

	FVector3 normalValue = FVector3(normalSample.r, normalSample.g, normalSample.b);
	FVector3 sampleValue = GetNormalized((2.f * normalValue) - FVector3(1.f, 1.f, 1.f));

	FVector3 newNormal = GetNormalized(tangentSpaceAxis * sampleValue);

	LightManager* m_lights = LightManager::GetInstance();
	Elite::RGBColor color;// = v.color;

	for (size_t i = 0; i < m_lights->GetLightCount(); i++)
	{
		FVector3 lightnorm = m_lights->GetLight(i)->GetDirection();
		float ObservedArea = Dot(-newNormal, lightnorm);

		if (0.f < ObservedArea)
		{
			Elite::Clamp(ObservedArea, 0.f, 1.f);
			RGBColor Irradiance = m_lights->GetLight(i)->GetIrradiance();

			color = BRDF::LambartDiffuse(DiffuseSample, ObservedArea, Irradiance) +
				BRDF::Phong(specSample, glossSample, newNormal, view, lightnorm, 25.f);
		}
	}

	return color;
}


void Effect::setResource()
{
	if (m_pDiffuseMapVariable->IsValid())
		m_pDiffuseMapVariable->SetResource(m_pDiffuseTexture->GetShader());

	if (m_pGlossMapVariable->IsValid())
		m_pGlossMapVariable->SetResource(m_pGlossTexture->GetShader());

	if (m_pSpecularMapVariable->IsValid())
		m_pSpecularMapVariable->SetResource(m_pSpecularTexture->GetShader());

	if (m_pNormalMapVariable->IsValid())
		m_pNormalMapVariable->SetResource(m_pNormalTexture->GetShader());
}



