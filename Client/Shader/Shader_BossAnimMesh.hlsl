// Shader_BossAnimMesh.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_NormalTexture : register(t1);
Texture2D<float4> g_UtilityTexture : register(t2);
Texture2D<float4> g_OpacityTexture : register(t3);
Texture2D<float4> g_MetalnessTexture : register(t4);
Texture2D<float4> g_SpecularTexture : register(t5);
Texture2D<float4> g_EmissiveTexture : register(t10);
Texture2D<float4> g_RoughTexture : register(t11);
Texture2D<float4> g_NoiseTexture : register(t12);