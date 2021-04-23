#include "LightingUtil.hlsl"
#include "ShaderDef.hlsli"


Texture2D       g_txDiffuse : register(t0);
Texture2D gShadowMap : register(t1);

SamplerState    g_sampler : register(s0);
SamplerComparisonState gsamShadow :register(s1);

struct PSInput
{
	float4 position    : SV_POSITION;
	float3 posW		   : POSITION;
	float4 shadowPosH :POSITION1;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
	float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
};

float ShadowCalculation(float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;
	// Depth in NDC space.
	float depth = shadowPosH.z;
	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);
	// Texel size.
	float dx = 1.0f / (float)width;
	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};
	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}
	return percentLit / 9.0f;

	//shadowPosH.xyz /= shadowPosH.w;
	//// Depth in NDC space.
	//float currentDepth = shadowPosH.z;
	//uint width, height, numMips;
	//gShadowMap.GetDimensions(0, width, height, numMips);
	//float2 PiexlPos = shadowPosH.xy * width;
	//float depthInMap = gShadowMap.Load(int3(PiexlPos, 0)).r;
	//depthInMap += 0.01;
	//return currentDepth > depthInMap ? 1.0 : 0;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float4 diffuseAlbedo = g_txDiffuse.Sample(g_sampler, input.uv0) * gDiffuseAlbedo;
	
	input.normal = normalize(input.normal);

	float3 toEyeW = normalize(gEyePosW - input.posW);

	float4 ambient = float4(gLight[0].Strength, 1.0) * diffuseAlbedo;
	const float shininess = 1.0f - gRoughness;
	Material mat = { diffuseAlbedo, gFresnelR0, shininess };
	float3 shadowFactor =  ShadowCalculation(input.shadowPosH);
	float4 directLight = ComputeLighting(gLight, mat, input.posW, input.normal, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;
	litColor.a = diffuseAlbedo.a;

    return litColor;
}