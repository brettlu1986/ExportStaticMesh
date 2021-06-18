#include "BloomUpVs.hlsl"

Texture2D BloomUpSourceATexture: register(t0);
SamplerState BloomUpSourceASampler: register(s3);

Texture2D BloomUpSourceBTexture: register(t1);
SamplerState BloomUpSourceBSampler: register(s4);

half4 PsMain(PSInput input) : SV_TARGET
{
	half4 color;

	half3 A0 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[0].xy).rgb;
	half3 A1 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[0].zw).rgb;
	half3 A2 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[1].xy).rgb;
	half3 A3 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[1].zw).rgb;
	half3 A4 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[2].xy).rgb;
	half3 A5 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[2].zw).rgb;
	half3 A6 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[3].xy).rgb;
	half3 A7 = BloomUpSourceATexture.Sample(BloomUpSourceASampler, input.uv[3].zw).rgb;

	half3 B0 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[3].zw).rgb;
	half3 B1 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[4].xy).rgb;
	half3 B2 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[4].zw).rgb;
	half3 B3 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[5].xy).rgb;
	half3 B4 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[5].zw).rgb;
	half3 B5 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[6].xy).rgb;
	half3 B6 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[6].zw).rgb;
	half3 B7 = BloomUpSourceBTexture.Sample(BloomUpSourceBSampler, input.uv[7].xy).rgb;


	half3 WA = BloomTintA.rgb;

	half3 WB = BloomTintB.rgb;

	color.rgb =
		A0 * WA +
		A1 * WA +
		A2 * WA +
		A3 * WA +
		A4 * WA +
		A5 * WA +
		A6 * WA +
		A7 * WA +
		B0 * WB +
		B1 * WB +
		B2 * WB +
		B3 * WB +
		B4 * WB +
		B5 * WB +
		B6 * WB +
		B7 * WB;
	color.a = 0;

	return color;
}