// Shader_AnimMesh.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);
Texture2D<float4> g_NormalTexture : register(t2); 
Texture2D<float4> g_IDTexture : register(t3); //의상 디퓨즈 색을 구분하기 위한 ID (오파시티)
Texture2D<float4> g_MetalnessTexture : register(t4);
Texture2D<float4> g_SpecularTexture : register(t5);
Texture2D<float4> g_RoughnessTexture : register(t6);
Texture2D<float4> g_OpacityTexture : register(t7);
Texture2D<float4> g_EmissiveTexture : register(t10);
Texture2D<float4> g_PlayerBrowTexture : register(t11); //플레이어 눈썹 텍스처
Texture2D<float4> g_NoiseTexture : register(t12);
Texture2D<float4> g_PlayerEyelashTexture : register(t13); //플레이어 아이래쉬 텍스쳐
Texture2D<float4> g_PlayerEyeWhiteTexture : register(t14); //플레이어 아이래쉬 텍스쳐
Texture2D<float4> g_PlayerEyeDetailTexture : register(t15); //플레이어 아이래쉬 텍스쳐
Texture2D<float4> g_PlayerEyeHighlightTexture : register(t16); //플레이어 아이래쉬 텍스쳐
Texture2D<float4> g_PlayerTattoTexture : register(t17); //페이스 페인팅 텍스쳐
Texture2D<float4> g_PlayerRimNoiseTexture : register(t18); //페이스 페인팅 텍스쳐



//로컬로 상수버퍼를 만들어서 사용해도된다.

struct BoneMatrix
{
    row_major float4x4 mat;
};

StructuredBuffer<BoneMatrix> g_CombinedMatrix : register(t20);
StructuredBuffer<BoneMatrix> g_OffsetMatrix : register(t21);
StructuredBuffer<uint>       g_RemapBone : register(t22);

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD1; //UV1 추가
    uint4 vBlendIndices : BLENDINDICES0;
    uint4 vBlendIndices2 : BLENDINDICES1;
    float4 vBlendWeights : BLENDWEIGHT0;
    float4 vBlendWeights2 : BLENDWEIGHT1;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD3;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct VS_OUT_NORMAL
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBiNormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD3;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

float4x4 SkinMatrix(uint iLocalSlot)
{
     
    //iLocalSlot은 기존 파츠 모델들이 가지고 있던 m_vecBlendIndices를 마스터기준으로 리맵한 g_RemapBone을 기준으로 마스터 인덱스로 변환

    uint iMasterIndex = g_RemapBone[iLocalSlot]; //이게이제 마스터본 인덱스 기준으로
    //최종 스킨 (g_BonesMatrices[In.vBlendIndices.x] = 이제 Master의 Offset과 Master의 ComBined행려을 곱해서 VS에서 생성해준다.
    return mul(g_OffsetMatrix[iMasterIndex].mat, g_CombinedMatrix[iMasterIndex].mat);
    //return mul(g_CombinedMatrix[iMasterIndex], g_OffsetMatrix[iMasterIndex]);
    
}
float2 RotateUV(float2 uv, float2 pivot, float rad)
{
    float s = sin(rad);
    float c = cos(rad);
    uv -= pivot;
    uv = float2(uv.x * c - uv.y * s, uv.x * s + uv.y * c);
    uv += pivot;
    return uv;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    //float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z) + (In.vBlendWeights.w) + (In.vBlendWeights2.x) + (In.vBlendWeights2.y) + (In.vBlendWeights2.z));
    
    float4x4 BoneMatrix = (SkinMatrix(In.vBlendIndices.x )* In.vBlendWeights.x)
                        + (SkinMatrix(In.vBlendIndices.y )* In.vBlendWeights.y)
                        + (SkinMatrix(In.vBlendIndices.z )* In.vBlendWeights.z)
                        + (SkinMatrix(In.vBlendIndices.w )* In.vBlendWeights.w)
                        + (SkinMatrix(In.vBlendIndices2.x) * In.vBlendWeights2.x)
                        + (SkinMatrix(In.vBlendIndices2.y) * In.vBlendWeights2.y)
                        + (SkinMatrix(In.vBlendIndices2.z) * In.vBlendWeights2.z)
                        + (SkinMatrix(In.vBlendIndices2.w) * In.vBlendWeights2.w);
    
    
    float4 Position = mul(float4(In.vPosition, 1.f), BoneMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(Position, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
    VS_OUT_NORMAL Out;
    
    //float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z) + (In.vBlendWeights.w) + (In.vBlendWeights2.x) + (In.vBlendWeights2.y) + (In.vBlendWeights2.z));
    
    float4x4 BoneMatrix = (SkinMatrix(In.vBlendIndices.x) * In.vBlendWeights.x)
                        + (SkinMatrix(In.vBlendIndices.y) * In.vBlendWeights.y)
                        + (SkinMatrix(In.vBlendIndices.z) * In.vBlendWeights.z)
                        + (SkinMatrix(In.vBlendIndices.w) * In.vBlendWeights.w)
                        + (SkinMatrix(In.vBlendIndices2.x) * In.vBlendWeights2.x)
                        + (SkinMatrix(In.vBlendIndices2.y) * In.vBlendWeights2.y)
                        + (SkinMatrix(In.vBlendIndices2.z) * In.vBlendWeights2.z)
                        + (SkinMatrix(In.vBlendIndices2.w) * In.vBlendWeights2.w);
    
    
    float4 Position = mul(float4(In.vPosition, 1.f), BoneMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    float4 vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);
    float4 vBiNormal = mul(float4(In.vBiNormal, 0.f), BoneMatrix);
    Out.vWorldPos = mul(Position, g_WorldMatrix);
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(Position, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vTexcoord1 = In.vTexcoord1;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
    Out.vBiNormal = normalize(mul(vBiNormal, g_WorldMatrix));
    
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD3;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    //float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z) + (In.vBlendWeights.w) + (In.vBlendWeights2.x) + (In.vBlendWeights2.y) + (In.vBlendWeights2.z));
    
    float4x4 BoneMatrix = (SkinMatrix(In.vBlendIndices.x) * In.vBlendWeights.x)
                        + (SkinMatrix(In.vBlendIndices.y) * In.vBlendWeights.y)
                        + (SkinMatrix(In.vBlendIndices.z) * In.vBlendWeights.z)
                        + (SkinMatrix(In.vBlendIndices.w) * In.vBlendWeights.w)
                        + (SkinMatrix(In.vBlendIndices2.x) * In.vBlendWeights2.x)
                        + (SkinMatrix(In.vBlendIndices2.y) * In.vBlendWeights2.y)
                        + (SkinMatrix(In.vBlendIndices2.z) * In.vBlendWeights2.z)
                        + (SkinMatrix(In.vBlendIndices2.w) * In.vBlendWeights2.w);
    
    
    float4 Position = mul(float4(In.vPosition, 1.f), BoneMatrix);
    
    matrix matWV;
    matWV = mul(g_WorldMatrix, g_ShadowCascade_ViewProjMatrix);
    
    Out.vPosition = mul(Position, matWV);
    
    return Out;
}


//struct PS_OUT
//{
//    float4 vDiffuse : SV_TARGET0;
//    float4 vNormal : SV_TARGET1;
//    float4 vDepth : SV_TARGET2;
//    vector vPick : SV_TARGET3;
//};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    vector vPick : SV_TARGET3;
    float4 vPBR : SV_TARGET4;
    float4 vSpecular : SV_TARGET5;
    float4 vRoughness : SV_TARGET6;
    float4 vEmissive : SV_Target7;
    
};

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((In.vNormal).xyz * 0.5f + 0.5f, 1.f);

    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.5f, 50.f);
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    
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

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out;
    
    return Out;
}

struct PS_IN_NORMAL
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD3;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

PS_OUT PS_CLOTH(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    float4 vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);
     
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float3 vId = g_IDTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    IdMasks tIdMasks = DecodeIdMap6(vId);
    //Tint는 색조 
    float3 vTint = tIdMasks.L1 * g_vPlayerInnerColor[0].rgb +
    tIdMasks.L2 * g_vPlayerInnerColor[1].rgb +
    tIdMasks.L3 * g_vPlayerInnerColor[2].rgb +
    tIdMasks.L4 * g_vPlayerInnerColor[3].rgb +
    tIdMasks.L5 * g_vPlayerInnerColor[4].rgb +
    tIdMasks.L6 * g_vPlayerInnerColor[5].rgb;
    
    float3 vDyed = vMtrlDiffuse.rgb * vTint; //최종 염색한 색상
    
    float3 vFinalColor = lerp(vMtrlDiffuse.rgb, vDyed, tIdMasks.fAny * g_fPlayerColorStrength);
    
    float4 Specular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    float4 PBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    
    //림라이트
    float3 V = normalize(g_vCamPosition.xyz - In.vWolrdPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_PlayerRimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.3f, 0.f, 0.8f), float3(0.f, 0.2f, 1.f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive = rimColor;

    Out.vDiffuse = float4(vFinalColor, vMtrlDiffuse.a);
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 6.f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID);
    Out.vSpecular = Specular;
    Out.vPBR = PBR;
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = vEmissive;
    
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

PS_OUT PS_MAIN_NORMAL_PBR(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if(vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);
     
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    float depthz = 6.f;
    
    if (g_bitFlag & FLAGSPECULAR)
    {   
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGEMISSIVE)
    {
        vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGSHININESS)  
    {
        vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if ((g_bitFlag & FLAGSPECULAR) && (g_bitFlag & FLAGEMISSIVE)) //일단 블러드웨폰 밝기 올리기위해서
    {
        vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
        depthz = 11.f;

    }
       
    if (vRoughness.r >= 0.5f)
    {
        float3 LinearSkinColor = pow(g_vSkinTint, 2.2f);
        vMtrlDiffuse.xyz *= LinearSkinColor;
    }
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, depthz, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR; 
    Out.vSpecular = vSpecular;
    Out.vRoughness = vRoughness;
    Out.vEmissive = vEmissive;
    
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

PS_OUT PS_MAIN_NORMAL_PBR_RIM(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);
     
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    float depthz = 6.f;
    
    if (g_bitFlag & FLAGSPECULAR)
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGEMISSIVE)
    {
        vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGSHININESS)
    {
        vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if ((g_bitFlag & FLAGSPECULAR) && (g_bitFlag & FLAGEMISSIVE)) //일단 블러드웨폰 밝기 올리기위해서
    {
        vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
        depthz = 11.f;

    }
       
    if (vRoughness.r >= 0.5f)
    {
        float3 LinearSkinColor = pow(g_vSkinTint, 2.2f);
        vMtrlDiffuse.xyz *= LinearSkinColor;
    }
    
     //림라이트
    float3 V = normalize(g_vCamPosition.xyz - In.vWolrdPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_PlayerRimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.3f, 0.f, 0.8f), float3(0.f, 0.2f, 1.f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive = rimColor;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, depthz, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR;
    Out.vSpecular = vSpecular;
    Out.vRoughness = vRoughness;
    Out.vEmissive = vEmissive;
    
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

PS_OUT PS_MAIN_NORMAL_PBR_RIM_BLOODWEAPON(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);
     
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    float depthz = 6.f;
    
    if (g_bitFlag & FLAGSPECULAR)
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGEMISSIVE)
    {
        vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGSHININESS)
    {
        vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if ((g_bitFlag & FLAGSPECULAR) && (g_bitFlag & FLAGEMISSIVE)) //일단 블러드웨폰 밝기 올리기위해서
    {
        vRoughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
        depthz = 11.f;

    }
       
    if (vRoughness.r >= 0.5f)
    {
        float3 LinearSkinColor = pow(g_vSkinTint, 2.2f);
        vMtrlDiffuse.xyz *= LinearSkinColor;
    }
    
     //림라이트
    float3 V = normalize(g_vCamPosition.xyz - In.vWolrdPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_PlayerRimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.3f, 0.f, 0.8f), float3(0.f, 0.2f, 1.f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive = rimColor;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, depthz, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR;
    Out.vSpecular = vSpecular;
    Out.vRoughness = vRoughness;
    Out.vEmissive = vEmissive;
    
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
//1 noise
//2 noise
//4 wave
//6 normal
//7 shift
//15 U
PS_OUT PS_HAIR(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);
     
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float noise1 = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 1.5f).r; //noise
    float noise2 = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord * 4.0f).r; //noise
    float wave = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord).r; //wave
    float Shift = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord).r;
    float2 PBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord).rg;
    
    //shift만들기
    float Shiftvalue = Shift + PBR.r;
    
    //noise만들기
    float NoiseMask = noise1 * noise2 * wave; // lerp(0.3f, 1.f, PBR.g); //최소치 0.3으로 안죽게

    //림노말 여기서 계산해서 DepthW에 쓴다
    //float3 smoothWorldN = In.vNormal.xyz;
    //float3 viewDir = normalize(g_vCamPosition.xyz - In.vWolrdPos.xyz);
    //float fRimFactor = 1.0 - saturate(dot(smoothWorldN, viewDir));
    
    //SRGB->Linear
    float3 LinearHairColor = pow(g_vPlayerHairColor.xyz, 2.2);
    
    //림라이트
    float4 vEmissive = DefaultEmissive;
    float3 V = normalize(g_vCamPosition.xyz - In.vWolrdPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_PlayerRimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.3f, 0.f, 0.8f), float3(0.f, 0.2f, 1.f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive = rimColor;
    Out.vDiffuse = float4(LinearHairColor, 1.0f);
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 12.f, NoiseMask);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = float4(In.vTangent.xyz * 0.5f + 0.5f, Shiftvalue); //여기에 스페큘러 계산할걸 넘겨준다
    Out.vPBR = float4(g_fPlayerMetallic, g_fPlayerRoughnessMin, 1.f, 1.f); //이거 U는 이렇게 쓰는거 아닌거 같아서 일단 값보고 던지기 Roughness 원래 0.5 적혀있음
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = vEmissive;
    
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

PS_OUT PS_EYE(PS_IN_NORMAL In) //눈전용 
{
    PS_OUT Out;
    
    float2 eyeUV = In.vTexcoord;
    
    eyeUV = (eyeUV - 0.5) * g_fPlayerEyeScale + 0.5; // 1.0=기본, >1 작아짐, <1 커짐
    
    eyeUV.x += g_fPlayerEyeOffsetU; // ImGui -0.1~0.1
    eyeUV.y += g_fPlayerEyeOffsetV; // ImGui -0.1~0.1

    //기본눈 색상 넣기
    float4 BackGround = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
    float4 EyeWhite = g_PlayerEyeWhiteTexture.Sample(LinearSampler, In.vTexcoord);
    float4 EyeDetail = g_PlayerEyeDetailTexture.Sample(LinearSampler, eyeUV);
    float4 highlight = g_PlayerEyeHighlightTexture.Sample(LinearSampler, eyeUV);

    float4 vFinalColor = BackGround * EyeWhite; //눈알 배경색 만들기

    float bMask = smoothstep(0.62, 0.74, EyeDetail.b); // 아래 자르기용
    
    //SRGB->Linear
    float3 LinearEyeColor = pow(g_vPlayerEyeBaseColor.xyz, 2.2);
    float3 LinearEyeAppendColor = pow(g_vPlayerEyeAppendColor.xyz, 2.2);
    
    float3 baseColor = LinearEyeColor * EyeDetail.r; //눈동자색 * R(밝기로사용)
    
    vFinalColor.rgb = lerp(vFinalColor.rgb, baseColor, bMask); //mask범위에서 섞는다(눈동자)

    vFinalColor.rgb = lerp(vFinalColor.rgb, LinearEyeAppendColor * EyeDetail.r, EyeDetail.g * bMask); //g범위 + mask범위 
    
    float3 eyeHighlight = LinearEyeAppendColor * highlight.r  // R: 눈동자색 하이라이트
                     + float3(1, 1, 1) * highlight.g; // G: 흰색 하이라이트
    
    vFinalColor.rgb += eyeHighlight;
    
    eyeHighlight *= g_fPlayerEyeHighlightStrength;
    
    Out.vDiffuse = vFinalColor;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 8.f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID);
    Out.vPBR = float4(0.0f, 0.8f, 0.7f, 1.0f);
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = float4(eyeHighlight, 1.f);
    Out.vSpecular = DefaultSpecular;
    
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

PS_OUT PS_FACE(PS_IN_NORMAL In) //눈 제외 얼굴전용
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);

    Out.vDiffuse = g_bitFlag;

    float2 vUvBase = (g_iPlayerUseSecondUV != 0) ? In.vTexcoord1 : In.vTexcoord; //눈썹용 UV
   
    float2 vPivot;
    float fSign;
    
    if(vUvBase.x <0.5f)
    {
        vPivot = float2(0.25f, 0.5f); //왼쪽눈 중심점
        fSign = 1.0f;
    }
    else
    {
        vPivot = float2(0.75f, 0.5f); //오른쪽눈 중심점
        fSign = -1.0f;
    }
    vUvBase -= vPivot; //원점으로 옮김
    vUvBase *= g_fPlayerBrowUVScale; //스케일적용(원점에서 스케일 올려야 제대로됨)
    
    float fA = g_fPlayerBrowUVRotation * fSign; //거울대칭으로 회전
    float2 vRotated;
    //표준 2D회전 행렬
    vRotated.x = vUvBase.x * cos(fA) - vUvBase.y * sin(fA);
    vRotated.y = vUvBase.x * sin(fA) + vUvBase.y * cos(fA);
    
    vUvBase = vRotated + vPivot; //피벗복원(원래위치로)
    vUvBase.x += g_fPlayerBrowXGap * fSign; //좌우간격
    vUvBase += g_vPlayerBrowUVOffset; //양쪽눈썹을 같은방향으로 이동
    
    //눈썹들 0.5기준으로 자기 영역만 있도록
    float fMinU, fMaxU;
    if (fSign > 0.0f) // 왼쪽 눈썹
    {
        fMinU = 0.0f;
        fMaxU = 0.5f;
    }
    else // 오른쪽 눈썹
    {
        fMinU = 0.5f;
        fMaxU = 1.0f;
    }
    
    float4 vBrowTexture = float4(0.0f, 0.0f, 0.0f, 0.0f); //없으면 이색으로
    if (vUvBase.x >= fMinU && vUvBase.x <= fMaxU && vUvBase.y >= 0.0f && vUvBase.y <= 1.0f)
    {
        vBrowTexture = g_PlayerBrowTexture.Sample(LinearClampSampler, vUvBase);
    }
    
    float fBrowMask = vBrowTexture.g;  //기본이고
    float fBrowDetail = vBrowTexture.r; //디테일
    
    float fDensity = lerp(0.7f, 1.0f, fBrowDetail); //디테일을 0.7~1사이를 t로 보간
    
    float fBrowA = saturate(fBrowMask * fDensity); //최종 (형태*결)
    
    //SRGB->Linear
    float4 vBrowColor = pow(g_vPlayerBrowColor, 2.2f); //눈썹색
    float4 vBaseColor= vMtrlDiffuse; //기본피부색(하얀색)
    
    float4 vFinalColor = lerp(vBaseColor, vBrowColor, fBrowA);
    
    float4 PBR = float4(0.0f, 0.5f, 1.0f, 1.0f); // 금속성 0, 거칠기 1 (기본값)
    float4 Emissive = DefaultEmissive;
    float4 ID = (0.f, 0.f, 0.f, 1.f);
    float EmissiveIntensity = 0.005f;
    float4 ExNormal = float4((In.vNormal).xyz * 0.5f + 0.5f, 1.f);
    float4 vSpecular = DefaultSpecular;
    float4 Roughness = DefaultRoughness;
    
    if (g_bitFlag & (1 << 2))
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
        Roughness = float4(1.f, 1.f, 1.f, 1.f); //일단 야매로 얼굴있는부분만 흰색으로

    }
    if(g_bitFlag & (1<<4))
    {
        Emissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if(g_bitFlag & (1<<6))
    {
        float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
        float3 vWorldNormal = normalize(mul(vNormalDesc, WorldMatrix));
        ExNormal = float4(vWorldNormal * 0.5f + 0.5f, 1.f); // 동일하게 [0~1] 인코딩(이거 내가했나?)
    }
    if(g_bitFlag & (1<<15))
    {
        PBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord); //ID맵 읽어옴
        float3 vOriginalColor = vFinalColor.xyz;  //원본색
        float fOriginalLum = dot(vOriginalColor, float3(0.299f, 0.587f, 0.114f)); //밝기계산
        float fBrowMask = smoothstep(0.3f, 0.6f, fOriginalLum); // 어두운 부분은 0, 밝은 부분만 1
        //눈썹, 아이라인 텍스쿠드 y축으로 나누기
        float fEyeLineG = PBR.g * step(0.45f, In.vTexcoord.y); // 아이라인만
        float fBrowG = PBR.g * step(In.vTexcoord.y, 0.45f); // 눈썹만

        //SRGB->Linear
        float3 LinearSkinTint = pow(g_vSkinTint, 2.2f);
        float3 LinearEyeLineColor = pow(g_vPlayerEyeLineColor.rgb, 2.2f);
        float3 LinearLipColor = pow(g_vPlayerLipColor.rgb, 2.2f);
        vFinalColor *= float4(LinearSkinTint, 1.f); //모든피부에 스킨적용

        vFinalColor.rgb = lerp(vFinalColor.rgb, LinearEyeLineColor, fEyeLineG * g_fEyeLineAlpha * fBrowMask); //아이라인에 적용
        vFinalColor.rgb = lerp(vFinalColor.rgb, LinearEyeLineColor, fBrowG * g_fEyeLineAlpha * fBrowMask); //눈썹에 적용     
        vFinalColor.rgb = lerp(vFinalColor.rgb, LinearLipColor, PBR.b * g_fLipAlpha * fBrowMask); //립에 적용

    }
    
    if (g_bitFlag & (1 << 7))
    {
        Roughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    float fBorder = step(0.45f, In.vTexcoord.y); // 0.45는 임시값
   
    Emissive *= EmissiveIntensity;
    
    //얼굴쪽 타투
    if (g_fPlayerTatooOnOff >= 1.f)
    {
        float2 TatooUV = In.vTexcoord1;
        TatooUV.x += -0.5f;
        TatooUV.x = -TatooUV.x;
        TatooUV.y += -0.5f;
        float invScale = 1.0 / g_fPlayerTattoScale;
        TatooUV = (TatooUV - 0.5) * invScale + 0.5;

// 오프셋도 스케일에 맞춰 보정
        TatooUV.x += g_fPlayerTattoOffsetU * invScale;
        TatooUV.y += g_fPlayerTattoOffsetV * invScale;
        float4 Tatto = g_PlayerTattoTexture.Sample(LinearClampSampler, TatooUV);
        //SGRB->Linear
        float4 LinearTattoColor = pow(Tatto, 2.2);
        vFinalColor = lerp(vFinalColor, g_vPlayerTattoColor, LinearTattoColor * g_fPlayerTattoAlpha);
    }
    
    
    Out.vDiffuse = vFinalColor; // 아이라인만 빨갛게 //vFinalColor;
    Out.vNormal = ExNormal;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 8.f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = vSpecular;
    Out.vPBR = float4(0.1, 0.5, 1.f, 1.f);
    Out.vRoughness = Roughness; 
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

PS_OUT PS_FACE_BACKUP(PS_IN_NORMAL In) //나중에 얼굴 ID씌워보고 안되면 나머지 애들 이걸로 굴리기
{
    PS_OUT Out;
    
    //g_Texture.Sample(/*어떤방식으로 샘플링할까?*/, 어디의 /*색을 얻어올까?*/);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_bitFlag & (1 << 13))
    {
        vMtrlDiffuse = g_PlayerEyelashTexture.Sample(LinearSampler, In.vTexcoord);

    }
    if (vMtrlDiffuse.a <= 0.3f)
        discard;
    

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);

    Out.vDiffuse = g_bitFlag;

    float4 PBR = float4(0.0f, 0.5f, 1.0f, 1.0f); // 금속성 0, 거칠기 1 (기본값)
    float4 Emissive = DefaultEmissive;
    float4 ID = (0.f, 0.f, 0.f, 1.f);
    float EmissiveIntensity = 0.005f;
    
    float4 ExNormal = float4((In.vNormal).xyz * 0.5f + 0.5f, 1.f);
    float4 vSpecular = DefaultSpecular;
    float4 Roughness = DefaultRoughness;
    
    if (g_bitFlag & (1 << 2))
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
        Roughness = float4(1.f, 1.f, 1.f, 1.f); //일단 야매로 얼굴있는부분만 흰색으로

    }
    if (g_bitFlag & (1 << 4))
    {
        Emissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & (1 << 6))
    {
        float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
        float3 vWorldNormal = normalize(mul(vNormalDesc, WorldMatrix));
        ExNormal = float4(vWorldNormal * 0.5f + 0.5f, 1.f); // 동일하게 [0~1] 인코딩(이거 내가했나?)
    }
    if (g_bitFlag & (1 << 15))
    {
        PBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord); //이걸로 받아와서
    }
    if (g_bitFlag & (1 << 7))
    {
        Roughness = g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    Emissive *= EmissiveIntensity;
    
    
    Out.vDiffuse = vMtrlDiffuse; //vMtrlDiffuse
    Out.vNormal = ExNormal;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 8.f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = vSpecular;
    Out.vPBR = float4(0.1, 0.5, 1.f, 1.f);
    Out.vRoughness = Roughness;
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
