// Shader_Font.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D g_FontTex : register(t0);
float4 PS_MAIN(float4 pos : SV_Position,
               float4 color : COLOR,
               float2 uv : TEXCOORD) : SV_Target
{
    float2 texelSize = 1.0 / g_TextureSize;
    float4 center = g_FontTex.Sample(LinearSampler, uv);
    
    // ¿Ü°û¼± ·ÎÁ÷
    float up = g_FontTex.Sample(LinearSampler, uv + float2(0, -texelSize.y * g_OutLineThickness)).a;
    float down = g_FontTex.Sample(LinearSampler, uv + float2(0, texelSize.y * g_OutLineThickness)).a;
    float left = g_FontTex.Sample(LinearSampler, uv + float2(-texelSize.x * g_OutLineThickness, 0)).a;
    float right = g_FontTex.Sample(LinearSampler, uv + float2(texelSize.x * g_OutLineThickness, 0)).a;

    float outlineAlpha = max(max(up, down), max(left, right));
    outlineAlpha = saturate(outlineAlpha - center.a);
    
    float3 finalRGB = lerp(g_OutLineColor.rgb, color.rgb, center.a);
    float finalA = max(center.a, outlineAlpha);

    return float4(finalRGB, finalA);
}