#include "pch.h"
#include "TriangleMesh.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include "EMatrix4.h"
#include "Camera.h"



TriangleMesh::TriangleMesh(const Elite::FPoint3& pos, ID3D11Device* pDevice, const std::string& filepath, 

	BaseEffect* effect)
	:m_Pos(pos)
	, m_pEffect(effect)
{
	m_TRSMatrix = Elite::FMatrix4::Identity();
	m_TRSMatrix *= Elite::MakeTranslation(Elite::FVector3(m_Pos));
	
	std::string line;
	std::ifstream read;

	std::regex getfloat("[-+]?[0-9]*\\.[0-9]*");
	std::regex getint("[0-9]+");
	std::regex StartWithV{"^v\\W.+"};
	std::regex StartWithF{ "^f\\W.+" };
	std::regex StartWithVT{ "^vt\\W.+" };
	std::regex startWithVN{ "^vn\\W.+" };


	read.open(filepath);
	if (read.is_open())
	{
		std::cout << "file found. " << filepath << "\n";
		while ( std::getline(read,line))
		{

			if (std::regex_match(line,StartWithV)) // find vertex coordinates
			{
			
				std::vector<float> values;
				for (std::sregex_iterator i = std::sregex_iterator(line.begin(), line.end(), getfloat);
					i != std::sregex_iterator();
					++i)
				{
					std::smatch m = *i;
					values.push_back(std::stof(m[0].str().c_str()));
				};
				if (values.size() == 3)
					m_Points.push_back(Elite::FPoint3(values[0], values[1], values[2]));				
			}

			if (std::regex_match(line,startWithVN))// find vertex normals
			{
				std::vector<float> values;
				for (std::sregex_iterator i = std::sregex_iterator(line.begin(), line.end(), getfloat);
					i != std::sregex_iterator(); ++i)
				{
					std::smatch m = *i;
					values.push_back(std::stof(m[0].str().c_str()));
				};
				if (values.size() == 3)
					m_Normals.push_back(Elite::GetNormalized(Elite::FVector3(values[0], values[1], values[2])));
			}

			if (std::regex_match(line,StartWithVT)) //find UV coordinates
			{
				std::vector<float> values;
				for (std::sregex_iterator i = std::sregex_iterator(line.begin(), line.end(), getfloat);
					i != std::sregex_iterator();
					++i)
				{
					std::smatch m = *i;
					values.push_back(std::stof(m[0].str().c_str()));
				};
				if (values.size() == 3)
					m_UVPoints.push_back(Elite::FVector2(values[0], 1.f - values[1]));
			}

			if (std::regex_match(line,StartWithF))// find faces  f v/vt/vn v/vt/vn v/vt/vn
			{
				std::vector<int> values;
				for (std::sregex_iterator i = std::sregex_iterator(line.begin(), line.end(), getint);
					i != std::sregex_iterator();
					++i)
				{
					std::smatch m = *i;
					values.push_back(std::stoi(m[0].str().c_str()));
				}
				if (values.size() == 9)
				{
					m_IDTriplets.push_back(IDTriplet(values[0] -1, values[1] -1, values[2] -1));
					m_IDTriplets.push_back(IDTriplet(values[3] -1, values[4] -1, values[5] -1));
					m_IDTriplets.push_back(IDTriplet(values[6] -1, values[7] -1, values[8] -1));

				}
			}

		}
		read.close();

		for (size_t i = 0; i < m_IDTriplets.size(); i++)// go over al ID triplets
		{
			bool IsAlreadyInVertexBuffer{false};
			size_t index{0};
			//loop over vertexbuffer/ if contained break en set true
			for (size_t j = 0; j < m_VertexBuffer.size(); j++) // go over all Vertex IDs
			{
				if (m_IDTriplets[i].NormalID == m_VertexBuffer[j].NormalID &&
					m_IDTriplets[i].TextureID == m_VertexBuffer[j].TextureID &&
					m_IDTriplets[i].VectorID == m_VertexBuffer[j].VectorID) // check if Vertex ID is in IN Triplet
				{
					IsAlreadyInVertexBuffer = true;
					index = j;
					break;
				}
			}

			if (IsAlreadyInVertexBuffer)
			{
				m_Indeces.push_back(index); // if already In vertex buffer, add the ID
			}
			else
			{
				m_VertexBuffer.push_back(m_IDTriplets[i]); // 
				m_Indeces.push_back(m_VertexBuffer.size() - 1);// add new 
			}
		}
		
		for (size_t i = 0; i < m_VertexBuffer.size(); i++)
		{
			IDTriplet currentTrip = m_VertexBuffer[i];
			Vertex ToAdd{};

			ToAdd.position = m_Points[currentTrip.VectorID];
			ToAdd.position.z *= -1.f;
			ToAdd.color = Elite::RGBColor{ 1.f,1.f,1.f };
			ToAdd.uv = m_UVPoints[currentTrip.TextureID];
			ToAdd.normal = m_Normals[currentTrip.NormalID];
			m_Verteces.push_back(ToAdd);
			m_WorldPositions.push_back(ToAdd.position);
			m_screenSpaceVertexes.push_back(ToAdd);
		}


		for (size_t i = 0; i < m_Indeces.size(); i++)
		{
			Vertex &v0 = m_Verteces[m_Indeces[i]];
			i++;
			Vertex &v1 = m_Verteces[m_Indeces[i]];
			i++;
			Vertex &v2 = m_Verteces[m_Indeces[i]];
			//https://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal////////
			const Elite::FPoint3 p0{ v0.position };
			const Elite::FPoint3 p1{ v1.position };
			const Elite::FPoint3 p2{ v2.position };
			const Elite::FVector2& uv0{ v0.uv };
			const Elite::FVector2& uv1{ v1.uv };
			const Elite::FVector2& uv2{ v2.uv };
			const Elite::FVector3 edge0 = p1 - p0;
			const Elite::FVector3 edge1 = p2 - p0;
			const Elite::FVector2 diffX = Elite::FVector2(uv1.x - uv0.x, uv2.x - uv0.x);
			const Elite::FVector2 diffY = Elite::FVector2(uv1.y - uv0.y, uv2.y - uv0.y);
			float r = 1.f / Cross(diffX, diffY);
			Elite::FVector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
			v0.tangent += tangent;
			v1.tangent += tangent;
			v2.tangent += tangent;
			v0.tangent = GetNormalized(Reject(v0.tangent, v0.normal));
			v1.tangent = GetNormalized(Reject(v1.tangent, v1.normal));
			v2.tangent = GetNormalized(Reject(v2.tangent, v2.normal));
			}

			//Create Vertex Layout
		HRESULT result = S_OK;
		static const uint32_t numElements{ 5 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 16;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "NORMAL";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 28;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "TANGENT";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[3].AlignedByteOffset = 40;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[4].SemanticName = "TEXTCOORD";
		vertexDesc[4].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[4].AlignedByteOffset = 52;
		vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


		//Create input Layout
		D3DX11_PASS_DESC passDesc;
		m_pEffect->GetEffectTechinque()->GetPassByIndex(0)->GetDesc(&passDesc);
		result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pVertexLayout);
		if (FAILED(result))
			return;

		// Create Vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * (uint32_t)m_Verteces.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA initData = { 0 };
		initData.pSysMem = m_Verteces.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result))
			return;

		//Create Index buffer
		m_AmountIndices = (uint32_t)m_Indeces.size();
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = m_Indeces.data();
		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);




		D3D11_RASTERIZER_DESC Rasterizer{};
		Rasterizer.CullMode = D3D11_CULL_FRONT;
		Rasterizer.FrontCounterClockwise = false;
		Rasterizer.FillMode = D3D11_FILL_SOLID;
		Rasterizer.DepthBias = 0;
		Rasterizer.SlopeScaledDepthBias = 0.0f;
		Rasterizer.DepthBiasClamp = 0.0f;
		Rasterizer.DepthClipEnable = true;
		Rasterizer.ScissorEnable = false;
		Rasterizer.MultisampleEnable = false;
		Rasterizer.AntialiasedLineEnable = false;


		result = pDevice->CreateRasterizerState(&Rasterizer,&m_pRasterizerFront);
		if (FAILED(result))
		{
			std::cout << "front failed\n";
			return;
		}

		D3D11_RASTERIZER_DESC RasterizerBack = Rasterizer;
		RasterizerBack.CullMode = D3D11_CULL_BACK;
		RasterizerBack.FrontCounterClockwise = false;

		result = pDevice->CreateRasterizerState(&RasterizerBack, &m_pRasterizerback);
		if (FAILED(result))
		{
			std::cout << "back failed\n";
			return;
		}
			

		D3D11_RASTERIZER_DESC RasterizerNone = Rasterizer;
		RasterizerNone.CullMode = D3D11_CULL_NONE;
		RasterizerNone.FrontCounterClockwise = false;

		result = pDevice->CreateRasterizerState(&RasterizerNone, &m_pRasterizerNone);
		if (FAILED(result))
		{
			std::cout << "none failed\n";
			return;
		}

	}
	else
	{
		std::cout << "file not found.\n";
	}

}

TriangleMesh::~TriangleMesh()
{
	delete m_pEffect;


	if (m_pVertexLayout)
		m_pVertexLayout->Release();

	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();

	if (m_pRasterizerFront)
		m_pRasterizerFront->Release();

	if (m_pRasterizerback)
		m_pRasterizerback->Release();

	if (m_pRasterizerNone)
		m_pRasterizerNone->Release();

	
}

void TriangleMesh::Update(uint32_t width, uint32_t height, bool isRight,
	Elite::FMatrix4 worldProjectMatrix, Elite::FMatrix4 worldMatrix, Elite::FMatrix4 viewMatrix)
{
	m_pEffect->update(worldProjectMatrix, worldMatrix, viewMatrix);
	if (isRight)
	{
		SetScreenSpaceVerteces(width, height, worldProjectMatrix);
	}
}

void const TriangleMesh::Render(ID3D11DeviceContext* pDeviceContext, uint32_t width, uint32_t height,
	bool isRight,const Camera* cam, uint32_t* pBackbufferPixals, std::vector<float>& depthbuffer) const
{
	if (!isRight)
	{
		//set vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		// set index buffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the input layout
		pDeviceContext->IASetInputLayout(m_pVertexLayout);

		//Set primitive topolgy
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//set rasterizerState
		switch (m_cullmode)
		{
		case 0:
			pDeviceContext->RSSetState(m_pRasterizerFront);
			break;
		case 1:
			pDeviceContext->RSSetState(m_pRasterizerback);
			break;
		case 2:
			pDeviceContext->RSSetState(m_pRasterizerNone);
			break;

		}
		
		//render shape
		D3DX11_TECHNIQUE_DESC techDesc;
		m_pEffect->GetEffectTechinque()->GetDesc(&techDesc);
		
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetEffectTechinque()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
		};
	}
	else
	{
		Elite::IPoint2 MinVal;
		Elite::IPoint2 MaxVal;
		Vertex output;
		FPoint3 position = cam->GetPosition();
		FPoint2 screenSpace;
		FVector3 direction; // set ray direction

		for (uint32_t i = 0; i < m_Indeces.size(); i += 3)
		{
			GetBoundingBox(i, MinVal, MaxVal, width, height);
			for (int r = MinVal.y; r < MaxVal.y; ++r) // raster space vertical
			{
				if (MaxVal.y > int(height) || MinVal.y > int(height))
				{
					std::cout << "to large" << std::endl;
				}

				for (int c = MinVal.x; c < MaxVal.x; ++c) // raster space horizontal
				{
					if (MaxVal.x > int(width) || MinVal.x > int(width))
					{
						std::cout << "to large" << std::endl;
					}

					direction = GetNormalized(cam->GetForward());

					if (Hit(i,Elite::FPoint2(c, r), depthbuffer[c + uint64_t(r * height)], output, position))
					{ 
						direction = Elite::GetNormalized(output.position.xyz - cam->GetPosition());
						output.color = m_pEffect->PixalShading(output,direction);
						output.color.Clamp();

						pBackbufferPixals[c + (r * width)] = Elite::GetSDL_ARGBColor(output.color);
					}
				}
			}
		}
	}

}


Elite::FMatrix4 TriangleMesh::getWorldMatrix() const
{
	return m_TRSMatrix;
}

void TriangleMesh::AddRotation(float rotation)
{
	m_rotation += rotation;
	Elite::FMatrix4 rotationMatrix = Elite::FMatrix4(Elite::MakeRotationY(m_rotation));
	Elite::FMatrix4 toPos = Elite::FMatrix4(Elite::MakeTranslation(Elite::FVector3(m_Pos)));
	m_TRSMatrix = toPos * rotationMatrix;
}

BaseEffect* TriangleMesh::GetEffect()
{
	return m_pEffect;
}

void TriangleMesh::NextCullMode()
{
	m_cullmode++;
	m_cullmode = m_cullmode %3; 

	switch (m_cullmode)
	{
	case 0:
		std::cout << "cullmode front \n";
		break;
	case 1:
		std::cout << "cullmode back \n";
		break;
	case 2:
		std::cout << "cullmode Both \n";
		break;

	}
}

const bool TriangleMesh::Hit(const uint32_t i, const Elite::FPoint2& pixelPos, float& depthValue, Vertex& v, const FPoint3& position) const
{
	int ID0 = i;
	int ID1 = i + 1;
	int ID2 = i + 2;
	const Vertex& v0 = m_screenSpaceVertexes[m_Indeces[ID0]];
	const Vertex& v1 = m_screenSpaceVertexes[m_Indeces[ID1]];
	const Vertex& v2 = m_screenSpaceVertexes[m_Indeces[ID2]];



	Elite::FVector2 EdgeA = (v1.position.xy - v0.position.xy);
	Elite::FVector2 EdgeB = (v0.position.xy - v2.position.xy);
	Elite::FVector2 EdgeC = (v2.position.xy - v1.position.xy);

	Elite::FVector2 pToV0 = pixelPos - v0.position.xy;
	if (Elite::Cross(-pToV0, EdgeA) > 0.f)
		return false;

	Elite::FVector2 pToV1 = pixelPos - v1.position.xy;
	if (Elite::Cross(-pToV1, EdgeC) > 0.f)
		return false;

	Elite::FVector2 pToV2 = pixelPos - v2.position.xy;
	if (Elite::Cross(-pToV2, EdgeB) > 0.f)
		return false;

	float totalwight = Cross(EdgeA, -EdgeB);
	float w0 = ((Elite::Cross(-pToV1, EdgeC)) / totalwight);

	//totalwight = Cross(m_EdgeC, -m_EdgeA);
	float w1 = ((Elite::Cross(-pToV2, EdgeB)) / totalwight);

	//totalwight = Cross(m_EdgeB, -m_EdgeC);
	float w2 = ((Elite::Cross(-pToV0, EdgeA)) / totalwight);


	float ZdepthBuffer{
		1.f / (
			(1.f / v0.position.z) * w0 +
			(1.f / v1.position.z) * w1 +
			(1.f / v2.position.z) * w2) };


	if (depthValue <= ZdepthBuffer || ZdepthBuffer > 1.f)
	{
		return false;
	}

	float WInterpolated
	{ 1.f /
		(
		(1.f / v0.position.w) * w0 +
		(1.f / v1.position.w) * w1 +
		(1.f / v2.position.w) * w2
		)
	};

	FVector3 NormalInterPolated
	{
		(
			FVector3(v0.normal) / v0.position.w * w0 +
			FVector3(v1.normal) / v1.position.w * w1 +
			FVector3(v2.normal) / v2.position.w * w2
		)
		* WInterpolated
	};
	Elite::Normalize(NormalInterPolated);




	FPoint3 PointInterpolated = FPoint3
	{
		(
			FVector3(m_WorldPositions[m_Indeces[ID0]].xyz) / v0.position.w * w0 +
			FVector3(m_WorldPositions[m_Indeces[ID1]].xyz) / v1.position.w * w1 +
			FVector3(m_WorldPositions[m_Indeces[ID2]].xyz) / v2.position.w * w2
		)
		* WInterpolated
	};


	FVector3 m_EdgeA = m_WorldPositions[m_Indeces[ID1]].xyz - m_WorldPositions[m_Indeces[ID0]].xyz;
	FVector3 m_EdgeB = m_WorldPositions[m_Indeces[ID2]].xyz - m_WorldPositions[m_Indeces[ID0]].xyz;

	FVector3 Normal = GetNormalized(Cross(m_EdgeA, -m_EdgeB));
	FVector3 camRay = GetNormalized(position - PointInterpolated);


	switch (m_cullmode)
	{
	case 0:
		if (Dot(Normal, camRay) <= 0.f)
		{
			return false;
		}
		break;
	case 1:
		if (Dot(Normal, camRay) >= 0.f)
		{
			return false;
		}
		break;
	case 2:
		if (Elite::AreEqual(Elite::Dot(Normal, camRay), 0.f))
		{
			return false;
		}
		break;
	default:
		break;
	}

	FVector2 UVInterpol{
		(
		(v0.uv / v0.position.w) * w0 +
		(v1.uv / v1.position.w) * w1 +
		(v2.uv / v2.position.w) * w2
		) * WInterpolated
	};


	RGBColor colorInterpol{
		(
		(v0.color / v0.position.w) * w0 +
		(v1.color / v1.position.w) * w1 +
		(v2.color / v2.position.w) * w2
		)
		* WInterpolated

	};
	colorInterpol.MaxToOne();


	FVector3 TangentInterpol
	{
		(
			FVector3(v0.tangent) / v0.position.w * w0 +
			FVector3(v1.tangent) / v1.position.w * w1 +
			FVector3(v2.tangent) / v2.position.w * w2
		)
		* WInterpolated
	};
	Elite::Normalize(TangentInterpol);

	depthValue = ZdepthBuffer;

	v.color = colorInterpol;
	v.uv = UVInterpol;
	v.normal = NormalInterPolated;
	v.position = PointInterpolated;
	v.tangent = TangentInterpol;

	return true;
}

const void TriangleMesh::GetBoundingBox(int IdTrip, IPoint2& min, IPoint2& max,
	const uint32_t witdh,const uint32_t height) const 
{

	min = m_screenSpaceVertexes[0].position.xy;
	max = m_screenSpaceVertexes[0].position.xy;


	for (int i = 0; i < 3; i++)
	{
		FPoint2 val = m_screenSpaceVertexes[m_Indeces[(uint32_t)IdTrip + uint32_t(i)]].position.xy;
		if (min.x > val.x)
		{
			min.x = val.x - 1;
		}
		if (min.y > val.y)
		{
			min.y = val.y - 1;
		}
		if (max.x < val.x)
		{
			max.x = val.x + 1;
		}
		if (max.y < val.y)
		{
			max.y = val.y + 1;
		}
	}
	if (min.x < 0)
	{
		min.x = 0;
	}
	if (min.y < 0)
	{
		min.y = 0;
	}
	if (max.x > witdh - 1)
	{
		max.x = witdh - 1;
	}
	if (max.y > height - 1)
	{
		max.y = height - 1;
	}
}

void TriangleMesh::SetScreenSpaceVerteces(uint32_t witdh, uint32_t height, Elite::FMatrix4& worldViewmatrix)
{
	Elite::FPoint4 temp;
	for (size_t i = 0; i < m_Verteces.size(); i++)
	{
		temp = { m_Verteces[i].position };
		m_WorldPositions[i] = m_TRSMatrix * temp;

		Elite::FPoint4 viewSpaceVertex = temp;
		Elite::FPoint4 projectedPos = worldViewmatrix * viewSpaceVertex;
		projectedPos.x /= projectedPos.w;
		projectedPos.y /= projectedPos.w;
		projectedPos.z /= projectedPos.w;
		projectedPos;

		m_screenSpaceVertexes[i].position.x = ((projectedPos.x + 1) / 2) * witdh;
		m_screenSpaceVertexes[i].position.y = ((1 - projectedPos.y) / 2) * height;
		m_screenSpaceVertexes[i].position.z = projectedPos.z;
		m_screenSpaceVertexes[i].position.w = projectedPos.w;

		Elite::FVector4 ToTrans = Elite::FVector4(m_Verteces[i].normal);
		m_screenSpaceVertexes[i].normal = Elite::GetNormalized((Inverse(Transpose(m_TRSMatrix)) * ToTrans).xyz);

		Elite::FVector4 ToTangent = Elite::FVector4(m_Verteces[i].tangent);
		m_screenSpaceVertexes[i].tangent = Elite::GetNormalized((Inverse(Transpose(m_TRSMatrix)) * ToTangent).xyz);
	}
	//CalculateInternValues();
}






