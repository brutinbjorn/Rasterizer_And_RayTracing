#pragma once

#include "BaseEffect.h"
#include "Camera.h"
class Primitive
{
public:
	Primitive();
	virtual ~Primitive();
	virtual void const Render(ID3D11DeviceContext* pDeviceContext, uint32_t width, uint32_t height, 
		bool isRight, const Camera* cam, uint32_t* pBackbufferPixals, std::vector<float>& depthbuffer) const = 0;
	virtual Elite::FMatrix4 getWorldMatrix() const = 0;
	virtual BaseEffect* GetEffect() = 0;
	virtual void AddRotation(float rotation) = 0;
	virtual void Update(uint32_t width, uint32_t height, bool isLeft, 
		Elite::FMatrix4 worldProjectMatrix, Elite::FMatrix4 worldMatrix, Elite::FMatrix4 viewMatrix) = 0;
	virtual void NextCullMode() = 0;

};

