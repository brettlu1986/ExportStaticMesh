
cbuffer BloomUpConstantBuffer : register(b0)
{
    float4 BloomUpSizeAndInvSize;
    float3 BloomColor;
};

struct VSInput
{
    float3 position    : POSITION;
    float2 uv        : TEXCOORD0;
};

struct PSInput
{
    float4 position    : SV_POSITION;
    float4 TexCoordVignette : TEXCOORD0;
    float4 TexCoords[6]        : TEXCOORD1;
};

float2 Circle(float Start, float Points, float Point)
{
    float Rad = (3.141592 * 2.0 * (1.0 / Points)) * (Point + Start);
    return float2(sin(Rad), cos(Rad));
}

PSInput VSMain(VSInput input)
{
    PSInput result = (PSInput)0;
    result.position = float4(input.position, 1.0f);

    result.TexCoordVignette.xy = input.uv.xy;

    float Start;
    float Scale;

    Start = 2.0 / 6.0;
    Scale = 0.66 / 2.0;

    result.TexCoords[0].xy = input.uv.xy + Circle(Start, 6.0, 0.0) * Scale * BloomUpSizeAndInvSize.zw;
    result.TexCoords[1].xy = input.uv.xy + Circle(Start, 6.0, 1.0) * Scale * BloomUpSizeAndInvSize.zw;
    result.TexCoords[2].xy = input.uv.xy + Circle(Start, 6.0, 2.0) * Scale * BloomUpSizeAndInvSize.zw;
    result.TexCoords[3].xy = input.uv.xy + Circle(Start, 6.0, 3.0) * Scale * BloomUpSizeAndInvSize.zw;
    result.TexCoords[4].xy = input.uv.xy + Circle(Start, 6.0, 4.0) * Scale * BloomUpSizeAndInvSize.zw;
    result.TexCoords[5].xy = input.uv.xy + Circle(Start, 6.0, 5.0) * Scale * BloomUpSizeAndInvSize.zw;

    return result;
}