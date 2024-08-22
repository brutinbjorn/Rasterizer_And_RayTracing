float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
float4x4 gViewInverse : VIEWINVERSE;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gGlossMap : GlossMap;
Texture2D gSpecularMap : SpecularMap;
//RasterizerState gRasterizerState : GRasterizerState;




static const float PI = 3.14159265f;
//------------------------
// Input/Output Structs
//-------------------------
struct VS_INPUT
{
	float4 position : POSITION;
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
//--------------
//sampler state
//--------------
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

//RasterizerState gRasterizerState
//{
//    CullMode = front;
//    FrontCounterClockwise = false;
//};
//RasterizerState gRasterizerStateBack
//{
//    CullMode = back;
//    FrontCounterClockwise = false;
//};
//RasterizerState gRasterizerStateNone
//{
//    CullMode = none;
//    FrontCounterClockwise = false;
//};



//-------------------
// Vertex Shader
//-------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
    output.worldPosition = mul(input.position,gWorldMatrix);

	output.position = float4(input.position);
	output.position = mul(output.position, gWorldViewProj);
	output.uv = input.uv;
    output.normal = normalize(mul(input.normal, (float3x3) gWorldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3) gWorldMatrix));
	
	return output;
}
//-------------------
// Shading
//-------------------
float3 lambert(float3 diffuse, float reflectance)
{
    return mul(diffuse, reflectance) / PI;
}

float3 Phong(VS_OUTPUT input,float3 lightvector, float3 viewvector,SamplerState state)
{
    float3 SpecularSample = gSpecularMap.Sample(state, input.uv);
    float3 GlossSample = gGlossMap.Sample(state, input.uv);
	
	float shininess = 25.f;
    float3 reflection = reflect(-lightvector, input.normal);
	
    float strong = dot(reflection, viewvector);
    strong = saturate(strong);
	

    float3 phongColor = SpecularSample * pow(strong, GlossSample.r * shininess);
    return phongColor;

}

float3 lambertDiffuse(VS_OUTPUT input,SamplerState state)
{
    float3 DiffuseColor = gDiffuseMap.Sample(state, input.uv).rgb;
	
    float3 lightvector = normalize(float3(0.577f, -0.577f, 0.577f));
    float observed = dot(-input.normal, lightvector);
    observed = saturate(observed);
	
    return (1.f, 1.f, 1.f) * 1.5f * DiffuseColor * observed;
	
}

float3 DoShading(VS_OUTPUT input, SamplerState state)
{
	float3 lightvector = normalize(float3(0.577f, -0.577f, 0.577f));
	
	float3 binormal = cross(input.normal,input.tangent);
	float3x3 tangentSpaceAxi = float3x3(input.tangent, binormal, input.normal);

    float3 SampleVal = gNormalMap.Sample(state, input.uv).xyz;
    float3 SampleNormal = (2 * SampleVal - float3(1.f, 1.f, 1.f));
	
	 float3 newNormal = normalize(mul(SampleNormal,tangentSpaceAxi));
    input.normal = newNormal;
   
	
    float3 viewdirection = normalize(input.worldPosition.xyz - gViewInverse[3].xyz);
    float3 lambertColor = lambertDiffuse(input,state);
    float phongcolor = Phong(input,lightvector,viewdirection,state);
	
    float3 color = lambertColor + phongcolor;
	
    color = saturate(color);   
    return color;
}

//--------------------------------
// pixal shader
//---------------------------------

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    //float3 viewDirection = normalize(input.worldPosition - gViewInverse[3].xyz);
    input.normal.z = -input.normal.z;
    input.tangent.z = -input.tangent.z;
    
    float3 color = DoShading(input,samPoint);
	return float4(color,1.f);
}

float4 LinearS(VS_OUTPUT input) : SV_TARGET
{
    input.normal.z = -input.normal.z;
    input.tangent.z = -input.tangent.z;
    float3 color = DoShading(input,samLinear);
    return float4(color, 1.f);
}

float4 AnisotropicS(VS_OUTPUT input) : SV_TARGET
{
    input.normal.z = -input.normal.z;
    input.tangent.z = -input.tangent.z;
    float3 color = DoShading(input,samAnisotropic);
    return float4(color, 1.f);
}


//------------------------
// Technique
//----------------------------
technique11 DefaultTechnique
{
	pass P0
	{

        SetDepthStencilState(NULL, 0);
	//	SetRasterizerState(NULL);
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader( ps_5_0, PS() ) );
	}
}
technique11 LinearTechnique
{
	pass P0
	{

        SetDepthStencilState(NULL, 0);
	//	SetRasterizerState(NULL);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, LinearS()));
	}
}

technique11 AnisotropicTechnique
{
	pass P0
	{

        SetDepthStencilState(NULL, 0);
	//	SetRasterizerState(NULL);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, AnisotropicS()));
	}
}
