#pragma pack_matrix(row_major)
#include "Shader_Buffer.hlsli"

Texture2D g_DebugTexture : register(t0);

Texture2D g_TextureDiffuse : register(t1);
Texture2D g_TextureNormal : register(t2);
Texture2D g_TextureShade : register(t3);
Texture2D g_TextureDepth : register(t4);
Texture2D g_TextureSpecular : register(t5);
Texture2D g_TextureShadow : register(t6);

SamplerState LinearSampler : register(s0);

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
   
    float4x4 matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
   
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
   
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BACKBUFFER PS_DEBUG_MAIN(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    Out.vColor = In.vPosition;
    Out.vColor = g_DebugTexture.Sample(LinearSampler, In.vTexcoord);
   
    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

    float4 vNormalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);

    float fSpecularStrength = vNormalDesc.w;
    
    /* 0~1 -> -1~1 */
    float4 vNormal = float4((vNormalDesc.xyz * 2.f - 1.f), 0.f);
    vNormal = normalize(vNormal);
    float AmbientStrength = vDepthDesc.z;
    
    float4 vShade = max(dot(normalize(g_vLightDirection) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient * AmbientStrength);

    Out.vShade = (g_vLightDiffuse * saturate(vShade));

    float4 vReflect = reflect(normalize(g_vLightDirection), normalize(vNormal));

    float fViewZ = vDepthDesc.y;

    float4 vWorldPos;

    // WorldPos구하기
    // 우선 NDC좌표 구하기
    // x, y를 TEXCOORD(0 ~ 1)에서 뽑아서 NDC좌표(-1, 1 ~ 1, -1)로 변환
    // z는 깊이 텍스쳐에서 뽑아서 넣어주기
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    // Z나누기 했던걸 복구
    // Loacl * World * View * Proj / w  => Loacl * World * View * Proj
    vWorldPos *= fViewZ;

    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);

    float4 Specular = (1.f, 1.f, 1.f, 1.f);
    float4 vLook = vWorldPos - g_vCamPosition;
    float fShininess = vDepthDesc.w;
    Out.vSpecular = (g_vMtrlSpecular * g_vLightSpecular * fSpecularStrength) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), fShininess);
    if(fShininess <= 0.f)
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    Out.vColor = 1.f;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float4 vDiffuse = g_TextureDiffuse.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a <= 0.0f)
        discard;

    float4 vShade = g_TextureShade.Sample(LinearSampler, In.vTexcoord);
    float4 vSpecular = g_TextureSpecular.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse * vShade + vSpecular;

    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y;

    float4 vWorldPos;

    // WorldPos구하기
    // 우선 NDC좌표 구하기
    // x, y를 TEXCOORD(0 ~ 1)에서 뽑아서 NDC좌표(-1, 1 ~ 1, -1)로 변환
    // z는 깊이 텍스쳐에서 뽑아서 넣어주기
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    // Z나누기 했던걸 복구
    // Loacl * World * View * Proj / w  => Loacl * World * View * Proj
    vWorldPos *= fViewZ;

    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);

    vWorldPos = mul(vWorldPos, g_LightViewMatrix);
    vWorldPos = mul(vWorldPos, g_LightProjMatrix);

    float fDepth = vWorldPos.w;

    /* 광원기준으로 그려낸 장면상에서, 현재 픽셀이 그려졌어야할 위치에 이미 기록되어있던 깊이를 꺼내온다 */
    float2 vTexcoord;
    vTexcoord.x = (vWorldPos.x / fDepth) * 0.5f + 0.5f;
    vTexcoord.y = (vWorldPos.y / fDepth) * -0.5f + 0.5f;

    // x -> 0 ~ 1 | 투영 z / 뷰스페이스 z 까지 완료한 상태
    // y -> n ~ f | 뷰스페이스 z
    // 아무것도 안 그려져있으면 (1, 1, 1, 1)
    float4 vLightDepthDesc = g_TextureShadow.Sample(LinearSampler, vTexcoord);

    if (vLightDepthDesc.x == 1.f && vLightDepthDesc.y == 1.f && vLightDepthDesc.z == 1.f && vLightDepthDesc.w == 1.f)
    {
        return Out;
    }
    
    // OldZ = n ~ f
    float fOldZ = vLightDepthDesc.y;

    if(fDepth - 1.f > fOldZ)
    {
        Out.vColor *= 0.5f;
    }

    return Out;
}
