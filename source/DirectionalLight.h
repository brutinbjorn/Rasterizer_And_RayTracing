#pragma once
#include "BaseLight.h"
class DirectionalLight :
    public BaseLight
{
public:
	DirectionalLight(const FVector3& direction, const RGBColor& color, const float& intensity);
	virtual ~DirectionalLight();
	virtual RGBColor GetIrradiance() const override;
	virtual float CheckLambertLaw() const override;
	virtual FVector3 GetDirection() const override;
	virtual float GetDistance() const override;
private:
	FVector3 m_Direction;
};

