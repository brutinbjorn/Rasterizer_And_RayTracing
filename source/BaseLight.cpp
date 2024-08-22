#include "pch.h"
#include "BaseLight.h"

BaseLight::BaseLight(const float& Intensity, const RGBColor& color)
	:m_Color(color)
	,m_LightIntensity(Intensity)
{
	m_On = true;
}

BaseLight::~BaseLight()
{
}

void BaseLight::ToggleLight()
{
	m_On = !m_On;
}
