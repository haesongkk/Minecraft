Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txOpacity : register(t4);

SamplerState samLinear : register(s0);

cbuffer TransformBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer LightBuffer : register(b1)
{
    float3 LightDirection;
    float DL_pad0;
    float4 LightAmbient;
    float4 LightDiffuse;
    float4 LightSpecular;
    float3 EyePosition;
    float DL_pad1;
}

cbuffer MarterialBuffer : register(b2)
{
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
    float MaterialSpecularPower;
    float3 Materialpad0;
}

cbuffer UsingBuffer : register(b3)
{
    bool UsingDiffuseMap;
    bool UsingNormalMap;
    bool UsingSpecularMap;
    bool UsingEmissiveMap;
    bool UsingOpacityMap;
}

cbuffer BoneBuffer : register(b4)
{
    matrix BoneMatrixPalletes[128];
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
    float3 Norm : NORMAL;
    float3 TangentModel : TANGENT;
    int4 BoneIndicies : BLENDINDICES;
    float4 BoneWeights : BLENDWEIGHTS;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Norm : NORMAL;
    float3 TangentWorld : TANGENT;
};
