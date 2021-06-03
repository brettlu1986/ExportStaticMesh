
Texture2D       g_txDiffuse : register(t0);
SamplerState    g_sampler : register(s1);

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

float4 PsMain(PSInput input) : SV_TARGET
{
	//return g_txDiffuse.Sample(g_sampler, input.uv0);
	float3 color = input.normal * 0.5 + 0.5;
	color = color * 0.8;
	float gamma = 2.2f;
	color = pow(color, 1.0f / gamma);

	return float4(color, 1.f);
}