#pragma once
#include <vector>
#include "Texture.h"
#include "Effect.h"
#include "Primitive.h"
#include "Structs.h"

using namespace Elite;

class TriangleMesh final : public Primitive
{
public:
	//constructor
	TriangleMesh(const Elite::FPoint3& pos, ID3D11Device* pDevice,
	const std::string& filepath,

	BaseEffect* effect);
	//destructor
	virtual ~TriangleMesh();

	virtual void Update(uint32_t width, uint32_t height, bool isleft,
		Elite::FMatrix4 worldProjectMatrix, Elite::FMatrix4 worldMatrix, Elite::FMatrix4 viewMatrix) override;
	virtual void const Render(ID3D11DeviceContext* pDeviceContext, uint32_t width, uint32_t height,
		bool isRight, const Camera* cam, uint32_t* pBackbufferPixals, std::vector<float>& depthbuffer) const override;
	virtual Elite::FMatrix4 getWorldMatrix() const override;
	virtual void AddRotation(float rotation) override;
	virtual BaseEffect* GetEffect() override;
	virtual void NextCullMode() override;

private:
	const bool Hit(const uint32_t i,const Elite::FPoint2& pixelPos, float& depthValue, Vertex& v, const FPoint3& position) const;
	const void GetBoundingBox(int IdTrip, IPoint2& min, IPoint2& max,const uint32_t witdh,const uint32_t height) const;
	void SetScreenSpaceVerteces(uint32_t witdh, uint32_t height, Elite::FMatrix4& matrix);



	Elite::FPoint3 m_Pos;
	Elite::FMatrix4 m_TRSMatrix;
	float m_rotation;

	//File data
	std::vector<Elite::FPoint3> m_Points{};
	std::vector<Elite::FVector2> m_UVPoints{};
	std::vector<Elite::FVector3> m_Normals{};
	std::vector<IDTriplet> m_IDTriplets;
	std::vector<IDTriplet> m_VertexBuffer;
	int m_cullmode{ 0 };


	//DirectX data
	BaseEffect* m_pEffect;
	std::vector<uint32_t> m_Indeces;
	std::vector<Vertex> m_Verteces;
	uint32_t m_AmountIndices;

	//Pointers
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11RasterizerState* m_pRasterizerFront = nullptr;
	ID3D11RasterizerState* m_pRasterizerback = nullptr;
	ID3D11RasterizerState* m_pRasterizerNone = nullptr;

	//Rasterizer data
	std::vector<Elite::FPoint4> m_WorldPositions;
	std::vector<Vertex> m_screenSpaceVertexes;


	float m_Rotation{};

};

