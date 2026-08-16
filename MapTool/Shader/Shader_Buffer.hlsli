#pragma pack_matrix(row_major)
// Shader_Buffer.hlsli

cbuffer Camera : register(b0)
{
    float4x4 g_ViewMatrix;
    float4x4 g_ProjMatrix;
    float4x4 g_InverseViewMatrix;
    float4x4 g_InverseProjMatrix;
    float4 g_vCamPosition;
};

cbuffer Object : register(b1)
{
    float4x4 g_WorldMatrix;
    float4 g_vColor;
    float g_fProgress;
    uint g_ObjectID;
    uint Object_Padding[2];
};

cbuffer Lights : register(b2)
{
    float g_fLightRange;
    float3 Light_Padding;
    float4 g_vLightDirection;
    float4 g_vLightPosition;
    float4 g_vLightDiffuse;
    float4 g_vLightAmbient;
    float4 g_vLightSpecular;
    float4x4 g_LightViewMatrix;
    float4x4 g_LightProjMatrix;
}

//texture2D g_DiffuseTexture;
cbuffer Material : register(b3)
{
    float4 g_vMtrlAmbient = float4(0.3f, 0.3f, 0.3f, 1.f);
    float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);
}

cbuffer Bones : register(b4)
{
    float4x4 g_BonesMatrices[512];
}

cbuffer MorphBuffer : register(b5)
{
    float4 g_MorphWeightsPacked[8]; // 32개의 weight를 8개의 float4로
}

cbuffer MorphIndices : register(b6)
{
    uint4 g_MorphIndicesPacked[8]; // 32개의 index를 8개의 uint4로
}

cbuffer MorphInfos : register(b7)
{
    uint g_NumActiveMorphs;
    uint g_NumVertices;
    uint2 _padding; // 16바이트 정렬용
}

cbuffer CB_BlobShadow : register(b8)
{
    float g_ShadowRadius;
    float g_ShadowIntensity;
    float g_ShadowSoftness;
    float g_Padding;
}

cbuffer EffectBuffer : register(b9)
{
    float4 g_vEffectColor; // 이펙트 색상
    float2 g_vAtlasSize; // 아틀라스 크기 (열, 행) - 예: (4, 3)
    float2 g_vPadding; // 패딩
};
