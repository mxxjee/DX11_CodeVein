#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"
//Shader_VtxMesh.hlsl

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);
Texture2D<float4> g_NormalTexture : register(t2);
Texture2D<float4> g_OpacityTexture : register(t3);
Texture2D<float4> g_PBRTexture : register(t4);
Texture2D<float4> g_SpecularTexture : register(t5);
Texture2D<float4> g_RoughnessTexture : register(t6);
Texture2D<float4> g_EmissiveTexture : register(t10);
Texture2D<float4> g_NoiseTexture : register(t12);
Texture2D<float4> g_PlayerRimNoiseTexture : register(t13);

Texture2D<float4> g_TextureDepth : register(t30); //30번에 맵의 depth가져온거

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
struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
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

VS_OUT_SHADOW VS_MAIN_SHADOW_CASCADE(VS_IN In)
{
    VS_OUT_SHADOW Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    //Out.vWolrdPos = vPosition;
    
    //Out.vPositionShadow = mul(vPosition, g_ShadowCascade_ViewProjMatrix);
    Out.vPosition = mul(vPosition, g_ShadowCascade_ViewProjMatrix);
    Out.vTexcoord = In.vTexcoord;
    //Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    //Out.vTexcoord = In.vTexcoord;
    //Out.vProjPos = Out.vPosition;
    
    
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

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

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

struct PS_OUT_GODRAY
{
    float4 vDiffuse : SV_TARGET0;
    //float4 vDepth : SV_TARGET1;

};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((In.vNormal.xyz) * 0.5f + 0.5f, 0.f);  // -1 ~ 1 사이의 값을 0 ~ 1로 변환
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = float4(0.04f, 0.04f, 0.04f, 1.f);
    //이거 없어도 넣어줘야함
    Out.vPBR = float4(0.0f, 0.8f, 1.0f, 0.0f); // 금속성 0, 거칠기 1 (기본값)
    Out.vRoughness = float4(0.f, 0.f, 0.f, 0.f);

    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

struct PS_OUT_SHADOW_CASCADE
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out;

    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

void PS_MAIN_SHADOW_CASCADE(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW_CASCADE Out;

    // Noise 텍스처
    if (g_bUseNoise)
    {
        float fLifeRatio = saturate(g_fTimeElapsed);
        float fNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).r;
        
        if (fNoise <= fLifeRatio)
            discard;
    }
    
    // return Out;
}

PS_OUT_GODRAY PS_MAIN_GODRAYMESH(PS_IN In)
{
    PS_OUT_GODRAY Out;

    float fMask = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord).r;
    //float Random = g_ObjectID / 10000000.f;
    float2 scrolledUV = In.vTexcoord;
    scrolledUV.y += g_fTime * g_fGodRayMeshScrollSpeed;
    scrolledUV.x += sin(g_fTime * g_fGodRayMeshSwayFreq + scrolledUV.y * g_fGodRayMeshSwayPhase) * g_fGodRayMeshSwayAmp ;

    float fNoise = g_RoughnessTexture.Sample(LinearSampler, scrolledUV).r;
    fNoise = pow(fNoise, g_fGodRayMeshContrast);

    float godRay = fMask * fNoise * g_fGodRayMeshIntensity;

    //물체 끝 경계에서 흐리게
    float fEdgeFade = smoothstep(0.f, g_fGodRayMeshEdgeFade, In.vTexcoord.x)
                    * smoothstep(0.f, g_fGodRayMeshEdgeFade, 1.f - In.vTexcoord.x)
                    * smoothstep(0.f, g_fGodRayMeshEdgeFade, In.vTexcoord.y)
                    * smoothstep(0.f, g_fGodRayMeshEdgeFade, 1.f - In.vTexcoord.y);

    //소프트 파티클(물체 경계에서 흐리게)
    float2 vScreenUV = In.vProjPos.xy / In.vProjPos.w;
    vScreenUV = vScreenUV * 0.5f + 0.5f;
    vScreenUV.y = 1.f - vScreenUV.y;

    float fSceneDepth = g_TextureDepth.Sample(PointSampler, vScreenUV).y;
    float fGodRayDepth = In.vProjPos.w;
    float fSoftFade = saturate((fSceneDepth - fGodRayDepth) / g_fSoftParticleRange);

    //옆에서 보명 투명해짐
    float3 vViewDir = normalize(g_vCamPosition.xyz - In.vWolrdPos.xyz);
    float3 vNormal = normalize(In.vNormal.xyz);
    float fAngleFade = pow(abs(dot(vViewDir, vNormal)), g_fGodRayMeshAngleFadePower);
    
    //거리페이드
    float fDistance = length(g_vCamPosition.xyz - In.vWolrdPos.xyz);
    float fDistanceFade = saturate((fDistance - g_fGodRayMeshFadeNear) / (g_fGodRayMeshFadeFar - g_fGodRayMeshFadeNear));

    Out.vDiffuse = float4(g_vGodRayMeshColor * godRay * fEdgeFade * fSoftFade * fAngleFade * fDistanceFade, 1.f);

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
    
    //float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz) * -1, normalize(In.vNormal.xyz));
    vNormal = float4(normalize(mul(vNormal.xyz, WorldMatrix)), 0.f);
    
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
    
    float4 Specular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = Specular;
    //이거 없어도 넣어줘야함
    Out.vPBR = float4(0.f, 0.f, 1.0f, 0.f); // 금속성 0, 거칠기 1 (기본값)
    Out.vRoughness = float4(1.f, 1.f, 1.f, 1.f); //머리
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz) * -1, normalize(In.vNormal.xyz));
    vNormal = float4(normalize(mul(vNormal.xyz, WorldMatrix)), 0.f);
    
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
    
    float4 Specular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    float4 PBR = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    float4 Roughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = Specular;
    Out.vPBR = PBR;
    Out.vRoughness = Roughness;   
    return Out;
}

PS_OUT PS_MAIN_OPACITY(PS_IN In)
{
    PS_OUT Out;

    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMtrlOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
    {
        Out.vDiffuse = vMtrlOpacity;
    }
    else
        Out.vDiffuse = vMtrlDiffuse;
   
    float4 Specular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vNormal = float4((In.vNormal.xyz) * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = Specular;
    
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR_MAP(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz) * In.vTangent.w, normalize(In.vNormal.xyz));
    vNormal = float4(normalize(mul(vNormal.xyz, WorldMatrix)), 1.f);

    //임시로 반환해서 사용된다는거 확인하기
    Out.vDiffuse = g_bitFlag;
    Out.vDiffuse = g_fTime;

    float luminance = dot(vMtrlDiffuse.rgb, float3(0.299, 0.587, 0.114));
    float roughness = lerp(0.8, 0.3, luminance);
    
    //습기효과
    float upFacing = saturate(vNormal.y); // 위를 향할수록 1
    float wetness = g_fCaveMapWet * upFacing; // ImGui: 0.0~1.0, 동굴 zone에서 0.5~0.8 //0.45 그다음 0.19
    roughness = lerp(roughness, roughness * 0.5f, wetness); // 젖으면 매끈하게
    vMtrlDiffuse *= lerp(1.0f, 0.6f, wetness); // 젖으면 어두워짐 (실제 물리 현상)
    
    float4 PBR = float4(0.0f, roughness, 1.0f, 1.0f); // 금속성 0, 거칠기 1 (기본값)
    float4 Emissive = float4(0.f, 0.f, 0.f, 1.f);
    float4 ID = float4(0.f, 0.f, 0.f, 1.f);
    float4 Specular = DefaultSpecular;
    float EmissiveStrength = 0.1f; //0.1
    float Depth = 5.f;
    
    Out.vDiffuse = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vDiffuse = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_bitFlag & (1 << 15))
        PBR = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bitFlag & (1 << 4))
        Emissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    if ((g_bitFlag & (1 << 7)) && (g_bitFlag & (1 << 15))) //지하죽순
    {
        float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.02f);
        float mask = g_RoughnessTexture.Sample(LinearSampler, panUV).r;

        mask = pow(mask, 2.0f);
        EmissiveStrength = g_fUnderBambooEmission; //300
        Emissive = Emissive * mask;
        Depth = 7.f;
    }
    else if (g_bitFlag & (1 << 7)) //전등 + 지상죽순
    {
        float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.02f);
        float mask = g_RoughnessTexture.Sample(LinearSampler, panUV).r;

        mask = pow(mask, 2.0f);
        EmissiveStrength = g_fBambooEmission; //35 실제로는 지하 전등 죽순
        Emissive = Emissive * mask;
    }

    if (g_bitFlag & (1 << 2))
        Specular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Emissive = Emissive * EmissiveStrength;

    Out.vDiffuse = vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, Depth, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = Specular;
    Out.vPBR = PBR;
    Out.vRoughness = float4(0.f, 0.f, 0.f, 1.f);
    Out.vEmissive = Emissive;
    
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR_DISSOLVE(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz) * In.vTangent.w, normalize(In.vNormal.xyz));
    vNormal = float4(normalize(mul(vNormal.xyz, WorldMatrix)), 1.f);

    //임시로 반환해서 사용된다는거 확인하기
    Out.vDiffuse = g_bitFlag;
    Out.vDiffuse = g_fTime;

    float luminance = dot(vMtrlDiffuse.rgb, float3(0.299, 0.587, 0.114));
    float roughness = lerp(0.8, 0.3, luminance);
    
    float4 PBR = float4(0.0f, roughness, 1.0f, 1.0f); // 금속성 0, 거칠기 1 (기본값)
    float4 Emissive = float4(0.f, 0.f, 0.f, 1.f);
    float4 ID = float4(0.f, 0.f, 0.f, 1.f);
    float4 Specular = DefaultSpecular;
    float EmissiveStrength = 0.1f; //0.1
    float Depth = 5.f;
    
    Out.vDiffuse = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vDiffuse = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_bitFlag & (1 << 15))
        PBR = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bitFlag & (1 << 4))
        Emissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    if ((g_bitFlag & (1 << 7)) && (g_bitFlag & (1 << 15))) //지하죽순
    {
        float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.02f);
        float mask = g_RoughnessTexture.Sample(LinearSampler, panUV).r;

        mask = pow(mask, 2.0f);
        EmissiveStrength = 2600.f; //300
        Emissive = Emissive * mask;
        Depth = 7.f;
    }
    else if (g_bitFlag & (1 << 7)) //전등 + 지상죽순  
    {
        float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.02f);
        float mask = g_RoughnessTexture.Sample(LinearSampler, panUV).r;

        mask = pow(mask, 2.0f);
        EmissiveStrength = g_fBambooEmission; //35 실제로는 지하 전등 죽순
        Emissive = Emissive * mask;
    }

    if (g_bitFlag & (1 << 2))
        Specular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Emissive = Emissive * EmissiveStrength;

    if (g_fRimOn)
    {
        float3 V = normalize(g_vCamPosition.xyz - In.vWolrdPos.xyz);
        float rim = 1 - saturate(dot(vNormal.xyz, V));
        rim = pow(rim, g_fRimIntensity);
    
        float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
        float mask = g_PlayerRimNoiseTexture.Sample(LinearSampler, panUV).r;
    
        float3 rimTint = lerp(float3(0.3f, 0.f, 0.8f), float3(0.f, 0.2f, 1.f), rim);

        float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
        Emissive = rimColor;
    }
    
    Out.vDiffuse = vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 6.f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = Specular;
    Out.vPBR = PBR;
    Out.vRoughness = float4(0.f, 0.f, 0.f, 1.f);
    Out.vEmissive = Emissive;
    
    // Noise 텍스처
    if (g_bUseNoise)
    {
        float fLifeRatio = saturate(g_fTimeElapsed);
        float fNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).r;
        
        if (fNoise <= fLifeRatio)
            discard;
        
        // 엣지 글로우
        float fEdge = smoothstep(fLifeRatio, fLifeRatio + 0.2f, fNoise);
        Out.vDiffuse.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
    }
    
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR_GODRAY(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(normalize(In.vTangent.xyz), normalize(In.vBinormal.xyz) * -1, normalize(In.vNormal.xyz));
    vNormal = float4(normalize(mul(vNormal.xyz, WorldMatrix)), 1.f);

    //임시로 반환해서 사용된다는거 확인하기
    Out.vDiffuse = g_bitFlag;
    Out.vDiffuse = g_fTime;

    float4 PBR = float4(0.0f, 0.7f, 1.0f, 1.0f); // 금속성 0, 거칠기 1 (기본값)
    float4 Emissive = float4(0.f, 0.f, 0.f, 1.f);
    float4 ID = float4(0.f, 0.f, 0.f, 1.f);
    float4 Specular = DefaultSpecular;
    float EmissiveStrength = 5.f;
    
    Out.vDiffuse = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vDiffuse = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    
    Emissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    Emissive = Emissive * EmissiveStrength;

    Out.vDiffuse = vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = Specular;
    Out.vPBR = PBR;
    Out.vRoughness = float4(0.f, 0.f, 0.f, 1.f);
    Out.vEmissive = Emissive;
    return Out;
}
