
#include "BloomDownVs.hlsl"

Texture2D BloomDownSourceTexture : register(t0);
SamplerState BloomDownSourceSampler : register(s2);

half4 PsMain(PSInput input) : SV_TARGET
{
	half4 color;
	half4 N0 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[0].xy).rgba;
	half4 N1 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[0].zw).rgba;
	half4 N2 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[1].xy).rgba;
	half4 N3 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[1].zw).rgba;
	half4 N4 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[2].xy).rgba;
	half4 N5 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[2].zw).rgba;
	half4 N6 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[3].xy).rgba;
	half4 N7 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[3].zw).rgba;
	half4 N8 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[4].xy).rgba;
	half4 N9 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[4].zw).rgba;
	half4 N10 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[5].xy).rgba;
	half4 N11 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[5].zw).rgba;
	half4 N12 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[6].xy).rgba;
	half4 N13 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[6].zw).rgba;
	half4 N14 = BloomDownSourceTexture.Sample(BloomDownSourceSampler, input.uv[7].xy).rgba;
	float W = 1.0 / 15.0;
	color.rgb =
		(N0 * W) +
		(N1 * W) +
		(N2 * W) +
		(N3 * W) +
		(N4 * W) +
		(N5 * W) +
		(N6 * W) +
		(N7 * W) +
		(N8 * W) +
		(N9 * W) +
		(N10 * W) +
		(N11 * W) +
		(N12 * W) +
		(N13 * W) +
		(N14 * W);
	color.a = 0;

	return color;
}