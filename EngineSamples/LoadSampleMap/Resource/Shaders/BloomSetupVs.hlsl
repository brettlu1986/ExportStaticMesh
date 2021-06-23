

cbuffer BloomSetupConstantBuffer : register(b0)
{
    float4 BufferSizeAndInvSize;
    float  BloomThreshold;
};

struct VSInput
{
    float3 position    : POSITION;
    float2 uv        : TEXCOORD0;
};

struct PSInput
{
    float4 position    : SV_POSITION;
    float2 uv[4]       : TEXCOORD0;
};

PSInput VsMain(VSInput input)
{
    PSInput result;

    result.position = float4(input.position, 1.0f);
    result.uv[0] = input.uv + BufferSizeAndInvSize.zw * float2(-1, -1);
    result.uv[1] = input.uv + BufferSizeAndInvSize.zw * float2(1, -1);
    result.uv[2] = input.uv + BufferSizeAndInvSize.zw * float2(-1, 1);
    result.uv[3] = input.uv + BufferSizeAndInvSize.zw * float2(1, 1);

    return result;
}

