
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

float4 PsMain(PSInput input) : SV_TARGET
{
	float AmbientStrength = 0.1;
	float3 DirectionalLightColor = gAimbientLight.xyz;
	float3 AmbientColor = AmbientStrength * DirectionalLightColor;

	float DiffuseStrength = max(dot(input.normal, -gLight[0].Direction), 0.0);
	float3 DiffuseColor = DiffuseStrength * DirectionalLightColor;

	float Shininess = 64.0f;

	float3 ViewDirection = normalize(gEyePosW - input.posW);

	float3 HalfwayDir = normalize(-gLight[0].Direction - ViewDirection);
	float SpecularStrength = pow(max(dot(input.normal, HalfwayDir), 0.0), Shininess);
	float3 SpecularColor = DirectionalLightColor * SpecularStrength;

	float3 color = AmbientColor + DiffuseColor + SpecularColor;
	color = color * 0.8;
	float gamma = 2.2f;
	color = pow(color, 1.0f / gamma);

	return float4(color, 1.f);
}