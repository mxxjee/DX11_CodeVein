#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"
//Shader_VtxMesh.hlsl

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);
Texture2D<float4> g_NormalTexture : register(t2);
Texture2D<float4> g_OpacityTexture : register(t3);

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};
struct VS_OUT_NORMAL
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

/* 정점의 기본적인 변환을 가져간다. World, View, Proj */ 
/* 정점의 구성 정보를 변형한다. (멤버를 늘리거나 , 줄이거나 ) */ 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vWolrdPos = vPosition;
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    
    return Out;
}

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
    VS_OUT_NORMAL Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vWolrdPos = vPosition;
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    
    return Out;
}

VS_OUT_NORMAL VS_MAIN_NORMAL_SAVE(VS_IN In)
{
    VS_OUT_NORMAL Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vWolrdPos = vPosition;
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent.xyz, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    
    return Out;
}

/* 포지션시멘틱에 해당하는 변수의 w로 모든 성분을 나눈다 */
/* 뷰포트로 변환한다.(윈도우좌표로 변환) */
/* 래스터라이즈 : 리턴된 정점정보를 기반으로하여 사이를 선형보간한 픽셀을 만들어준다.  */


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    vector vPick : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    vMtrlDiffuse.rgb += g_vColor.rgb;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((In.vNormal.xyz) * 0.5f + 0.5f, 0.f);  // -1 ~ 1 사이의 값을 0 ~ 1로 변환
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)

    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out;
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}


PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
    PS_OUT Out;
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float fAlpha = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord).r;
    
    clip(fAlpha - 0.1f);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.a = fAlpha;
    Out.vNormal = float4((In.vNormal.xyz) * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    
    return Out;
}

struct PS_IN_NORMAL
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);   
    
    //프리넬림효과(가장자리 까맣게)
    //float3 normal = normalize(In.vNormal);
    //float3 viewDir = normalize(g_vCamPosition - In.vWolrdPos);

    // 프레넬 계산 (실루엣 강조)
    //float fresnel = 1.0f - saturate(dot(viewDir, normal));

    // 강도 조절
    //fresnel = pow(fresnel, 3.0f);

    // 검은색으로 어둡게
    //float3 finalColor = lerp(vMtrlDiffuse.xyz, float3(0, 0, 0), fresnel * 0.7f);
    
    //vMtrlDiffuse.xyz = vMtrlDiffuse.xyz;
    
    Out.vDiffuse = vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    return Out;
}

PS_OUT PS_MAIN_OPACITY(PS_IN In)
{
    PS_OUT Out;

    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMtrlOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);


    if (vMtrlDiffuse.a <= 0.05f)
        discard;
    if (vMtrlOpacity.r <= 0.5f)
        discard;

    vMtrlOpacity *= 0.f;
    
    Out.vDiffuse = vMtrlDiffuse + vMtrlOpacity;
    Out.vNormal = float4((In.vNormal.xyz) * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    return Out;
}