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
	float3 gLightPos;
	float cbPerObjectPad2;
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

Texture2D       g_txDiffuse : register(t0);
SamplerState    g_sampler : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
	float4 diffuseAlbedo = g_txDiffuse.Sample(g_sampler, input.uv0) * gDiffuseAlbedo;
	
	input.normal = normalize(input.normal);

	float3 toEyeW = normalize(gEyePosW - input.posW);

	float4 ambient = gAimbientLight * diffuseAlbedo;
	const float shininess = 1.0f - gRoughness;
	Material mat = { diffuseAlbedo, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLight, mat, input.posW, input.normal, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;
	litColor.a = diffuseAlbedo.a;

    return litColor;
}