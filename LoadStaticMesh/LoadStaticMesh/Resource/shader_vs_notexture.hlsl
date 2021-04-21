


#include "LightingUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
};

cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float gRoughness;
	float4x4 gMatTransform;
}

cbuffer cbPass : register(b2)
{
	float4x4 gViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float4x4 gLightSpaceMatrix;
	float4 gAimbientLight;
	Light gLight[MaxLights];
}

struct VSInput
{
	float3 position    : POSITION;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
	float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
};

struct PSInput
{
	float4 position    : SV_POSITION;
	float3 posW		   : POSITION;
	float4 positionLightSpace :POSITION1;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
	float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
};

PSInput VSMain(VSInput vin)
{
	PSInput vout = (PSInput)0.0f;

	float4 posW = mul(float4(vin.position, 1.0f), gWorld);
	vout.posW = posW.xyz;

	vout.normal = normalize((float3)(mul(vin.normal, gWorld)));
	vout.position = mul(posW, gViewProj);

	vout.positionLightSpace = mul(posW, gLightSpaceMatrix);

	vout.tangent = vin.tangent;
	vout.uv0 = vin.uv0;
	vout.uv1 = vin.uv1;
	vout.color = vin.color;

	return vout;
}