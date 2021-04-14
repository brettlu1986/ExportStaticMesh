
#include "LightingUtil.hlsl"

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
	float4 gAimbientLight;
	Light gLight[MaxLights];
}

struct PSInput
{
	float4 position    : SV_POSITION;
	float3 posW		   : POSITION;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
	float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
};

float4 PsMain(PSInput input) : SV_TARGET
{
	input.normal = normalize(input.normal);

	float3 toEyeW = normalize(gEyePosW - input.posW);

	float4 ambient = gAimbientLight * gDiffuseAlbedo;
	const float shininess = 1.0f - gRoughness;
	Material mat = { gDiffuseAlbedo, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLight, mat, input.posW, input.normal, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;
	litColor.a = gDiffuseAlbedo.a;

	return litColor;
}