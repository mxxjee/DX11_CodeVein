// Shader_Toon.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_ShadCTexture : register(t1); // 그림자 영역 색상
Texture2D<float4> g_CtrlTexture : register(t2); // 컨트롤 마스크 (R:미사용, G:메탈릭, B:림라이트)
Texture2D<float4> g_BaseTexture : register(t3);

StructuredBuffer<float3> g_MorphDeltaPositions : register(t10);
StructuredBuffer<float3> g_MorphDeltaNormals : register(t11);

// 툰 셰이딩 파라미터
static const float SHADOW_THRESHOLD = 0.3f; // 그림자 경계 위치 (낮을수록 넓게 돼서 밝아보임)
static const float SHADOW_SOFTNESS = 0.005f; // 그림자 경계 부드러움(높을수록 부드럽게 되고 낮을수록 경계의 차이가 심해짐)
static const float RIM_POWER = 8.0f; // 림라이트 폭 (높을수록 좁아짐)
static const float RIM_INTENSITY = 0.5f; // 림라이트 강도

// 메탈릭 파라미터
static const float METAL_SPEC_POWER = 64.0f; // 메탈릭 스페큘러 shapeness(뭔지모르겠음)
static const float METAL_INTENSITY = 1.2f; // 메탈릭 반사 강도

// 아웃라인 파라미터
static const float OUTLINE_WIDTH = 0.003f; // 아웃라인 두께
static const float4 OUTLINE_COLOR = float4(0.15f, 0.1f, 0.1f, 1.0f); // 아웃라인 색상

static const float4 g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f };

float GetMorphWeight(uint i)
{
    //uint vecIndex = i / 4;
    //uint component = i % 4;
    //return g_MorphWeightsPacked[vecIndex][component];
    return 0;
}

uint GetMorphIndex(uint i)
{
    //uint vecIndex = i / 4;
    //uint component = i % 4;
    //return g_MorphIndicesPacked[vecIndex][component];
    return 0;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHT;
    uint uVertexID : SV_VertexID;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vViewDir : TEXCOORD2;
};

struct VS_OUT_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_OUTLINE VS_OUTLINE(VS_IN In)
{
    VS_OUT_OUTLINE Out;
    
    float3 MorphedPos = In.vPosition;
    float3 MorphedNor = In.vNormal;
    
    // Morph 적용
    [loop]
    for (uint i = 0; i < g_NumActiveMorphs; ++i)
    {
        uint morphIndexInMesh = GetMorphIndex(i);
        float morphWeight = GetMorphWeight(i);
        
        uint deltaIndex = (morphIndexInMesh * g_NumVertices) + In.uVertexID;
        
        float3 deltaPos = g_MorphDeltaPositions[deltaIndex];
        float3 deltaNor = g_MorphDeltaNormals[deltaIndex];
        
        MorphedPos += deltaPos * morphWeight;
        MorphedNor += deltaNor * morphWeight;
    }
    
    float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z));
    
    // 본 스키닝
    float4x4 BoneMatrix = (g_BonesMatrices[In.vBlendIndices.x] * In.vBlendWeights.x)
                        + (g_BonesMatrices[In.vBlendIndices.y] * In.vBlendWeights.y)
                        + (g_BonesMatrices[In.vBlendIndices.z] * In.vBlendWeights.z)
                        + (g_BonesMatrices[In.vBlendIndices.w] * fWeightW);
    
    float3 FinalPos = mul(float4(MorphedPos, 1.f), BoneMatrix).xyz;
    float3 FinalNor = mul(float4(MorphedNor, 0.f), BoneMatrix).xyz;
    
    // 월드 변환
    float4 WorldPos = mul(float4(FinalPos, 1.f), g_WorldMatrix);
    float3 WorldNor = normalize(mul(float4(FinalNor, 0.f), g_WorldMatrix).xyz);
    
    // 뷰-프로젝션 변환 (먼저 클립 스페이스로)
    float4 ViewPos = mul(WorldPos, g_ViewMatrix);
    float4 ClipPos = mul(ViewPos, g_ProjMatrix);
    
    // [수정] 뷰 스페이스에서 노말 변환
    float3 ViewNor = normalize(mul(float4(WorldNor, 0.f), g_ViewMatrix).xyz);
    
    // [수정] 클립 스페이스에서 노말 방향으로 확장 (스크린 기준 균일한 두께)
    float2 screenNormal = normalize(ViewNor.xy);
    ClipPos.xy += screenNormal * OUTLINE_WIDTH * ClipPos.w;
    
    Out.vPosition = ClipPos;
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

// Pass 0
struct PS_IN_OUTLINE
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_OUTLINE(PS_IN_OUTLINE In)
{
    PS_OUT Out;
    
    // 알파 테스트 (투명한 부분은 아웃라인도 그리지 않음)
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //if (vDiffuse.a < 0.5f)
    //    discard;
    
    Out.vColor = OUTLINE_COLOR;
    
    return Out;
}

// Pass 1 : 기본 버텍스 셰이더
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float3 MorphedPos = In.vPosition;
    float3 MorphedNor = In.vNormal;
    
    // Morph 적용
    [loop]
    for (uint i = 0; i < g_NumActiveMorphs; ++i)
    {
        uint morphIndexInMesh = GetMorphIndex(i);
        float morphWeight = GetMorphWeight(i);
        
        uint deltaIndex = (morphIndexInMesh * g_NumVertices) + In.uVertexID;
        
        float3 deltaPos = g_MorphDeltaPositions[deltaIndex];
        float3 deltaNor = g_MorphDeltaNormals[deltaIndex];
        
        MorphedPos += deltaPos * morphWeight;
        MorphedNor += deltaNor * morphWeight;
    }
    
    float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z));
    
    // 메쉬 정점 본 스키닝(GPU단계)
    float4x4 BoneMatrix = (g_BonesMatrices[In.vBlendIndices.x] * In.vBlendWeights.x)
                        + (g_BonesMatrices[In.vBlendIndices.y] * In.vBlendWeights.y)
                        + (g_BonesMatrices[In.vBlendIndices.z] * In.vBlendWeights.z)
                        + (g_BonesMatrices[In.vBlendIndices.w] * fWeightW);
    
    float3 FinalPos = mul(float4(MorphedPos, 1.f), BoneMatrix).xyz;
    float3 FinalNor = mul(float4(MorphedNor, 0.f), BoneMatrix).xyz;
    
    // 탄젠트, 바이노말도 스키닝
    float3 FinalTan = mul(float4(In.vTangent, 0.f), BoneMatrix).xyz;
    float3 FinalBiN = mul(float4(In.vBiNormal, 0.f), BoneMatrix).xyz;

    // 월드 변환
    float4 WorldPos = mul(float4(FinalPos, 1.f), g_WorldMatrix);
    Out.vWorldPos = WorldPos;
    
    // 시선 방향 계산
    Out.vViewDir = normalize(g_vCamPosition.xyz - WorldPos.xyz);
    
    float4 ViewPos = mul(WorldPos, g_ViewMatrix);
    Out.vPosition = mul(ViewPos, g_ProjMatrix);
    
    // 노말, 탄젠트, 바이노말 월드 변환
    Out.vNormal = normalize(mul(float4(FinalNor, 0.f), g_WorldMatrix).xyz);
    Out.vTangent = normalize(mul(float4(FinalTan, 0.f), g_WorldMatrix).xyz);
    Out.vBiNormal = normalize(mul(float4(FinalBiN, 0.f), BoneMatrix).xyz);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vViewDir : TEXCOORD2;
};


// Pass 1 : 기본 픽셀 셰이더
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vShadow = g_ShadCTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vCtrl = g_CtrlTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    // 노말 정규화
    float3 N = normalize(In.vNormal);
    float3 L = normalize(-g_vLightDirection.xyz); // 광원 방향
    float3 V = normalize(In.vViewDir); // 시선 방향
    float3 H = normalize(L + V); // 하프벡터
    
    // 기본 셀 셰이딩 (Half Lambert + smoothstep)
    float NdotL = dot(N, L);
    float halfLambert = NdotL * 0.5f + 0.5f; // 0~1로 리매핑
    
    // smoothstep으로 부드러운 경계
    float shadowMask = smoothstep(
        SHADOW_THRESHOLD - SHADOW_SOFTNESS,
        SHADOW_THRESHOLD + SHADOW_SOFTNESS,
        halfLambert
    );
    
    // diff와 shad_c 블렌딩
    float3 baseColor = lerp(vShadow.rgb, vDiffuse.rgb, shadowMask);
    
    // 림라이트 (Rim Light)
    float rim = 1.0f - saturate(dot(N, V));
    rim = pow(rim, RIM_POWER);
    
    // 빛 받는 쪽에만 림라이트 적용
    float rimMask = saturate(NdotL + 0.5f) * vCtrl.b;
    float3 rimColor = rim * RIM_INTENSITY * rimMask * g_vLightDiffuse.rgb;
    
    // 메탈릭 반사
    float metallic = vCtrl.g;
    float3 metallicColor = float3(0.0f, 0.0f, 0.0f);
    
    if (metallic > 0.01f)
    {
        // Blinn-Phong 스페큘러
        float NdotH = saturate(dot(N, H));
        float spec = pow(NdotH, METAL_SPEC_POWER);
        
        // 메탈은 자기 색상으로 반사 (금색 버클 = 금색 반사)
        float3 metalSpecColor = vDiffuse.rgb * 1.5f + float3(0.1f, 0.1f, 0.1f);
        
        // 프레넬 (메탈은 전체적으로 높은 반사율)
        float fresnel = 0.85f + 0.15f * pow(1.0f - saturate(dot(N, V)), 2.0f);
        
        // 라이트 방향 고려
        float lightFactor = saturate(NdotL + 0.3f);
        
        metallicColor = spec * metalSpecColor * fresnel * metallic * lightFactor * METAL_INTENSITY;
        
        // 메탈 부분은 diffuse 약간 밝게 (광택감)
        baseColor = lerp(baseColor, vDiffuse.rgb * 1.1f, metallic * 0.3f);
    }
    
    // 최종 색상
    float3 finalColor = baseColor + rimColor + metallicColor;
    
    Out.vColor = float4(finalColor, vDiffuse.a);
    
    return Out;
}

// Pass 2 : 엔젤 링, 항상 밝은 부분 셰이더
PS_OUT PS_HAIR(PS_IN In)
{
    PS_OUT Out;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vShadow = g_ShadCTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vCtrl = g_CtrlTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuse.a < 0.1f)
        discard;
    
    // 노말 정규화
    float3 N = normalize(In.vNormal);
    float3 L = normalize(-g_vLightDirection.xyz); // 광원 방향
    float3 V = normalize(In.vViewDir); // 시선 방향
    float3 H = normalize(L + V); // 하프벡터
    
    // 기본 셀 셰이딩 (Half Lambert + smoothstep)
    float NdotL = dot(N, L);
    float halfLambert = NdotL * 0.5f + 0.5f; // 0~1로 리매핑
    
    // smoothstep으로 부드러운 경계
    float shadowMask = smoothstep(
        SHADOW_THRESHOLD - SHADOW_SOFTNESS,
        SHADOW_THRESHOLD + SHADOW_SOFTNESS,
        halfLambert
    );
    
    // diff와 shad_c 블렌딩
    float3 baseColor = lerp(vShadow.rgb, vDiffuse.rgb, shadowMask);
    
    // 림라이트 (Rim Light)
    float rim = 1.0f - saturate(dot(N, V));
    rim = pow(rim, RIM_POWER);
    
    // 빛 받는 쪽에만 림라이트 적용
    float rimMask = saturate(NdotL + 0.5f) * vCtrl.b;
    float3 rimColor = rim * RIM_INTENSITY * rimMask * g_vLightDiffuse.rgb;
    
    // 메탈릭 반사
    float metallic = vCtrl.g;
    float3 metallicColor = float3(0.0f, 0.0f, 0.0f);
    
    if (metallic > 0.01f)
    {
        // Blinn-Phong 스페큘러
        float NdotH = saturate(dot(N, H));
        float spec = pow(NdotH, METAL_SPEC_POWER);
        
        // 메탈은 자기 색상으로 반사 (금색 버클 = 금색 반사)
        float3 metalSpecColor = vDiffuse.rgb * 1.5f + float3(0.1f, 0.1f, 0.1f);
        
        // 프레넬 (메탈은 전체적으로 높은 반사율)
        float fresnel = 0.85f + 0.15f * pow(1.0f - saturate(dot(N, V)), 2.0f);
        
        // 라이트 방향 고려
        float lightFactor = saturate(NdotL + 0.3f);
        
        metallicColor = spec * metalSpecColor * fresnel * metallic * lightFactor * METAL_INTENSITY;
        
        // 메탈 부분은 diffuse 약간 밝게 (광택감)
        baseColor = lerp(baseColor, vDiffuse.rgb * 1.1f, metallic * 0.3f);
    }
    
    // 최종 색상 합성
    float3 finalColor = baseColor + rimColor + metallicColor; // metallicColor 추가

    float3 highlight = g_BaseTexture.Sample(LinearSampler, In.vTexcoord);
    finalColor += highlight.g * 0.5f;
    
    Out.vColor = float4(finalColor, vDiffuse.a);
    
    return Out;
}

// Pass 3 : 기본 셰이더
PS_OUT PS_MAIN_FALLBACK(PS_IN In)
{
    PS_OUT Out;
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
   
    float fShade = max(dot(normalize(g_vLightDirection.xyz) * -1.f, normalize(In.vNormal)), 0.f);
    
    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    
    return Out;
}
