#pragma once
#include "ERGBColor.h"
#include "EMath.h"



struct Vertex
{
	Elite::FPoint4 position{};	//float3 position : POSITION;
	Elite::RGBColor color{};	//float3 Color : COLOR;
	Elite::FVector3 normal{};
	Elite::FVector3 tangent{};
	Elite::FVector2 uv{};

};

struct IDTriplet
{
	IDTriplet(uint32_t vector, uint32_t textcoord, uint32_t vecNorm)
		:VectorID{ vector }, TextureID{ textcoord }, NormalID{ vecNorm }{}
	uint32_t VectorID, TextureID, NormalID;
};