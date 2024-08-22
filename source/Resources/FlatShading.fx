float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
float4x4 gViewInverse : VIEWINVERSE;
Texture2D gDiffuseMap : DiffuseMap;
static const float PI = 3.14159265f;

//------------------------
// Input/Output Structs
//-------------------------
struct VS_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXTCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPosition : COLOR;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXTCOORD;
};

//samplerstates

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Clamp;
	AddressV = Clamp;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};


// blendState
BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = Add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0f;
};

//depthStencilState
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;
	
    StencilReadMask = 0x0F;
    StencilWriteMask = 0x0F;
	
    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;

    FrontfaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;

    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};

// rasterizerstate
RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};
//-------------------
// Vertex Shader
//-------------------


VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
    output.worldPosition = mul(input.position,gWorldMatrix);
	output.position = float4(input.position, 1.f);
	output.position = mul(output.position, gWorldViewProj);
	output.uv = input.uv;
    output.normal = normalize(mul(input.normal, (float3x3) gWorldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3) gWorldMatrix));
	return output;
}

float4 DoShading(VS_OUTPUT input,SamplerState state)
{
	
    float4 Color = gDiffuseMap.Sample(state, input.uv);
    float4 SourceColor = saturate(Color);
	return SourceColor;
}

//--------------------------------
// pixal shader
//---------------------------------

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = DoShading(input,samPoint);   
    //float3 SourceColor = (1.f, 1.f, 1.f);
	//SourceColor = ((color.rgb * color.a) + (input.worldPosition.rgb * 1.f));
    color = float4((color.rgb * color.a),color.a);
    return (color);
}

float4 LinearS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = DoShading(input,samLinear);
	
    color = float4((color.rgb * color.a), color.a);
    return float4(color);
}

float4 AnisotropicS(VS_OUTPUT input) : SV_TARGET
{
    float4 color = DoShading(input,samAnisotropic);
	
    color = float4((color.rgb * color.a), color.a);
    return float4(color);
}


//------------------------
// Technique
//----------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader( ps_5_0, PS() ) );
	}
}

technique11 LinearTechnique
{
	pass P0
	{
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, LinearS()));
	}
}

technique11 AnisotropicTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, AnisotropicS()));
	}
}
