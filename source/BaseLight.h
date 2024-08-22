#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "structs.h"
using namespace Elite;

class BaseLight
{
public:
	BaseLight(const float& Intensity, const RGBColor& color);
	virtual ~BaseLight();

	virtual RGBColor GetIrradiance() const = 0;
	virtual float CheckLambertLaw() const = 0;

	// return vector from light to hitpoint normal P->Q = Q - P
	virtual FVector3 GetDirection() const = 0;

	virtual float GetDistance() const = 0;
	virtual void ToggleLight();
protected:
	float m_LightIntensity;
	RGBColor m_Color;
	bool m_On;



};