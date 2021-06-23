
#include "LightingUtil.hlsl"
#include "ShaderDef.hlsli"

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

