
#include "BloomSetupVs.hlsl"

Texture2D SceneColorTexture : register(t0);
SamplerState SceneColorSampler : register(s2);

half  Luminance(half3  LinearColor)
{
	return dot(LinearColor, half3 (0.3, 0.59, 0.11));
}

half4 PsMain(PSInput input) : SV_TARGET
{
	half4 C0 = SceneColorTexture.Sample(SceneColorSampler, input.uv[0]);
	half4 C1 = SceneColorTexture.Sample(SceneColorSampler, input.uv[1]);
	half4 C2 = SceneColorTexture.Sample(SceneColorSampler, input.uv[2]);
	half4 C3 = SceneColorTexture.Sample(SceneColorSampler, input.uv[3]);

	C0.rgb = C0;
	C1.rgb = C1;
	C2.rgb = C2;
	C3.rgb = C3;

	half3 AverageColor = 0.0f;
	AverageColor.rgb = (C0.rgb * 0.25) + (C1.rgb * 0.25) + (C2.rgb * 0.25) + (C3.rgb * 0.25);

	half TotalLuminance = Luminance(AverageColor.rgb);
	half BloomLuminance = TotalLuminance - BloomThreshold;
	half Amount = saturate(BloomLuminance * 0.5f);

	half4 color;
	color.rgb = AverageColor * Amount;
	color.a = 0.0f;
	return color;
}