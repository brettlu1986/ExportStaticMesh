cbuffer BloomDownConstantBuffer : register(b0)
{
	float4 BufferSizeAndInvSize;
	float BloomDownScale;
};

struct VSInput
{
	float3 position    : POSITION;
	float2 uv        : TEXCOORD0;
};

struct PSInput
{
	float4 position    : SV_POSITION;
	float4 uv[8]       : TEXCOORD0;
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

	float Start = 2.0 / 14.0;
	float Scale = BloomDownScale;

	result.uv[0].xy = input.uv.xy;
	result.uv[0].zw = input.uv.xy + Circle(Start, 14.0, 0.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[1].xy = input.uv.xy + Circle(Start, 14.0, 1.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[1].zw = input.uv.xy + Circle(Start, 14.0, 2.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[2].xy = input.uv.xy + Circle(Start, 14.0, 3.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[2].zw = input.uv.xy + Circle(Start, 14.0, 4.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[3].xy = input.uv.xy + Circle(Start, 14.0, 5.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[3].zw = input.uv.xy + Circle(Start, 14.0, 6.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[4].xy = input.uv.xy + Circle(Start, 14.0, 7.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[4].zw = input.uv.xy + Circle(Start, 14.0, 8.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[5].xy = input.uv.xy + Circle(Start, 14.0, 9.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[5].zw = input.uv.xy + Circle(Start, 14.0, 10.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[6].xy = input.uv.xy + Circle(Start, 14.0, 11.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[6].zw = input.uv.xy + Circle(Start, 14.0, 12.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[7].xy = input.uv.xy + Circle(Start, 14.0, 13.0) * Scale * BufferSizeAndInvSize.zw;
	result.uv[7].zw = float2(0.0, 0.0);

	return result;
}