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

cbuffer FrustumBuffer : register(b1)
{
    float3 ViewPos;
    float NearZ;
    float FarZ;
    float3 Vx;
    float3 Vy;
    float Aspect;
    float tanFov;
    float3 Look;

};

struct LightPos
{
    float3 LightPos;
    float Lpad1;
    float4 LightColor;

    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float Constant;
    float Linear;
    float Quadratic;

    float Lpad2;
};


cbuffer LightBuffer : register(b2)
{
    LightPos lights[1024];
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
float4 CaculateLights(float3 worldPos, float3 viewDir,float4 normals,LightPos light)
{
    float4 color;
    float3 lightDir;
    float3 halfDir;
    float diff;
    float spec;
    float attenuation;
    float distance;
    float4 colors;

    lightDir = normalize(light.LightPos - worldPos);
    diff = max(dot(normals.xyz, lightDir), 0.0);
    
    halfDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(halfDir, normals.xyz), 0.0), 10);

    distance = length(viewDir);
    attenuation = 1.0f / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    colors = (light.Ambient + spec * light.Specular + diff * light.Diffuse) * light.LightColor * attenuation;
    colors.w = 1.0f;
    return colors;
}

float4 PS(PS_INPUT input) : SV_Target
{
    float ratio;
    float4 colors;
    float4 normals;
    float4 outputColor;
    float3 worldPos;
    float3 viewDir;
    float lightColors;

    colors = txDiffuse.Sample(samLinear, input.Tex);
    normals = normalTexture.Sample(samLinear, input.Tex);
    if (normals.x == 0.f && normals.y == 0.f
       && normals.z == 0.f && normals.w == 1.0f)
    {
        return saturate(colors);
    }

    //ratio = 1 / (FarZ - NearZ) * (1 - NearZ / normals.w);
    ratio = NearZ / (FarZ - (FarZ - NearZ) * normals.w);
    viewDir = (input.Pos.x * Vx * Aspect + input.Pos.y * Vy) * ratio + Look * ratio;
    worldPos = viewDir + ViewPos;
    
    for (int i = 0; i < 1024;i++)
        lightColors += CaculateLights(worldPos,viewDir,normals, lights[i]);
   
    colors *= lightColors;

    outputColor = saturate(colors);
    return outputColor;
}
