


#include "LightingUtil.hlsl"
#include "ShaderDef.hlsli"

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

PSInput VSMain(VSInput vin)
{
	PSInput vout = (PSInput)0.0f;
	
	//transfrom to world space
	float4 posW = mul(float4(vin.position, 1.0f), gWorld);
	vout.posW = posW.xyz;
	
	vout.normal = mul(vin.normal, (float3x3)gWorld);
	vout.position = mul(posW, gViewProj);

	vout.shadowPosH = mul(posW, gShadowMatrix);
	vout.uv0 = vin.uv0; 

	vout.tangent = vin.tangent;
	vout.uv1 = vin.uv1;
	vout.color = vin.color;

	return vout;
}