// Shader_VtxGPUInstance.hlsl
// GPU-Driven Instancing Shader
// VS: StructuredBuffer에서 인스턴스 월드 행렬 읽기 (SV_InstanceID 기반)
// PS: 디퍼드 G-Buffer 출력 (Shader_VtxInstance.hlsl PS 기준)

#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

/* ============================================================ */
/*                          Textures                             */
/* ============================================================ */
// 수정됨 : 기존 Shader_VtxInstance.hlsl 레지스터 슬롯에 맞춤
Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);
Texture2D<float4> g_NormalTexture : register(t2);
Texture2D<float4> g_OpacityTexture : register(t3);
Texture2D<float4> g_PBRTexture : register(t4);
Texture2D<float4> g_SpecularTexture : register(t5);
Texture2D<float4> g_RoughnessTexture : register(t6);
Texture2D<float4> g_EmissiveTexture : register(t10);

/* GPU 컬링 결과 인스턴스 행렬 */
StructuredBuffer<matrix> g_InstanceMatrices : register(t11);
StructuredBuffer<uint> g_GroupOffsets : register(t12); // 그룹별 시작 오프셋

/* ============================================================ */
/*                         VS / PS 구조체                         */
/* ============================================================ */
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

// 수정됨 : Shadow용 VS_OUT
struct VS_OUT_Shadow
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPositionShadow : TEXCOORD3;
};

// 수정됨 : PS_OUT을 기존 엔진 G-Buffer 레이아웃에 맞춤
struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPick : SV_TARGET3;
    float4 vPBR : SV_TARGET4;
    float4 vSpecular : SV_TARGET5;
    float4 vRoughness : SV_TARGET6;
    float4 vEmissive : SV_Target7;
};

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

cbuffer CS_Group : register(b1)
{
    uint g_GroupIndex;
    uint3 padding000;
}

/* ============================================================ */
/*                          Vertex Shader                        */
/* ============================================================ */
VS_OUT VS_MAIN(VS_IN input, uint instanceID : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
    
    uint matrixIndex = g_GroupOffsets[g_GroupIndex] + instanceID;
    matrix world = g_InstanceMatrices[matrixIndex];
   

    // 수정됨 : World._14 -> _41 (기존 엔진은 vWorld0.w = Right.w에 ObjectID 저장)
    // StructuredBuffer는 row_major 기준이므로 _14에 ObjectID가 들어있다면 그대로 유지
    // 기존 VtxInstance에서는 vWorld0.w (= 1행 4열)에 ObjectID를 넣으므로 _14 사용
    uint objID = (uint) world._14;
    world._14 = 0.0f;

    float4x4 matWV = mul(world, g_ViewMatrix);
    float4x4 matWVP = mul(matWV, g_ProjMatrix);

    output.vPosition = mul(float4(input.vPosition, 1.0f), matWVP);
    output.vProjPos = output.vPosition;
    output.vNormal = normalize(mul(float4(input.vNormal, 0.0f), world)).xyz;
    output.vTangent = normalize(mul(float4(input.vTangent, 0.0f), world));
    output.vBinormal = normalize(mul(float4(input.vBinormal, 0.0f), world));
    output.vTexcoord = input.vTexcoord;
    output.vWorldPos = mul(float4(input.vPosition, 1.0f), world);
    output.vWorldPos.w = (float) objID; // 수정됨 : WorldPos.w에 ObjectID 보관 (기존 엔진 방식)

    return output;
}


/* ============================================================ */
/*                      Shadow Vertex Shader                     */
/* ============================================================ */
// 그림자 패스용 VS
VS_OUT_Shadow VS_MAIN_SHADOW(VS_IN input, uint instanceID : SV_InstanceID)
{
    VS_OUT_Shadow output = (VS_OUT_Shadow) 0;

    uint matrixIndex = g_GroupOffsets[g_GroupIndex] + instanceID;
    matrix world = g_InstanceMatrices[matrixIndex];
    world._14 = 0.0f; // ObjectID 제거

    float4 vPosition = mul(float4(input.vPosition, 1.0f), world);
    output.vWorldPos = vPosition;

    output.vPositionShadow = mul(vPosition, g_ShadowCascade_ViewProjMatrix);
    output.vPosition = mul(vPosition, g_ShadowCascade_ViewProjMatrix);
    output.vNormal = normalize(mul(float4(input.vNormal, 0.0f), world));
    output.vTexcoord = input.vTexcoord;
    output.vProjPos = output.vPosition;

    return output;
}


/* ============================================================ */
/*                          Pixel Shader                         */
/* ============================================================ */
// 수정됨 : 기존 Shader_VtxInstance.hlsl PS_MAIN 로직에 맞춤
PS_OUT PS_MAIN(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, input.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    // 수정됨 : DecodeNormalBC5 사용 (기존 엔진 방식)
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, input.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.0f);

    float3x3 WorldMatrix = float3x3(
        normalize(input.vTangent.xyz),
        normalize(input.vBinormal.xyz) * input.vTangent.w,
        normalize(input.vNormal.xyz)
    );
    vNormal = float4(normalize(mul(vNormal.xyz, WorldMatrix)), 0.0f);

    // PBR 기본값 (금속성 0, 거칠기 0.7, AO 1)
    float4 PBR = float4(0.0f, 0.7f, 1.0f, 0.0f);
    float4 Emissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float EmissiveStrength = 0.1f;

    // 수정됨 : bitFlag 규칙을 기존 엔진에 맞춤
    if (g_bitFlag & (1 << 15))
        PBR = g_PBRTexture.Sample(LinearSampler, input.vTexcoord);
    if (g_bitFlag & (1 << 4))
        Emissive = g_EmissiveTexture.Sample(LinearSampler, input.vTexcoord);
    if (g_bitFlag & (1 << 7))
    {
        float2 panUV = input.vTexcoord + float2(0.0f, g_fTime * 0.02f);
        float mask = g_RoughnessTexture.Sample(LinearSampler, panUV).r;
        mask = pow(mask, 2.0f);
        EmissiveStrength = 35.0f;
        Emissive = Emissive * mask;
    }
    Emissive = Emissive * EmissiveStrength;

    output.vDiffuse = vMtrlDiffuse;
    output.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.0f);
    output.vDepth = float4(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w, 5.0f, 400.0f);
    output.vPick = input.vWorldPos; // 수정됨 : .w에 ObjectID가 이미 들어있음
    output.vSpecular = float4(0.04f, 0.04f, 0.04f, 1.0f);
    output.vPBR = PBR;
    output.vRoughness = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.vEmissive = Emissive;

    return output;
}


/* ============================================================ */
/*                      Shadow Pixel Shader                      */
/* ============================================================ */
// 그림자 패스용 PS
PS_OUT_SHADOW PS_MAIN_SHADOW(VS_OUT input)
{
    PS_OUT_SHADOW output = (PS_OUT_SHADOW) 0;

    return output;
}