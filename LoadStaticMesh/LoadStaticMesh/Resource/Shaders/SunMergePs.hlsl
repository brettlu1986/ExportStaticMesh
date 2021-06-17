#include "SunMergeVs.hlsl"

Texture2D BloomSetup_BloomTexture: register(t0);
SamplerState BloomSetup_BloomSampler: register(s0);

Texture2D BloomUpTexture: register(t1);
SamplerState BloomUpSampler: register(s1);

half4 PsMain(PSInput input) : SV_TARGET
{
	half4 color;

	float Scale1 = 1.0 / 7.0;
	float Scale2 = 1.0 / 7.0;

	half3 Bloom2 = (
		BloomSetup_BloomTexture.Sample(BloomSetup_BloomSampler, input.TexCoordVignette.xy).rgba * Scale1 +
		BloomSetup_BloomTexture.Sample(BloomSetup_BloomSampler, input.TexCoords[0].xy).rgba * Scale2 +
		BloomSetup_BloomTexture.Sample(BloomSetup_BloomSampler, input.TexCoords[1].xy).rgba * Scale2 +
		BloomSetup_BloomTexture.Sample(BloomSetup_BloomSampler, input.TexCoords[2].xy).rgba * Scale2 +
		BloomSetup_BloomTexture.Sample(BloomSetup_BloomSampler, input.TexCoords[3].xy).rgba * Scale2 +
		BloomSetup_BloomTexture.Sample(BloomSetup_BloomSampler, input.TexCoords[4].xy).rgba * Scale2 +
		BloomSetup_BloomTexture.Sample(BloomSetup_BloomSampler, input.TexCoords[5].xy).rgba * Scale2) * rcp(Scale1 * 1.0 + Scale2 * 6.0);

	color.rgb = BloomUpTexture.Sample(BloomUpSampler, input.TexCoordVignette.xy);

	half Scale3 = 1.0 / 5.0;
	color.rgb *= Scale3;

	color.rgb += Bloom2 * Scale3 * BloomColor;
	color.a = 1.0f;

	return color;
}