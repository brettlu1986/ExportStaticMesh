

struct PSInput
{
	float4 position    : SV_POSITION;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
	float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
};

float4 PsMain(PSInput input) : SV_TARGET
{
	float3 color = input.normal * 0.5f + 0.5f;
	return float4(color, 1.0f);
}