
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VSInput
{
    float3 position    : POSITION;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
    float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
};

struct PSInput
{
	float4 position    : SV_POSITION;
	float3 normal     : NORMAL;
	float3 tangent   : TANGENT;
	float2 uv0        : TEXCOORD0;
	float2 uv1        : TEXCOORD1;
	float4 color     : COLOR;
};

PSInput VSMain(VSInput vin)
{
	PSInput vout;

	vout.position = mul(float4(vin.position, 1.0f), gWorldViewProj);

	vout.normal = vin.normal;
	vout.tangent = vin.tangent;
	vout.uv0 = vin.uv0;
	vout.uv1 = vin.uv1;
	vout.color = vin.color;

	return vout;
}