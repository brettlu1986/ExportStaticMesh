
#include "LightingUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbSkinned : register(b1)
{
	float4x4 gBoneTransforms[80];
};

cbuffer cbPass2 : register(b3)
{
	float4x4 gLightSpaceMatrix;
	float4x4 gShadowMatrix;
	Light gLight[MaxLights];
}

struct PSInput
{
	float4 position    : SV_POSITION;
};

struct SkmVSInput
{
	float3 position    : POSITION;
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

PSInput VsMain(SkmVSInput vin)
{
	PSInput Vout = (PSInput)0.f;

	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.BoneWeights.x;
	weights[1] = vin.BoneWeights.y;
	weights[2] = vin.BoneWeights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
	float3 posL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		posL += weights[i] * mul(float4(vin.position, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
	}
	vin.position = posL;

	float4 posW = mul(float4(vin.position, 1.0f), gWorld);
	Vout.position = mul(posW, gLightSpaceMatrix);
	return Vout;
}

