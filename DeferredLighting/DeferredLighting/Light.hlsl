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
Texture2D depthTexture : register(t2);
SamplerState samLinear : register(s0);

cbuffer cbChangesEveryFrame : register(b0)
{
    matrix World;
    matrix View;
    matrix Proj;
};

cbuffer LightBuffer : register(b1)
{
    float3 LightPos;
    float4 LightColor;

    float Constant;
    float Linear;
    float Quadratic;

    float3 Ambient;
    float3 Diffuse;
    float3 Specular;
};

cbuffer FrustumBuffer : register(b2)
{
    float3 ViewPos;
    float NearZ;
    float FarZ;
    float3 Vx;
    float3 Vy;
    float3 Look;
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
    output.Pos = input.Pos;
   
    output.Tex = input.Tex;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    float ratio;
    float4 colors;
    float4 normals;
    float3 lightDir;
    float4 outputColor;
    float3 worldPos;
    float3 viewDir;
    float3 halfDir;
    float diff;
    float spec;
    float ambient;
    float attenuation;

    colors = txDiffuse.Sample(samLinear, input.Tex);
    normals = normalTexture.Sample(samLinear, input.Tex);
    ratio = 1 / (FarZ - NearZ) * (1 - NearZ / normals.w);
    viewDir = (input.Pos.x * Vx + input.Pos.y * Vy + Look) * ratio;
    worldPos = viewDir + ViewPos;
    
    lightDir = normalize(LightPos - worldPos);
    diff = max(dot(normals.xyz, lightDir),0.0);
    
    halfDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(halfDir, normals.xyz), 0.0), 10);

    ambient = Ambient;
    
    //colors += (ambient + spec + diff) * LightColor;

    outputColor = saturate(colors);
    return outputColor;
}
