
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
	float4x4 gLightSpaceMatrix;
	float4 gAimbientLight;
	Light gLight[MaxLights];
}

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

Texture2D gShadowMap : register(t1);
SamplerState g_sampler : register(s0);

float ShadowCalculation(float4 positionLightSpace)
{
	//// Complete projection by doing division by w.
	//positionLightSpace.xyz /= positionLightSpace.w;
	//// Depth in NDC space.
	//float depth = positionLightSpace.z;

	//uint width, height, numMips;
	//gShadowMap.GetDimensions(0, width, height, numMips);

	//// Texel size.
	//float dx = 1.0f / (float)width;

	//float percentLit = 0.0f;
	//const float2 offsets[9] =
	//{
	//	float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
	//	float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
	//	float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	//};

	//[unroll]
	//for (int i = 0; i < 9; ++i)
	//{
	//	percentLit += gShadowMap.SampleCmpLevelZero(g_sampler,
	//		positionLightSpace.xy + offsets[i], depth).r;
	//}

	//return percentLit / 9.0f;
	float3 ProjCoords = positionLightSpace.xyz / positionLightSpace.w;
	ProjCoords = ProjCoords * 0.5 + 0.5;
	float ClosestDepth = gShadowMap.Sample(g_sampler, ProjCoords.xy).r;
	float CurrentDepth = ProjCoords.z;
	float Shadow = CurrentDepth > ClosestDepth ? 1.0 : 0.0;
	return 0;
}

float4 PsMain(PSInput input) : SV_TARGET
{
	float AmbientStrength = 0.2;
	float3 DirectionalLightColor = gAimbientLight.xyz;
	float3 AmbientColor = AmbientStrength * DirectionalLightColor;

	float3 LightDirection = -gLight[0].Direction;
	float DiffuseStrength = max(dot(input.normal, LightDirection), 0.0);
	float3 DiffuseColor = DiffuseStrength * DirectionalLightColor;

	float Shininess = 64.0f;
	float3 ViewDirection = normalize(gEyePosW - input.posW);
	float3 HalfwayDir = normalize(LightDirection + ViewDirection);
	float SpecularStrength = pow(max(dot(input.normal, HalfwayDir), 0.0), Shininess);
	float3 SpecularColor = DirectionalLightColor * SpecularStrength;

	float Shadow = ShadowCalculation(input.positionLightSpace);
	float3 color = ( AmbientColor + (1 - Shadow) * (DiffuseColor + SpecularColor) ) * input.color;
	color = color * 0.8;
	float gamma = 2.2f;
	color = pow(color, 1.0f / gamma);

	return float4(color, 1.f);
}