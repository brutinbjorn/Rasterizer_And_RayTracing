#pragma once
#include "ERGBColor.h"
#include "EMath.h"
using namespace Elite;
namespace BRDF
{
	RGBColor Lambert(const RGBColor& diffuseColor, const float& reflectance)
	{
		return ((RGBColor(reflectance, reflectance, reflectance) * diffuseColor) / float(M_PI));
	};

	RGBColor Lambert(const RGBColor& diffuseColor, const RGBColor& reflectance)
	{
		return ((reflectance * diffuseColor) / float(M_PI));
	};

	RGBColor LambartDiffuse(const RGBColor& difuseColor, const float Observed,
		const RGBColor Irradiance)// no PHI here yet, test
	{

		RGBColor DiffuseColor = difuseColor;
		return Irradiance * DiffuseColor * Observed;
	};	

	//float3 lambertDiffuse(VS_OUTPUT input)
	//{
	//	float3 DiffuseColor = gDiffuseMap.Sample(samPoint, input.uv).rgb;
	//	float3 lightvector = normalize(float3(0.577f, -0.577f, 0.577f));
	//	float observed = dot(-input.normal, lightvector);
	//	observed = saturate(observed);
	//	return (1.f, 1.f, 1.f) * 1.5f * DiffuseColor * observed;
	//}

	RGBColor Phong(const float& reflection, const FVector3& hitNormal, const FVector3& ViewVector, const FVector3& lightvector, const float exponent)
	{
		return RGBColor();
	}
	RGBColor Phong(const RGBColor& specular, const RGBColor& gloss, const FVector3& hitNormal, const FVector3& ViewVector, const FVector3& lightvector, const float shininess)
	{
		RGBColor SpecSample = specular;
		RGBColor GlossSample = gloss;

		FVector3 reflection = Elite::Reflect(-lightvector, hitNormal);

		float strong = Dot(reflection, ViewVector);
		if (strong < 0.f)
		{
			strong = 0.f;
		}
		if (strong > 1.f)
		{
			strong = 1.f;
		}
		
		RGBColor phongColor = specular * powf(strong, gloss.r * shininess);
		return phongColor;
	}

	//float3 Phong(VS_OUTPUT input, float3 lightvector, float3 viewvector)
	//{
	//	float3 SpecularSample = gSpecularMap.Sample(samPoint, input.uv);
	//	float3 GlossSample = gGlossMap.Sample(samPoint, input.uv);
	//	float shininess = 25.f;
	//	float3 reflection = reflect(-lightvector, input.normal);
	//	float strong = dot(reflection, viewvector);
	//	strong = saturate(strong);
	//	float3 phongColor = SpecularSample * pow(strong, GlossSample.r * shininess);
	//	return phongColor;
	//}

	float NormalDistribution_Trowbirdge(const FVector3& hitNormal, const FVector3& halfVector, const float& roughnessA)
	{
		float a2 = Square(roughnessA);
		float n;
		n = (a2) / (float(M_PI) * (Square(Square(Dot(hitNormal, halfVector)) * (a2 - 1) + 1)));
		return n;
	};

	RGBColor fresnel_schlick(const RGBColor& f0, const FVector3& halfVector, const FVector3& viewVector)
	{

		RGBColor val = f0 + ((RGBColor(1.f, 1.f, 1.f) - f0) * powf((1 - Dot(halfVector, viewVector)), 5.f));

		return val;
	};

	float Geometry_Schlick(const FVector3& hitNormal, const FVector3& viewVector, const  float& roughness)
	{
		float k = Square(Square(roughness) + 1) / 8;
		float g = Dot(hitNormal, viewVector) / (Dot(hitNormal, viewVector) * (1 - k) + k);
		return g;
	};


	//float3 lambert(float3 diffuse, float reflectance)
	//{
	//	return mul(diffuse, reflectance) / PI;
	//}





}