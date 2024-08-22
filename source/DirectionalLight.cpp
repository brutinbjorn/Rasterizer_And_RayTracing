#include "pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const FVector3& direction, const RGBColor& color, const float& intensity)
	:BaseLight(intensity, color)
	, m_Direction{ direction }
{
	Elite::Normalize(m_Direction);
}

DirectionalLight::~DirectionalLight()
{
}

RGBColor DirectionalLight::GetIrradiance() const
{
	RGBColor irradiance{};
	if (m_On)
	{
		irradiance = { m_Color * m_LightIntensity };
	}
	return irradiance;
}

float DirectionalLight::CheckLambertLaw() const
{
	//float m_DotValue = Elite::Dot(hit.hitNormal, -m_Direction);
	return FLT_MAX;
}

FVector3 DirectionalLight::GetDirection() const
{
	return m_Direction;
}

float DirectionalLight::GetDistance() const
{
	return FLT_MAX;
}
