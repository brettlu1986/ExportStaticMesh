cbuffer BloomUpConstantBuffer : register(b0)
{
	float4 BufferASizeAndInvSize;
	float4 BufferBSizeAndInvSize;
	float4 BloomTintA;
	float4 BloomTintB;
	float2 BloomUpScales;
};

struct VSInput
{
	float3 position    : POSITION;
	float2 uv        : TEXCOORD0;
};

struct PSInput
{
	float4 position    : SV_POSITION;
	float4 uv[8]        : TEXCOORD0;
};

float2 Circle(float Start, float Points, float Point)
{
	float Rad = (3.141592 * 2.0 * (1.0 / Points)) * (Point + Start);
	return float2(sin(Rad), cos(Rad));
}

PSInput VsMain(VSInput input)
{

	PSInput result;

	result.position = float4(input.position, 1.0f);

	float Start;
	float Scale;

	Start = 2.0 / 7.0;
	Scale = BloomUpScales.x;


	result.uv[0].xy = input.uv.xy + Circle(Start, 7.0, 0.0) * Scale * BufferASizeAndInvSize.zw;
	result.uv[0].zw = input.uv.xy + Circle(Start, 7.0, 1.0) * Scale * BufferASizeAndInvSize.zw;
	result.uv[1].xy = input.uv.xy + Circle(Start, 7.0, 2.0) * Scale * BufferASizeAndInvSize.zw;
	result.uv[1].zw = input.uv.xy + Circle(Start, 7.0, 3.0) * Scale * BufferASizeAndInvSize.zw;
	result.uv[2].xy = input.uv.xy + Circle(Start, 7.0, 4.0) * Scale * BufferASizeAndInvSize.zw;
	result.uv[2].zw = input.uv.xy + Circle(Start, 7.0, 5.0) * Scale * BufferASizeAndInvSize.zw;
	result.uv[3].xy = input.uv.xy + Circle(Start, 7.0, 6.0) * Scale * BufferASizeAndInvSize.zw;

	result.uv[3].zw = input.uv.xy;

	Start = 2.0 / 7.0;
	Scale = BloomUpScales.y;

	result.uv[4].xy = input.uv.xy + Circle(Start, 7.0, 0.0) * Scale * BufferBSizeAndInvSize.zw;
	result.uv[4].zw = input.uv.xy + Circle(Start, 7.0, 1.0) * Scale * BufferBSizeAndInvSize.zw;
	result.uv[5].xy = input.uv.xy + Circle(Start, 7.0, 2.0) * Scale * BufferBSizeAndInvSize.zw;
	result.uv[5].zw = input.uv.xy + Circle(Start, 7.0, 3.0) * Scale * BufferBSizeAndInvSize.zw;
	result.uv[6].xy = input.uv.xy + Circle(Start, 7.0, 4.0) * Scale * BufferBSizeAndInvSize.zw;
	result.uv[6].zw = input.uv.xy + Circle(Start, 7.0, 5.0) * Scale * BufferBSizeAndInvSize.zw;
	result.uv[7].xy = input.uv.xy + Circle(Start, 7.0, 6.0) * Scale * BufferBSizeAndInvSize.zw;
	result.uv[7].zw = float2(0.0, 0.0);

	return result;
}