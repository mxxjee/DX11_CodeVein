#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

//Texture2D g_DiffuseTexture : register(t0);
//Texture2D g_NormalTexture : register(t1);
//Texture2D<float4> g_OpacityTexture : register(t2);
//Texture2D<float4> g_PBRTexture : register(t3);
//Texture2D<float4> g_RoughnessTexture : register(t6);
//Texture2D<float4> g_EmissiveTexture : register(t10);

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);
Texture2D<float4> g_NormalTexture : register(t2);
Texture2D<float4> g_OpacityTexture : register(t3);
Texture2D<float4> g_PBRTexture : register(t4);
Texture2D<float4> g_SpecularTexture : register(t5);
Texture2D<float4> g_RoughnessTexture : register(t6);
Texture2D<float4> g_EmissiveTexture : register(t10);
struct VS_IN
{
    // 묶어서 사용할 종합 데이터
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    // 인스턴스 개별 데이터 (이 경우엔 WorldMatrix)
    float4 vWorld0 : WORLD0;
    float4 vWorld1 : WORLD1;
    float4 vWorld2 : WORLD2;
    float4 vWorld3 : WORLD3;
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
struct VS_OUT_Shadow
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPositionShadow : TEXCOORD3;
};
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 Right = float4(In.vWorld0.xyz, 0.f); // World0.w에 ObjectID를 넣어둬서 강제로 1로 만들어줌
    uint ObjID = In.vWorld0.w;  // ObjectID는 따로 빼서 보관
    float4x4 matWorld = float4x4(Right, In.vWorld1, In.vWorld2, In.vWorld3);
    
    float4x4 matWV = mul(matWorld, g_ViewMatrix);
    float4x4 matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vProjPos = Out.vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), matWorld));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), matWorld));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), matWorld); // 여기서 WorldPos.w에 ObjID 보관
    Out.vWorldPos.w = ObjID;
    
    return Out;
}

VS_OUT_Shadow VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_Shadow Out;
    float4 Look = float4(In.vWorld0.xyz, 0.f); // World0.w에 ObjectID를 넣어둬서 강제로 1로 만들어줌
    float4x4 matWorld = float4x4(Look, In.vWorld1, In.vWorld2, In.vWorld3);
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), matWorld);
    Out.vWolrdPos = vPosition;
    
    Out.vPositionShadow = mul(vPosition, g_ShadowCascade_ViewProjMatrix);
    Out.vPosition = mul(vPosition, g_ShadowCascade_ViewProjMatrix);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), matWorld));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    
    return Out;
}
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse     : SV_TARGET0;
    float4 vNormal      : SV_TARGET1;
    float4 vDepth       : SV_TARGET2;
    float4 vPick        : SV_TARGET3;
    float4 vPBR         : SV_TARGET4;
    float4 vSpecular    : SV_TARGET5;
    float4 vRoughness   : SV_TARGET6;
    float4 vEmissive    : SV_Target7;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz) * In.vTangent.w, normalize(In.vNormal.xyz));
    vNormal = float4(normalize(mul(vNormal.xyz, WorldMatrix)), 0.f);

    //임시로 반환해서 사용된다는거 확인하기
    Out.vDiffuse = g_bitFlag;
    Out.vDiffuse = g_fTime;
    
    float luminance = dot(vMtrlDiffuse.rgb, float3(0.299, 0.587, 0.114));
    float roughness = lerp(0.8, 0.3, luminance);
    
    //습기효과
    float upFacing = saturate(vNormal.y); // 위를 향할수록 1
    float wetness = g_fCaveMapWet * upFacing;
    roughness = lerp(roughness, roughness * 0.5f, wetness); // 젖으면 매끈하게
    vMtrlDiffuse *= lerp(1.0f, 0.6f, wetness); // 젖으면 어두워짐 (실제 물리 현상)
    
    float4 PBR = float4(0.0f, roughness, 1.0f, 1.0f); // 금속성 0, 거칠기 1 (기본값)
    float4 Emissive = float4(0.f, 0.f, 0.f, 1.f);
    float4 ID = (0.f, 0.f, 0.f, 1.f);
    float4 Specular = DefaultSpecular;
    float EmissiveStrength = 0.1f;
    float Depth = 5.f;
    
    //Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vDiffuse = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_bitFlag & (1 << 15))
        PBR = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bitFlag & (1 << 4))
        Emissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    if ((g_bitFlag & (1 << 7)) && (g_bitFlag & (1 << 15))) //지하죽순
    {
        float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.02f);
        float mask = g_RoughnessTexture.Sample(LinearSampler, panUV).r;

        mask = pow(mask, 2.0f);
        EmissiveStrength = g_fUnderBambooEmission; 
        Emissive = Emissive * mask;
        Depth = 7.f;
    }
    else if (g_bitFlag & (1 << 7)) //전등 + 지상죽순
    {
        float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.02f);
        float mask = g_RoughnessTexture.Sample(LinearSampler, panUV).r;

        mask = pow(mask, 2.0f);
        EmissiveStrength = g_fBambooEmission; //35
        Emissive = Emissive * mask;
    }
    if (g_bitFlag & (1 << 2))
        Specular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Emissive = Emissive * EmissiveStrength;

    Out.vDiffuse = vMtrlDiffuse;
   //vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, Depth, 400.f); //z축 5로 배경 구분
    Out.vPick = In.vWorldPos;
    Out.vSpecular = Specular;
    Out.vPBR = PBR;
    Out.vRoughness = float4(0.f, 0.f, 0.f, 1.f);
    Out.vEmissive = Emissive;
    return Out;
    
    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out;

    return Out;
}