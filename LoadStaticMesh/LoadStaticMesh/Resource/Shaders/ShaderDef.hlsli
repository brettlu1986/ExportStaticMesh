
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
	float gAlpha;
	float3 Padding;
	float4 gColorBlendAdd;
}

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

struct VSInput
{
	float3 position    : POSITION;
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
