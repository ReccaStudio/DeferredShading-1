//--------------------------------------------------------------------------------------
// File: Tutorial08.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D normalTexture : register(t1);
SamplerState samLinear : register(s0);

cbuffer cbChangesEveryFrame : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
};

cbuffer LightBuffer : register(b1)
{
    float3 LightDirection;
    float Padding;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    input.Pos.w = 1.0f;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Proj);
    output.Tex = input.Tex;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    float4 colors;
    float4 normals;
    float3 lightDir;
    float diff;
    float4 outputColor;

    colors = txDiffuse.Sample(samLinear, input.Tex);
    normals = normalTexture.Sample(samLinear, input.Tex);

    lightDir = -LightDirection;
    diff = saturate(dot(normals.xyz, lightDir));
    
    outputColor = saturate(colors * diff);
    return outputColor;
}
