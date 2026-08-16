#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DecalTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);
Texture2D<float4> g_TextureDepth : register(t4);        // Renderer에서 바인딩한 깊이

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    float4 vViewPos = mul(vWorldPos, g_ViewMatrix);
    
    Out.vPosition = mul(vViewPos, g_ProjMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;     // G-Buffer Diffuse 값
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    // 화면 텍스처의 UV 계산 (투영 좌표 => 0~1범위)
    float2 vScreenUV;
    vScreenUV.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vScreenUV.y = -In.vProjPos.y / In.vProjPos.w * 0.5f + 0.5f;
    
    // Depth 를 월드 위치 복원
    float4 vDepthDesc = g_TextureDepth.Sample(PointSampler, vScreenUV);
    float fViewZ = vDepthDesc.y;
    float fNDCDepth = vDepthDesc.x;
    
    if (fViewZ <= 0.001f)
        discard;
    
    // WorldPos구하기
    // 우선 NDC좌표 구하기
    // x, y를 TEXCOORD(0 ~ 1)에서 뽑아서 NDC좌표(-1, 1 ~ 1, -1)로 변환
    // z는 깊이 텍스쳐에서 뽑아서 넣어주기
        float4 vWorldPos;
    vWorldPos.x = vScreenUV.x * 2.f - 1.f;
    vWorldPos.y = vScreenUV.y * -2.f + 1.f;
    vWorldPos.z = fNDCDepth;
    vWorldPos.w = 1.f;
    
    // Z나누기 했던걸 복구
    // Loacl * World * View * Proj / w  => Loacl * World * View * Proj
    vWorldPos *= fViewZ;

    //투영행렬 역나누기
    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    //뷰 역행렬 나누기
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);
    
    // 월드 -> 데칼 로컬 좌표
    float4 vLocalPos = mul(vWorldPos, g_InvDecalWorldMatrix);
    
    // 큐브의 범위 체크 (-0.5 ~ 0.5)
    clip(0.5f - abs(vLocalPos.xyz));     // 인자가 0보다 작으면 현재 픽셀을 버린다.(큐브 밖이면 삭제하는 함수)
    
    // 로컬 xz -> UV
    float2 vDecalUV = vLocalPos.xz + 0.5f;
    
    // 텍스처 샘플링
    float4 vDecalColor = g_DecalTexture.Sample(LinearClampSampler, vDecalUV);
    
    // 알파 마스크 계산
    uint iAlphaX = g_iMaskIndex / 2;
    uint iAlphaY = g_iMaskIndex % 2;
    float2 vMaskUV = (vDecalUV * 0.5f) + float2(iAlphaY * 0.5f, iAlphaX * 0.5f);
    
    float fMaskAlpha = g_AlphaTexture.Sample(LinearClampSampler, vMaskUV).r;
    vDecalColor.a *= fMaskAlpha * g_fDecalAlpha;
    
    if (vDecalColor.a < 0.01f)
        discard;

    Out.vColor = vDecalColor * g_vColor;
    return Out;
}