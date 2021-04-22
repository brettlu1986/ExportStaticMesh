
#include "LightingUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
};

cbuffer cbPass : register(b2)
{
	float4x4 gViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float4x4 gLightSpaceMatrix;
	float4x4 gShadowMatrix;
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
};

PSInput VsMain(VSInput vin)
{
	PSInput Vout = (PSInput)0.f;

	float4 posW = mul(float4(vin.position, 1.0f), gWorld);
	Vout.position = mul(posW, gLightSpaceMatrix);
	return Vout;
}

