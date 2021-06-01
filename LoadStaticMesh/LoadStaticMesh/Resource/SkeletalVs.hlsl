#include "LightingUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
};

cbuffer cbSkinned : register(b1)
{
	float4x4 gBoneTransforms[80];
};

cbuffer cbPass1 : register(b2)
{
	float4x4 gViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
}

cbuffer cbPass2 : register(b3)
{
	float4x4 gLightSpaceMatrix;
	float4x4 gShadowMatrix;
	Light gLight[MaxLights];
}

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

struct SkmVSInput
{
	float3 position    : POSITION;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
	float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

PSInput VsMain(SkmVSInput vin)
{
	PSInput vout = (PSInput)0.0f;

	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.BoneWeights.x;
	weights[1] = vin.BoneWeights.y;
	weights[2] = vin.BoneWeights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		// Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.

		posL += weights[i] * mul(float4(vin.position, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
		normalL += weights[i] * mul(vin.normal, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
		tangentL += weights[i] * mul(vin.tangent.xyz, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
	}
	vin.position = posL;
	vin.normal = normalL;
	vin.tangent = tangentL;

	float4 posW = mul(float4(vin.position, 1.0f), gWorld);
	vout.posW = posW.xyz;

	vout.normal = mul(vin.normal, (float3x3)gWorld);
	vout.tangent = mul(vin.tangent, (float3x3)gWorld);
	vout.position = mul(posW, gViewProj);

	vout.shadowPosH = mul(posW, gShadowMatrix);
	vout.uv0 = vin.uv0;
	vout.uv1 = vin.uv1;
	vout.color = vin.color;

	return vout;
}