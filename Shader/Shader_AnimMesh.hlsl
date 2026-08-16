// Shader_AnimMesh.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_NormalTexture : register(t1);
Texture2D<float4> g_UtilityTexture : register(t2);
Texture2D<float4> g_OpacityTexture : register(t3);
Texture2D<float4> g_MetalnessTexture : register(t4);
Texture2D<float4> g_SpecularTexture : register(t5);
Texture2D<float4> g_EmissiveTexture : register(t10);
Texture2D<float4> g_RoughTexture : register(t11);
Texture2D<float4> g_NoiseTexture : register(t12);
Texture2D<float4> g_RimNoiseTexture : register(t13);

struct BoneMatrix
{
    row_major float4x4 mat;
};

StructuredBuffer<BoneMatrix> g_CombinedMatrix : register(t20);
StructuredBuffer<BoneMatrix> g_OffsetMatrix : register(t21);
StructuredBuffer<uint> g_RemapBone : register(t22);

float4x4 SkinMatrix(uint iLocalSlot)
{
    uint iMyIndex = g_RemapBone[iLocalSlot]; //이게이제 자기본 인덱스 기준으로
    
    return mul(g_OffsetMatrix[iMyIndex].mat, g_CombinedMatrix[iMyIndex].mat);
    
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHT;
    float2 vTexcoord1 : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPositionShadow : TEXCOORD3;
    float2 vTexcoord1 : TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    //float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z));
    
    float4x4 BoneMatrix = (SkinMatrix(In.vBlendIndices.x) * In.vBlendWeights.x)
                        + (SkinMatrix(In.vBlendIndices.y) * In.vBlendWeights.y)
                        + (SkinMatrix(In.vBlendIndices.z) * In.vBlendWeights.z)
                        + (SkinMatrix(In.vBlendIndices.w) * In.vBlendWeights.w);
    
    //float4 vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    //
    //float4x4 matWV, matWVP;
    //matWV = mul(g_WorldMatrix, g_ViewMatrix);
    //matWVP = mul(matWV, g_ProjMatrix);
    //
    //Out.vPosition = mul(vPosition, matWVP);
    //Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    //Out.vTexcoord = In.vTexcoord;
    //Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    float4 Position = mul(float4(In.vPosition, 1.f), BoneMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    Out.vWorldPos = mul(float4(Position.xyz, 1.f), g_WorldMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPositionShadow = Position;
    Out.vPosition = mul(Position, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vTexcoord1 = In.vTexcoord1;
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct VS_OUT_NORMAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vLocalPosition : TEXCOORD3;
    float2 vTexcoord1 : TEXCOORD4;
};

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
    VS_OUT_NORMAL Out;
    float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z));
    
    float4x4 BoneMatrix = (SkinMatrix(In.vBlendIndices.x) * In.vBlendWeights.x)
                        + (SkinMatrix(In.vBlendIndices.y) * In.vBlendWeights.y)
                        + (SkinMatrix(In.vBlendIndices.z) * In.vBlendWeights.z)
                        + (SkinMatrix(In.vBlendIndices.w) * fWeightW);
    
    float4 Position = mul(float4(In.vPosition, 1.f), BoneMatrix);
    float3 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix).xyz;
    float3 vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix).xyz;
    float3 vBinormal = mul(float4(In.vBinormal, 0.f), BoneMatrix).xyz;
    Out.vLocalPosition = Position;
    Out.vWorldPos = mul(float4(Position.xyz, 1.f), g_WorldMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(Position, matWVP);
    Out.vNormal = normalize(mul(vNormal, (float3x3) g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(vTangent, (float3x3) g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(vBinormal, (float3x3) g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vTexcoord1 = In.vTexcoord1;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW_CASCADE(VS_IN In)
{
    VS_OUT_SHADOW Out;
    float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z));
    
    float4x4 BoneMatrix = (SkinMatrix(In.vBlendIndices.x) * In.vBlendWeights.x)
                        + (SkinMatrix(In.vBlendIndices.y) * In.vBlendWeights.y)
                        + (SkinMatrix(In.vBlendIndices.z) * In.vBlendWeights.z)
                        + (SkinMatrix(In.vBlendIndices.w) * fWeightW);
    
    
    float4 Position = mul(float4(In.vPosition, 1.f), BoneMatrix);

    matrix matShadow = mul(g_WorldMatrix, g_ShadowCascade_ViewProjMatrix);

    Out.vPosition = mul(Position, matShadow);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vPositionShadow : TEXCOORD3;
    float2 vTexcoord1 : TEXCOORD4;
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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((In.vNormal).xyz * 0.5f + 0.5f, 1.f);
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.0f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = float4(0.f, 0.6, 1.f, 1.f); //임시로 번들거림제거
    Out.vSpecular = DefaultSpecular;
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = DefaultEmissive;
    
    return Out;
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

struct PS_OUT_SHADOW_CASCADE
{
    float vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out;
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 1.f);
    //float depth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);;
    //float vis = 1.0 - depth; // 반전
    //Out.vDepth = float4(vis, vis, 0.f, 1.0);

    return Out;
}

void PS_MAIN_CASCADE_SHADOW(PS_IN_SHADOW In)
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
    
    //return Out;
}

struct PS_IN_NORMAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vLocalPosition : TEXCOORD3;
    float2 vTexcoord1 : TEXCOORD4;
};

PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 Utility = g_UtilityTexture.Sample(LinearSampler, In.vTexcoord);
       
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.0f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = float4(0.f, 0.6, 1.f, 1.f); //임시로 번들거림제거
    Out.vSpecular = DefaultSpecular;
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = DefaultEmissive;
    
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse;

    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    
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
        vRoughness = g_RoughTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    float4 Utility = g_UtilityTexture.Sample(LinearSampler, In.vTexcoord);
       
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.0f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR; //임시로 번들거림제거
    Out.vSpecular = vSpecular;
    Out.vRoughness = vRoughness;
    Out.vEmissive = vEmissive;
    
    return Out;
}

PS_OUT PS_NPC_HAIR(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1, In.vNormal.xyz);
     
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float noise1 = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 1.5f).r; //noise
    float noise2 = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord * 4.0f).r; //noise
    float wave = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord).r; //wave
    float Shift = g_RoughTexture.Sample(LinearSampler, In.vTexcoord).r;
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
    //float3 LinearHairColor = pow(float4(0.208, 0.092, 0.04f, 1.f), 2.2);
    
    Out.vDiffuse = float4(0.208, 0.092, 0.04f, 1.f);
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 12.f, NoiseMask);
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //월드 xyz 주소를 주고 w값을 1로준다(위치라서 w값 1)
    Out.vSpecular = float4(In.vTangent.xyz * 0.5f + 0.5f, Shiftvalue); //여기에 스페큘러 계산할걸 넘겨준다
    Out.vPBR = float4(g_fPlayerMetallic, g_fPlayerRoughnessMin, 1.f, 1.f); //이거 U는 이렇게 쓰는거 아닌거 같아서 일단 값보고 던지기 Roughness 원래 0.5 적혀있음
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = DefaultEmissive;
    
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

PS_OUT PS_MAIN_NORMAL_PBR_DISSOLVE(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse;

    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    
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
        vRoughness = g_RoughTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    float4 Utility = g_UtilityTexture.Sample(LinearSampler, In.vTexcoord);
       
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 18.f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR; //임시로 번들거림제거
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
        Out.vEmissive.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
    }
    
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR_DISSOLVE_MONSTER(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse;

    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    float3 vId = DefaultEmissive;
    IdMasks tIdMasks;
    
    if (g_bitFlag & FLAGSPECULAR)
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGOPACITY)
    {
        vId = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord).rgb;
        tIdMasks = DecodeIdMap6(vId);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGEMISSIVE)
    {
        //게임에서 쓸거 0.1, 0.2(SlaveDevil) 0, 0.2(Vampire), 0.13, -0.1(Slime)
        float2 EyeUV = In.vTexcoord;
        float invScale = 1.0;
        EyeUV = (EyeUV - 0.5) * invScale + 0.5;

        EyeUV.x += g_vEmissionUVPos.x * invScale;
        EyeUV.y += g_vEmissionUVPos.y * invScale;
        if (!tIdMasks.fAny)
            vEmissive = g_EmissiveTexture.Sample(LinearClampSampler, EyeUV) * 2.f;
        
        //float2 EyeUV = In.vTexcoord;
        //float invScale = 1.0;
        //EyeUV = (EyeUV - 0.5) * invScale + 0.5;

        //EyeUV.x += g_fPlayerTattoOffsetU * invScale;
        //EyeUV.y += g_fPlayerTattoOffsetV * invScale;
        //if ((vMtrlDiffuse.r >= g_fPlayerEyeScale.r) && (vMtrlDiffuse.g >= g_fPlayerEyeOffsetU) && (vMtrlDiffuse.b <= g_fPlayerEyeOffsetV))
        //    vEmissive = g_EmissiveTexture.Sample(LinearClampSampler, EyeUV);
    }
    if (g_bitFlag & FLAGSHININESS)
    {
        vRoughness = g_RoughTexture.Sample(LinearSampler, In.vTexcoord);
    }
    float4 Utility = g_UtilityTexture.Sample(LinearSampler, In.vTexcoord);
    
     //림라이트
    float3 V = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_RimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.8f, 0.1f, 0.0f), float3(1.0f, 0.3f, 0.1f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive += rimColor;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 18.f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR; //임시로 번들거림제거
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
        Out.vEmissive.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
    }
    
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR_DISSOLVE_GIANTVAMPIRE(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse;

    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    
    if (g_bitFlag & FLAGSPECULAR)
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGEMISSIVE)
    {
        float2 EyeUV = In.vTexcoord1;
        float invScale = 1.0;
        EyeUV = (EyeUV - 0.5f) * invScale + 0.5f;

        EyeUV.x += 0.61f * invScale; //0.13 //슬라임
        EyeUV.y += 0.19f * invScale; //-0.1

        if (vMtrlDiffuse.r >= 0.502f && vMtrlDiffuse.g <= 0.358f && vMtrlDiffuse.b <= 1.f)
            vEmissive = g_EmissiveTexture.Sample(LinearClampSampler, EyeUV) * 2.f;
    }
    if (g_bitFlag & FLAGSHININESS)
    {
        vRoughness = g_RoughTexture.Sample(LinearSampler, In.vTexcoord);
    }
    float4 Utility = g_UtilityTexture.Sample(LinearSampler, In.vTexcoord);
       
    //림라이트
    float3 V = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_RimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.8f, 0.1f, 0.0f), float3(1.0f, 0.3f, 0.1f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive += rimColor;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 18.f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR; //임시로 번들거림제거
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
        Out.vEmissive.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
    }
    
    return Out;
}

PS_OUT PS_MAIN_NORMAL_PBR_DISSOLVE_GIANTWHITEDEVIL(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse;

    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    float3 vId = DefaultEmissive;
    IdMasks tIdMasks;
    
    if (g_bitFlag & FLAGSPECULAR)
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGOPACITY)
    {
        vId = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord).rgb;
        tIdMasks = DecodeIdMap6(vId);
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGEMISSIVE)
    {
        float2 EyeUV = In.vTexcoord;
        float invScale = 1.f;
        EyeUV = (EyeUV - 0.5f) * invScale + 0.5f;

        EyeUV.x += 0.f * invScale;
        EyeUV.y += 0.f * invScale;
        if ((vMtrlDiffuse.r >= 0.436f) && (vMtrlDiffuse.g >= 0.f) && (vMtrlDiffuse.b <= 0.36f))
            vEmissive = g_EmissiveTexture.Sample(LinearClampSampler, EyeUV) * 2.f;
    }
    if (g_bitFlag & FLAGSHININESS)
    {
        vRoughness = g_RoughTexture.Sample(LinearSampler, In.vTexcoord);
    }
    float4 Utility = g_UtilityTexture.Sample(LinearSampler, In.vTexcoord);
    
     //림라이트
    float3 V = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_RimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.8f, 0.1f, 0.0f), float3(1.0f, 0.3f, 0.1f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive += rimColor;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 18.f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR; //임시로 번들거림제거
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
        Out.vEmissive.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
    }
    
    return Out;
}

PS_OUT PS_MAIN_SAVEPOINT(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 Emissive = DefaultEmissive;
    float4 Opacity = float4(1.f, 0.f, 0.f, 0.f); //나중에 올리버 Roughness들어갈거 생각해서(savePoint Roughtness 이걸로사용)
    
    float4 color1 = float4(0.430f, 0.635f, 0.567f, 1.0f); // #6EA291
    float4 color2 = float4(0.306f, 0.323f, 0.258f, 1.0f); // #4E5242
    
    //이거 나중에 다른애들이 emissive나 Opacity쓰면 savepoint패스 따로 분리해야함
    float EmissiveBottom;
    float EmissiveTop;
    if (g_bitFlag & (1 << 4))
    {
        EmissiveBottom = -0.24f;
        EmissiveTop = 0.7f;
        Emissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
        Opacity = Emissive;
        Emissive *= 2.f;
        vMtrlDiffuse = float4(lerp(vMtrlDiffuse.rgb, vMtrlDiffuse.rgb * color1.rgb, 0.3f), vMtrlDiffuse.w);
    }
        
    if (g_bitFlag & (1 << 7)) //여기서 색 섞는다
    {
        EmissiveBottom = 0.1f; //0.3
        EmissiveTop = 0.7f; //1.f
        Emissive = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);
        Opacity = Emissive;
        vMtrlDiffuse = float4(lerp(vMtrlDiffuse.rgb * color1.rgb, vMtrlDiffuse.rgb * color2.rgb, Emissive.xyz), vMtrlDiffuse.w);
        Emissive *= 5.f;

    }
    float heightMaskBody = saturate((In.vLocalPosition.y - EmissiveBottom) / (EmissiveTop - EmissiveBottom));
    Emissive = Emissive * heightMaskBody;

    float4 Utility = g_UtilityTexture.Sample(LinearSampler, In.vTexcoord);
       
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.0f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = float4(0.f, 0.6, vMtrlDiffuse.r, 2.f); //임시로 번들거림제거
    Out.vSpecular = DefaultSpecular;
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = Emissive;
    
    return Out;
}

struct PS_OUT_PBR
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPick : SV_TARGET3;
    float4 vPBR : SV_TARGET4;
};

PS_OUT_PBR PS_MAIN_NORMAL_TOON(PS_IN_NORMAL In)
{
    PS_OUT_PBR Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 fRough = g_RoughTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.0f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, shiness
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = fRough;
    
    return Out;
}

PS_OUT PS_CHARACTER_EYE(PS_IN_NORMAL In)
{
    PS_OUT Out;

    float2 baseUV = In.vTexcoord;
    float2 lightUV = In.vTexcoord;
    
    if (g_bitFlag & (1 << 0))
    {
        lightUV.x = 1.0f - lightUV.x;
    }
    
    float4 colorC = g_DiffuseTexture.Sample(LinearSampler, baseUV);
    float4 colorW = g_OpacityTexture.Sample(LinearSampler, baseUV);
    float4 colorH = g_EmissiveTexture.Sample(LinearSampler, lightUV);
    float4 colorS = g_SpecularTexture.Sample(LinearSampler, lightUV);
    
    float3 mix1 = lerp(colorW.rgb, colorC.rgb, colorC.a);
    
    float3 finalBaseColor = lerp(mix1, colorH.rgb, colorH.a);
    
    //float3 finalEmission = finalBaseColor + colorS.rgb;
    
    Out.vDiffuse = float4(finalBaseColor, 1.0f);
    //Out.vEmissive = float4(finalEmission, 1.0f);
    Out.vEmissive = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 1.0f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID);
    Out.vPBR = float4(0.0f, 0.5f, 1.0f, 1.0f);
    Out.vRoughness = DefaultRoughness;
    //Out.vSpecular = DefaultSpecular;
    Out.vSpecular = colorS;
    
    return Out;
}


PS_OUT PS_MAIN_NORMAL_PBR_CV(PS_IN_NORMAL In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);;

    if (vMtrlDiffuse.a <= 0.3f)
        discard;

    float3 vNormalDesc = DecodeNormalBC5(g_NormalTexture, LinearSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz, 0.f);
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = float4(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    float4 vPBR = DefaultPBR;
    float4 vSpecular = DefaultSpecular;
    float4 vRoughness = DefaultRoughness;
    float4 vEmissive = DefaultEmissive;
    float3 vOpacity = DefaultEmissive;
    
    if (g_bitFlag & FLAGSHININESS)
    {
        vRoughness = g_RoughTexture.Sample(LinearSampler, In.vTexcoord);
        if (vRoughness.r <= 0.3f)
            discard;
    }
    if (g_bitFlag & FLAGSPECULAR)
    {
        vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGOPACITY)
    {
        vOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    }
    if (g_bitFlag & FLAGMETALNESS)
    {
        vPBR = g_MetalnessTexture.Sample(LinearSampler, In.vTexcoord);
    }
    if (g_bitFlag & FLAGEMISSIVE)
    {
        float2 EyeUV = In.vTexcoord;
        float invScale = 1.0 / 0.182;
        EyeUV = (EyeUV - 0.5) * invScale + 0.5;

        EyeUV.x += -0.38 * invScale; //0.13 //슬라임
        EyeUV.y += -0.47 * invScale; //-0.1

        vEmissive = g_EmissiveTexture.Sample(LinearClampSampler, EyeUV);
        if (g_fTime > 0.f)
            vEmissive *= float4(1.f, 0.f, 0.f, 1.f) * 2.f; //특정조건되면 눈 붉어지게
    }
       
    //림라이트
    float3 V = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float rim = 1 - saturate(dot(vNormal.xyz, V));
    rim = pow(rim, g_fRimIntensity);
    
    float2 panUV = In.vTexcoord + float2(0.f, g_fTime * 0.1f);
    float mask = g_RimNoiseTexture.Sample(LinearSampler, panUV).r;
    
    float3 rimTint = lerp(float3(0.8f, 0.1f, 0.0f), float3(1.0f, 0.3f, 0.1f), rim);

    float4 rimColor = float4(rim * rimTint * g_fRimPower * mask, 1.f);
    vEmissive += rimColor;
    
    Out.vDiffuse = vMtrlDiffuse; 
    Out.vNormal = float4((vNormal).xyz * 0.5f + 0.5f, 1.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 18.f, 50.f); //물체의 깊이값, w나누기값, ambient강하기, 샤프니스(크게줄수록specular줄어듬)
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID); //물체의 월드위치, 물체의 식별번호
    Out.vPBR = vPBR; //임시로 번들거림제거
    Out.vSpecular = vSpecular; //vSpecular;
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
        Out.vEmissive.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
    }
    
    return Out;
}