//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

struct VertexIn
{
    float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer cb0 : register(b0)
{
    float4x4 gWorldViewProj;
};

VertexOut VSMain(VertexIn vin)
{
    VertexOut vout;

    vout.PosH = mul(float4(vin.PosL.x, vin.PosL.y, vin.PosL.z, 1.0f), gWorldViewProj);
    vout.Color = vin.Color;

    return vout;
}

float4 PSMain(VertexOut input) : SV_TARGET
{
    return input.Color;
}
