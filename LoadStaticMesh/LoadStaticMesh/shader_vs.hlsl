
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VSInput
{
    float3 position    : POSITION;
    float2 uv        : TEXCOORD0;
};

struct PSInput
{
	float4 position    : SV_POSITION;
	float2 uv        : TEXCOORD0;
};

PSInput VSMain(VSInput vin)
{
	PSInput vout;

	vout.position = mul(float4(vin.position, 1.0f), gWorldViewProj);

	vout.uv = vin.uv;

	return vout;
}