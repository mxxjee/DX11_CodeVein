#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"
//Shader_VtxMesh.hlsl

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);

static const float4 g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f };

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
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    
    
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
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fShade = saturate(dot(normalize(g_vLightDirection) * -1.f, In.vNormal));
    
    float3 vLook = (In.vWolrdPos - g_vCamPosition).xyz;
    float3 vReflect = reflect(normalize(g_vLightDirection), normalize(In.vNormal)).xyz;
    
    float fSpecular = pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 5000);
    
    //Out.vColor = g_vLightDiffuse * vMtrlDiffuse;
    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient))
    /*+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular*/;
    
    return Out;
}
