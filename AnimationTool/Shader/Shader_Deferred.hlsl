#pragma pack_matrix(row_major)
#include "Shader_Buffer.hlsli"

Texture2D g_DebugTexture : register(t0);

Texture2D g_TextureDiffuse : register(t1);
Texture2D g_TextureNormal : register(t2);
Texture2D g_TextureShade : register(t3);
Texture2D g_TextureDepth : register(t4);
Texture2D g_TextureSpecular : register(t5);
Texture2D g_TextureShadow1 : register(t6);
Texture2D g_TexturePBR : register(t7);
Texture2D g_TextureShadow2 : register(t8);
Texture2D g_TextureShadow3 : register(t9);
Texture2D g_TextureShadow4 : register(t10);
Texture2D g_TextureHDR : register(t11);
Texture2D g_TextureBright : register(t12);
Texture2D g_TextureSampling : register(t13); //이거 이름 재사용
Texture2D g_TextureMtrlSpecular : register(t14); //이거 이름 재사용
Texture2D g_TextureRoughness : register(t15); //이거 이름 재사용
Texture2D g_TextureEmissive : register(t16); 
Texture2D g_TextureSSAO : register(t17);
Texture2D g_TextureSSAOBLUR : register(t18);
Texture2D g_TextureBakeShadow : register(t19);
Texture2D g_TexturePick : register(t20);
Texture2D g_TextureGodRayCopy : register(t21);
Texture2D g_TextureGodRay : register(t22);
Texture2D g_TextureCamVelocity : register(t23);
Texture2D g_TextureCamMotionBlur : register(t24);
Texture2D g_TextureSamplingAdd : register(t25);
Texture2D g_TextureEffectEmissive : register(t26);

SamplerState LinearSampler : register(s0);
SamplerState LinearClampSampler : register(s2);
SamplerComparisonState ShadowSampler : register(s4);
SamplerState PointWrapSampler : register(s6);

// 이렇게 static const로 선언하면 버퍼 슬롯을 차지하지 않음
static const float4 g_vMtrlAmbient = { 1.f, 1.f, 1.f, 1.f };
static const float4 g_vMtrlSpecular = { 1.f, 1.f, 1.f, 1.f };

static const float2 poissonDisk[16] =
{
    float2(-0.94201624, -0.39906216),
    float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870),
    float2(0.34495938, 0.29387760),
    float2(-0.91588581, 0.45771432),
    float2(-0.81544232, -0.87912464),
    float2(-0.38277543, 0.27676845),
    float2(0.97484398, 0.75648379),
    float2(0.44323325, -0.97511554),
    float2(0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),
    float2(0.79197514, 0.19090188),
    float2(-0.24188840, 0.99706507),
    float2(-0.81409955, 0.91437590),
    float2(0.19984126, 0.78641367),
    float2(0.14383161, -0.14100790)
};

float3 ReconstructWorldPos(float2 _texcoord, float _ndcZ, float _viewZ)
{
    float4 pos;
    pos.x = _texcoord.x * 2.f - 1.f;
    pos.y = _texcoord.y * -2.f + 1.f;
    pos.z = _ndcZ;
    pos.w = 1.f;
    pos *= _viewZ;
    pos = mul(pos, g_InverseProjMatrix);
    pos = mul(pos, g_InverseViewMatrix);
    return pos.xyz;
}

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

struct PS_ONLY_BIND_STRUCT
{
    float4 vColor : SV_TARGET0;
};

PS_ONLY_BIND_STRUCT PS_BIND_ONLY(VS_OUT In)
{
    PS_ONLY_BIND_STRUCT Out;
    
    Out.vColor = float4(1.f, 1.f, 1.f, 1.f);
    
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

    //Out.vColor = In.vPosition;
    Out.vColor = g_DebugTexture.Sample(LinearSampler, In.vTexcoord);
   
    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

//빛계산해준다
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

    float4 vNormalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);

    
    float fSpecularStrength = vNormalDesc.w;
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    /* 0~1 -> -1~1 */
    float4 vNormal = float4((vNormalDesc.xyz * 2.f - 1.f), 0.f);
    vNormal = normalize(vNormal);
    float AmbientStrength = vDepthDesc.z;
    
    //ambient 더하고 diffuse 곱해준다
    float4 vShade = max(dot(normalize(g_vLightDirection) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient * AmbientStrength);

    Out.vShade = (g_vLightDiffuse * saturate(vShade));

    float fViewZ = vDepthDesc.y;

    float4 vWorldPos;
    //TexCorrd를 NDC 좌표로 바꿔준다

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

    //투영행렬 역나누기
    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    //뷰 역행렬 나누기
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);

    //이제 월드 좌표로 바꼈음(카메라에서 픽셀보는 방향벡터)
    float4 Specular = float4(1.f, 1.f, 1.f, 1.f);
    float4 vLook = vWorldPos - g_vCamPosition;
    
    //퐁조명방식(Blinn-Phong 변형)
    //샤이니스가 0보다 작으면 스페큘러는 0
    float fShininess = vDepthDesc.w;
    //리플렉션구하기(빛과 지형을 사용해서 반사되는 빛)
    float4 vReflect = reflect(normalize(g_vLightDirection), normalize(vNormal));
    Out.vSpecular = (g_vMtrlSpecular * g_vLightSpecular * fSpecularStrength) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), fShininess);
    
    if(fShininess <= 0.f)
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);

    return Out;
}

float KajiyaKaySpec(float3 tangent, float3 halfVec, float power)
{
    //tangent를 normal 방향으로 shift
    float TdotH = dot(tangent, halfVec);
    //sin 기반 스펙큘러 (가닥을 따라 띠 형태로 맺힘)
    float sinTH = sqrt(1.0 - TdotH * TdotH);
    return pow(saturate(sinTH), power);
}

float3 ShiftTangent(float3 tangent, float3 normal, float shiftAmount)
{
    return normalize(tangent + normal * shiftAmount);
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL_PBR(PS_IN In)
{
    PS_OUT_LIGHT Out;
    Out.vShade = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Out.vSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //Depth
    // [PBR] 배경 픽셀은 라이팅 안 함
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    if (vDepthDesc.y <= 0.001f || vDepthDesc.z == 2.f)
        return Out;

    //Normal
    float4 vNormalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);
    // 0~1 -> -1~1
    float3 N = vNormalDesc.xyz * 2.f - 1.f;
    N = normalize(N);
    
    //specular
    float4 vSpecularDesc = g_TextureMtrlSpecular.Sample(LinearSampler, In.vTexcoord);
    float3 T;
    float Noise;
    float Shift;
    if (vDepthDesc.z == 12.f)//머리라면 기본으로 돌리기
    {
        
        Shift = vSpecularDesc.w;
        Noise = vDepthDesc.w;
        T = vSpecularDesc.xyz * 2.f - 1.f;
        T = normalize(T);
        
        vSpecularDesc = DefaultSpecular;
    }
        
    
    float4 SSSCheck = g_TextureRoughness.Sample(LinearSampler, In.vTexcoord);
    
    //PBR
    float4 PBR = g_TexturePBR.Sample(LinearSampler, In.vTexcoord);
    
    //b가 SSS, Metal, r AO공통, g를 러프니스로 썼었음
    //g가 AO r은? b SSS 스무스니스 반전해서 러프니스

    //피부용 SSS? 옷은 메탈릭이래
    float Metalic = 0;
    float SSS = 0;
    float roughness = 0;
    if (SSSCheck.r >= 0.5f)
    {
        SSS = PBR.r;
        roughness = max(PBR.g, g_fSkinRoughnessMin); //피부면 높여서 플라스틱 느낌 줄이기
    }
    else
    {
        Metalic = PBR.r; //r
        roughness = max(PBR.g, g_fMtrlRoughnessMin); //일반이면 맨들맨들하게
    }
    float AO = PBR.b; //g

   
    //diffuse
    float4 vDiffuseDesc = g_TextureDiffuse.Sample(LinearSampler, In.vTexcoord);
    float3 Albedo = vDiffuseDesc.xyz;
        
    float fViewZ = vDepthDesc.y;
    float4 vWorldPos;
    //TexCorrd를 NDC 좌표로 바꿔준다

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

    //투영행렬 역나누기
    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    //뷰 역행렬 나누기
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);

    //노말기반으로 거리별 스페큘러 뭉개기(스페큘러 문제 아닌듯)
    //float fDistFromCam = length(vWorldPos.xyz - g_vCamPosition.xyz);
    //float fDistRoughBias = saturate((fDistFromCam - g_fFogStartDist) / (g_fFogEndDist - g_fFogStartDist));
    //roughness = saturate(roughness + fDistRoughBias * g_fHeightFogIntensity);
    float alpha = roughness * roughness; //GGX사용
    
    //이제 월드 좌표로 바꼈음(카메라에서 픽셀보는 방향벡터)
    //뷰, 빛방향, 하프벡터
    float3 V = normalize(g_vCamPosition.xyz - vWorldPos.xyz); //카메라 방향
    float3 L = normalize(-g_vLightDirection.xyz); // +에서 -로 고침 빛방향
    float3 H = normalize(V + L); //하프벡터
    
    float3 specular;
    float3 diffuse_factor;
    float NdotL = saturate(dot(N, L)); // +  + 0.1f
    float3 F0;
    //머리면 speuclar 다르게 계산
    if (vDepthDesc.z == 12.f) 
    {
        float hairRoughness = max(PBR.g, 0.1f);
        float basePower = lerp(g_fPlayerHighlightPower * 2.0, g_fPlayerHighlightPower * 0.3, hairRoughness);
        
        float3 t1 = ShiftTangent(T, N, Shift + g_fPlayerHighlight1Shift);
        float3 t2 = ShiftTangent(T, N, Shift + g_fPlayerHighlight2Shift);
        
        float spec1 = KajiyaKaySpec(t1, H, basePower);
        float spec2 = KajiyaKaySpec(t2, H, basePower);

        
        float3 specColor1 = float3(1, 1, 1);
        float3 specColor2 = max(Albedo, 0.15); //최소값 보장으로 검정색에서도 보이게

        specular = spec1 * g_fPlayerHighlight1Strength * lerp(0.7, 1.0, Noise)
                + spec2 * g_fPlayerHighlight2Strength * specColor2 * lerp(0.7, 1.0, Noise);
        
        diffuse_factor = (1.0f - Metalic) / 3.14f;
    }
    //일반물체 스페큘러 계산
    else
    {
        //프레넬 계산
        float3 specLuma = dot(vSpecularDesc.rgb, float3(0.2126, 0.7152, 0.0722)); // 그레이스케일화(밝기만 추출 공식임)
        float3 baseF0 = clamp(specLuma, 0.02f, 0.08f); // 비금속 F0 범위로 제한(0.02~0.08범위로)
        F0 = lerp(baseF0, Albedo, Metalic); //비금속이면 위의값 쓰고 메탈이면 Albedo색상 baseF0

        //DistributionGGX(미세면분포)
        float alpha2 = alpha * alpha;
        float NdotH = saturate(dot(N, H));
        float denomD = max((NdotH * NdotH * (alpha2 - 1.0f) + 1.0f), 0.00001f); // lobe 넓이 0.05f
        float D = alpha2 / (denomD * denomD * 3.14f);
    
        //GeometrySmith(미세면자기가림)
        float NdotV = saturate(dot(N, V));
        //NdotL위치였음
    
        float k = (roughness + 1.0f) * (roughness + 1.0f) / g_fKValue; // direct light용
        float G_V = NdotV / (NdotV * (1.0f - k) + k + 0.00001);
        float G_L = NdotL / (NdotL * (1.0f - k) + k + 0.00001);
        float G = G_V * G_L;
    
        //FresnelSchlick(시선에 따라 반사율)
        float3 F = F0 + (1.0f - F0) * pow(1.0 - saturate(dot(H, V)), 5.0f); //specular영향 5.f값이랑
    
        //Cook-Torrance Specular
        float3 numerator = D * G * F;
        float denom2 = 4.0 * max(NdotV * NdotL, 0.01f);
        //float specFade = smoothstep(0.0f, 0.15f, NdotV); //에지에서 부드럽게 감쇠
        specular = (numerator / denom2); // * specFade;
        //specular *= 0.45f;
        //diffuse(최종에 diffus해주기 때문에 뺌)
        float3 kD = (1.0f - F) * (1.0f - Metalic);
        diffuse_factor = max(kD, g_fMinDiffuse) / 3.14f; //3.14빼서 밝기 확보(툰) //이거 다시 넣어보기
    }
    
    //Direct Light
        float3 lightColor = g_vLightDiffuse.rgb * g_fLightMultiplier;
    // NdotL에 약간의 soft bias (그림자 너무 깊지 않게, 어깨처럼 부드럽게)
    
    //툰이라서 추가
    //float NdotL_soft = saturate(dot(N, L) * 1.2f + 0.05f); // 곡률 보정 + 약한 bias
    // 기존 NdotL을 부드러운 2단계로
    float3 direct;
    float mtrNdotL;
    if(vDepthDesc.z == 12.f)
    {
        mtrNdotL = saturate(dot(N, L) * 0.5 + 0.5); // half-lambert
        direct = lightColor * mtrNdotL;
    }
    else
    {
        mtrNdotL = smoothstep(g_vToonShadowRange.x, g_vToonShadowRange.y, NdotL);
        direct = lightColor * lerp(g_vToonBrightnessRange.x, g_vToonBrightnessRange.y, mtrNdotL); //밝기?
    }
    //float3 direct = lightColor * NdotL_soft;
    
    //SSS적용
    float backLight = pow(saturate(dot(-N, L)), g_fSSSPower); // // 뒤에서 오는 빛
    float3 sssContrib = backLight * SSS * g_fSSSIntensity * lightColor * g_vSSSColor; // // SSS 기여
    
    //Ambient
   
    //뒷면보정 어두울수록 ambient 추가해줌
    //float backFaceBrightness = 1.0f - toonNdotL; // 뒷면일수록 1에 가까움
    //float ambientBoost = lerp(0.8f, 1.2f, backFaceBrightness); // 뒷면이면 ambient 3배

    //Ambient + AO
    float3 ambientBase = g_vLightAmbient.rgb; // float3(0.3f, 0.3f, 0.3f);
    float3 ambient = ambientBase * g_fAmbientStrength * AO; //* ambientBoost;
    

    //최소Ambient보장
    float3 ambientFloor = max(Albedo * g_vAmbientFloorMin.x, g_vAmbientFloorMin.y); // Albedo가 어두워도 최소 보장
    ambient = max(ambient, ambientFloor);
    
    //옷 뒷면 보존용
    float backFill = (1.0f - mtrNdotL) * g_fBackFillStrength; // 뒷면일수록 최대 추가
    ambient += backFill;
    //ambient = max(ambient, float3(0.08f, 0.08f, 0.08f));
    
    // 림라이트 (SSS 이후, Shade 출력 전에 추가) - 툰이라서 추가2
    //float3 smoothN = normalize(N + V * 0.3f); // [EDITED] 시선 방향으로 살짝 당겨서 곡면 부드럽게
    //float rim = 1.0f - saturate(dot(smoothN, V));
    //rim = smoothstep(0.55f, 0.75f, rim); // [EDITED] 고정 범위로 깔끔하게 자르기, g_fRimPower 대신
    //float rimMask = smoothstep(g_vRimMaskRange.x, g_vRimMaskRange.y, NdotL);
    //float3 rimColor = lightColor * g_fRimIntensity;
    //float3 rimContrib = rim * rimColor; / rimMask;
    
    //메탈 diffuse 0인거 방지
    float metalShadow = mtrNdotL; //밝은곳에서는 적용 덜되게 1.0 - 
    float metalMask = smoothstep(0.3f, 0.7f, Metalic);
    float MetalAmbientIntesity = 1.f;
    if (vDepthDesc.z == 11.f) //블러드웨폰만 임시로 3올리기
        MetalAmbientIntesity = 1.f;
    float3 metalAmbient = metalMask * Albedo * ambient * MetalAmbientIntesity * metalShadow;
    
    //야매 환경광
    float3 R = reflect(-V, N);
    float3 fakeEnv = lerp(float3(0.05f, 0.05f, 0.08f), float3(0.3f, 0.3f, 0.35f), saturate(R.y * 0.5f + 0.5f));
    // 위쪽은 밝고 아래쪽은 어둡게 (하늘-바닥 근사)

    float envRough = roughness * roughness;
    fakeEnv *= (1.0f - envRough); // 러프하면 반사 약하게

    float3 envSpecular = fakeEnv * F0 * metalMask;
    
    //피부
    if (SSSCheck.r >= 0.5f)     
    { // 최종 Shade = direct + ambient diffusefactor* 색감위해서 뺌
        Out.vShade.rgb = diffuse_factor * (direct + ambient) + sssContrib + envSpecular; //+ rimContrib; * g_vSkinTint
    }
    else if(vDepthDesc.z == 6.f || vDepthDesc.z == 12.f)
    {
        Out.vShade.rgb = diffuse_factor * (direct + ambient) + metalAmbient + envSpecular;
    }
    //나머지
    else
    {
        Out.vShade.rgb = diffuse_factor * (direct + ambient) + metalAmbient + envSpecular; //+ rimContrib; //+sssContrib + rimContrib;
    }

    Out.vShade.a = 1.0f;

    // Specular 별도 출력 (은은하게)
    //float shadeFade = smoothstep(0.0f, 0.7f, toonNdotL); // 뒷면(어두운 쪽)이면 0으로
    float shadeFade = pow(smoothstep(0.0f, 0.7f, mtrNdotL), 2.0f); // 제곱으로 더 급격하게
    float specBoost = lerp(g_vSpecBoostRange.x, g_vSpecBoostRange.y, roughness); //강한부분강하게 약한부분 약하게//(맵핑심무성)
    float specBostMap = lerp(g_vSpecBosstMapRange.x, g_vSpecBosstMapRange.y, roughness); //강한부분강하게 약한부분 약하게//(맵핑심무성)
    float specBoostMonster = lerp(g_vSpecMonsterBoostRange.x, g_vSpecMonsterBoostRange.y, roughness); //강한부분강하게 약한부분 약하게//(맵핑심무성)
    float UnderSpecZero = g_fUnderMapZeroDirSpecular;
    
    if (vDepthDesc.z == 12.f) //머리면 speuclar 다르게 계산
    {
        float hairShadeFade = smoothstep(0.0, 0.3, NdotL);
        Out.vSpecular = float4(specular * lightColor * g_fSpecularIntensity * hairShadeFade * UnderSpecZero, 1.0f);
    }
    else if (vDepthDesc.z == 5.f) //맵, 무기 sepuclar
    {
        Out.vSpecular = float4(specular * lightColor * NdotL * g_fSpecularMapIntensity * specBostMap * shadeFade * UnderSpecZero, 1.0f);
    }
    else if (vDepthDesc.z == 18.f) //몬스터
    {
        Out.vSpecular = float4(specular * lightColor * NdotL * g_fMonsterSpecularIntensity * specBoostMonster * shadeFade * UnderSpecZero, 1.0f);
    }
    else //나머지(죽순, 캐릭터, 세이브포인터, NPC)
    {
        Out.vSpecular = float4(specular * lightColor * NdotL * g_fSpecularIntensity * specBoost * UnderSpecZero * shadeFade, 1.0f);
    }
    return Out;
}
//빛계산해준다(Toon)
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL_TOON(PS_IN In)
{
    PS_OUT_LIGHT Out;

    float4 vNormalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);
    float4 vNormal = float4((vNormalDesc.xyz * 2.f - 1.f), 0.f); // 0~1 -> -1~1 
    vNormal = normalize(vNormal);
    
    float fSpecularStrength = vNormalDesc.w;
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    float AmbientStrength = vDepthDesc.z;
    float fShineness = vDepthDesc.w;
    //ambient 더하고 diffuse 곱해준다
    float4 vShade = max(dot(normalize(g_vLightDirection) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient * AmbientStrength);

    Out.vShade = (g_vLightDiffuse * saturate(vShade));

    float fViewZ = vDepthDesc.y;

    float4 vWorldPos;
    //TexCorrd를 NDC 좌표로 바꿔준다
    
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

    //투영행렬 역나누기
    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    //뷰 역행렬 나누기
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);

    //이제 월드 좌표로 바꼈음(카메라에서 픽셀보는 방향벡터)
    float4 Specular = float4(1.f, 1.f, 1.f, 1.f);
    float3 V = vWorldPos.xyz - g_vCamPosition.xyz;
    float3 L = normalize(-g_vLightDirection.xyz);
    
    //블린 퐁 하프벡터?
    float3 H = normalize(V + L);
    
    float3 NdotL = saturate(dot(vNormal.xyz, L));
    
    //툰쉐이딩 2~3단계 cel밴드
    //diffuse
    //float toonDiffuse = step(0.5f, NdotL); //2밴드
    //float toonDiffuse = floor(NdotL * 3.0f) / 3.0f; //3밴드
    float toonDiffuse = smoothstep(0.45f, 0.55f, NdotL); //soft cel
    
    
    //specular
    float NdotH = saturate(dot(vNormal.xyz, H));
    float specularBase = pow(NdotH, fShineness); //블린퐁 specular
    float toonSpecular = step(0.6f, specularBase); //hard hightlight 강조?강도?
    
    //Rim Light
    float rim = 1.0f - saturate(dot(vNormal.xyz, V));
    rim = smoothstep(0.4f, 0.8f, rim) * 0.5f;   //림 두께/강도 조정
    float3 rimColor = float3(1.0f, 0.95f, 0.9f) * rim; //밝은테두리
    
    //최종 계산
    float lightMultiplier = 8.0f; //툰은 밝게
    float3 lightColor = g_vLightDiffuse.xyz * lightMultiplier;
    
    //diffuse적용
    float3 diffuse = toonDiffuse * lightColor;
    
    //specular적용
    float specularIntesity = 1.5f;
    float3 specular = toonSpecular * g_vLightSpecular.rgb * fSpecularStrength * specularIntesity;
    
    //Ambient적용(기존유지 AO처럼
    float3 ambient = g_vLightAmbient.rgb * g_vMtrlAmbient.rgb * AmbientStrength;
    
    //최종 Shade
    Out.vShade.rgb = diffuse + specular + ambient + rimColor;
    Out.vShade.a = 1.f;
    
    Out.vSpecular = float4(specular, 1.0f);
    
    if(fShineness <= 0.f)
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

//이거 점조명
PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    Out.vShade = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Out.vSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //Depth
    // [PBR] 배경 픽셀은 라이팅 안 함
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    if (vDepthDesc.y <= 0.001f || vDepthDesc.z == 2.f)
        return Out;

    //Normal
    float4 vNormalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);
    // 0~1 -> -1~1
    float3 N = vNormalDesc.xyz * 2.f - 1.f;
    N = normalize(N);
    
    //specular
    float4 vSpecularDesc = g_TextureMtrlSpecular.Sample(LinearSampler, In.vTexcoord);
    float3 T;
    float Noise;
    float Shift;
    if (vDepthDesc.z == 12.f)//머리라면 기본으로 돌리기
    {
        
        Shift = vSpecularDesc.w;
        Noise = vDepthDesc.w;
        T = vSpecularDesc.xyz * 2.f - 1.f;
        T = normalize(T);
        
        vSpecularDesc = DefaultSpecular;
    }
    
    float4 SSSCheck = g_TextureRoughness.Sample(LinearSampler, In.vTexcoord);
    
    //PBR
    float4 PBR = g_TexturePBR.Sample(LinearSampler, In.vTexcoord);
    
    //b가 SSS, Metal, r AO공통, g를 러프니스로 썼었음
    //g가 AO r은? b SSS 스무스니스 반전해서 러프니스

    //피부용 SSS? 옷은 메탈릭이래
    float Metalic = 0;
    float SSS = 0;
    float roughness = 0;
    if (SSSCheck.r >= 0.5f)
    {
        SSS = PBR.r;
        roughness = max(PBR.g, g_fSkinRoughnessMin); //피부면 높여서 플라스틱 느낌 줄이기
    }
    else
    {
        Metalic = PBR.r; //r
        roughness = max(PBR.g, g_fMtrlRoughnessMin); //일반이면 맨들맨들하게
    }
    float AO = PBR.b; //g
    float alpha = roughness * roughness + 0.05; //GGX사용
   
    //diffuse
    float4 vDiffuseDesc = g_TextureDiffuse.Sample(LinearSampler, In.vTexcoord);
    float3 Albedo = vDiffuseDesc.xyz;
        
    float fViewZ = vDepthDesc.y;
    float4 vWorldPos;
    //TexCorrd를 NDC 좌표로 바꿔준다

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

    //투영행렬 역나누기
    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    //뷰 역행렬 나누기
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);

    //이제 월드 좌표로 바꼈음(카메라에서 픽셀보는 방향벡터)
    //뷰, 빛방향, 하프벡터
    float3 V = normalize(g_vCamPosition.xyz - vWorldPos.xyz); //카메라 방향

    ////프레넬 계산
    float3 specLuma = dot(vSpecularDesc.rgb, float3(0.2126, 0.7152, 0.0722)); // 그레이스케일화
    float3 baseF0 = clamp(specLuma, 0.02, 0.08); // 비금속 F0 범위로 제한
    float3 F0 = lerp(baseF0, Albedo, Metalic);

    ////DistributionGGX(미세면분포)
    //float alpha2 = alpha * alpha;
    
    //GeometrySmith(미세면자기가림)
    float NdotV = saturate(dot(N, V));
    
    //미세변분포
    float k = (roughness + 1.0f) * (roughness + 1.0f) / g_fKValue; // direct light용
    float G_V = NdotV / (NdotV * (1.0f - k) + k + 0.00001);
    
    // 픽셀이 받는 모든 조명의 에너지를 이곳에 누적ㅎㅁ
    float3 finalShade = float3(0.0f, 0.0f, 0.0f);
    float3 finalSpecular = float3(0.0f, 0.0f, 0.0f);
    
    float3 t1;
    float3 t2;
    if (vDepthDesc.z == 12.f)
    {
        t1 = ShiftTangent(T, N, Shift + g_fPlayerHighlight1Shift);
        t2 = ShiftTangent(T, N, Shift + g_fPlayerHighlight2Shift);
    }
    for (uint i = 0; i < lightCount; ++i)
    {
        // 배열에서 i번째 조명의 위치와 범위 가져오기
        float3 lightPos = lights_vPosition[i].xyz;
        float lightRange = lights_vRangeAndType[i].x;

        float3 L = lightPos - vWorldPos.xyz;
        float fDistance = length(L);
        
        // 빛의 범위를 벗어나면 계산스킵
        if (fDistance > lightRange)
            continue;

        L = normalize(L);
        //빛감쇠
        /* 빛을 받으면 1에 가깝게, 빛을 안받으면 0에 가깝게 */
        //float fAtt = saturate((lightRange - fDistance) / lightRange);
        float fDistRatio = fDistance / lightRange; //0~1로 정규화
        float fWindow = saturate(1.0f - fDistRatio * fDistRatio); //가까울때는 천천히 어두워지고 멀어질수록 빠르게 어두워짐(지수방식?)
        fWindow = fWindow * fWindow; // 끝에서 사르르 사라지게
        
        float fFalloff = pow(saturate(1.0f - fDistRatio), 1.5);

    // 최종 감쇠 = 물리 감쇠 × 윈도우
        float fAtt = fFalloff * fWindow;
        
        //하프벡터
        float3 H = normalize(V + L);

        float3 specular;
        float3 diffuse_factor;
        float NdotL = saturate(dot(N, L)); // +  + 0.1f
        if (vDepthDesc.z == 12.f) //머리면 speuclar 다르게 계산
        {
            float spec1 = KajiyaKaySpec(t1, H, g_fPlayerHighlightPower);
            float spec2 = KajiyaKaySpec(t2, H, g_fPlayerHighlightPower);
        
            float3 specColor1 = float3(1, 1, 1);
            float3 specColor2 = Albedo;

            specular = spec1 * g_fPlayerHighlight1Strength * lerp(0.7, 1.0, Noise)
                + spec2 * g_fPlayerHighlight2Strength * specColor2 * lerp(0.7, 1.0, Noise);
        
            diffuse_factor = (1.0f - Metalic) / 3.14f;
        }
        else
        {

        //DistributionGGX(미세면분포)
            float alpha2 = alpha * alpha;
            float NdotH = saturate(dot(N, H));
            float denomD = max((NdotH * NdotH * (alpha2 - 1.0f) + 1.0f), 0.00001f); // lobe 넓이 0.05f
            float D = alpha2 / (denomD * denomD * 3.14f);
   
            float G_L = NdotL / (NdotL * (1.0f - k) + k + 0.00001);
            float G = G_V * G_L;
    
        //FresnelSchlick(시선에 따라 반사율)
            float3 F = F0 + (1.0f - F0) * pow(1.0 - saturate(dot(H, V)), 5.0f); //specular영향 5.f값이랑
    
        //Cook-Torrance Specular
            float3 numerator = D * G * F;
            float denom2 = 4.0 * max(NdotV * NdotL, 0.01f);
        //float specFade = smoothstep(0.0f, 0.15f, NdotV); //에지에서 부드럽게 감쇠
            specular = (numerator / denom2); // * specFade;
        //specular *= 0.45f;
        //diffuse(최종에 diffus해주기 때문에 뺌)
            float3 kD = (1.0f - F) * (1.0f - Metalic);
            diffuse_factor = max(kD, g_fMinDiffuse) / 3.14f; //3.14빼서 밝기 확보(툰) //이거 다시 넣어보기
        }
        
        // 배열에서 해당 조명의 Diffuse 가져오기
        float3 lightColor = lights_vDiffuse[i].rgb * g_fLightMultiplier * fAtt;
        
         //툰이라서 추가
        //float NdotL_soft = saturate(dot(N, L) * 1.2f + 0.05f); // 곡률 보정 + 약한 bias
        // 기존 NdotL을 부드러운 2단계로
        float3 direct;
        float mtrNdotL;
        if (vDepthDesc.z == 12.f)
        {
            mtrNdotL = saturate(dot(N, L) * 0.5 + 0.5); // half-lambert
            direct = lightColor * mtrNdotL;
        }
        else
        {
            mtrNdotL = smoothstep(g_vToonShadowRange.x, g_vToonShadowRange.y, NdotL);
            direct = lightColor * lerp(g_vToonBrightnessRange.x, g_vToonBrightnessRange.y, mtrNdotL); //밝기?
        }
        //float3 direct = lightColor * NdotL_soft;
        
        //SSS적용
        float backLight = pow(saturate(dot(-N, L)), g_fSSSPower); // // 뒤에서 오는 빛
        float3 sssContrib = backLight * SSS * g_fSSSIntensity * lightColor * g_vSSSColor; // // SSS 기여
        
        // 배열에서 해당 조명의 Ambient 가져오기
        float3 ambientBase = lights_vAmbient[i].rgb;
        float3 ambient = ambientBase * g_fAmbientStrength * AO * fAtt;
        
        //최소Ambient보장
        //float3 ambientFloor = max(Albedo * g_vAmbientFloorMin.x, g_vAmbientFloorMin.y); // Albedo가 어두워도 최소 보장
        //ambient = max(ambient, ambientFloor); // 빛 폭발 버그를 막기 위해 루프 안에서는 주석 처리
        
        //옷 뒷면 보존용
        float backFill = (1.0f - mtrNdotL) * g_fBackFillStrength; // 뒷면일수록 최대 추가
        ambient += backFill * fAtt;
        
        float3 currentShade = float3(0, 0, 0);

        // 림라이트 (SSS 이후, Shade 출력 전에 추가) - 툰이라서 추가2
        //float3 smoothN = normalize(N + V * 0.3f); // [EDITED] 시선 방향으로 살짝 당겨서 곡면 부드럽게
        //float rim = 1.0f - saturate(dot(smoothN, V));
        //rim = smoothstep(0.55f, 0.75f, rim); // [EDITED] 고정 범위로 깔끔하게 자르기, g_fRimPower 대신
        //float rimMask = smoothstep(g_vRimMaskRange.x, g_vRimMaskRange.y, NdotL);
        //float3 rimColor = lightColor * g_fRimIntensity;
        //float3 rimContrib = rim * rimColor * rimMask;
        
        //메탈 diffuse 0인거 방지
        float metalMask = smoothstep(0.3f, 0.7f, Metalic);
        float MetalAmbientIntesity = 1.f;
        if (vDepthDesc.z == 11.f) //블러드웨폰만 임시로 3올리기
            MetalAmbientIntesity = 1.f;
        float3 metalAmbient = metalMask * Albedo * ambient * MetalAmbientIntesity;
    
          //피부
        if (SSSCheck.r >= 0.5f)
        { // 최종 Shade = direct + ambient diffusefactor* 색감위해서 뺌
            currentShade = (diffuse_factor * (direct + ambient) + sssContrib) * 0.6f; //+ rimContrib; * g_vSkinTint
        }
    //나머지
        else
        {
            currentShade = diffuse_factor * (direct + ambient) + metalAmbient; //+ rimContrib; //+sssContrib + rimContrib;
        }
        
        
        float shadeFade = pow(smoothstep(0.0f, 0.7f, mtrNdotL), 2.0f); // 제곱으로 더 급격하게
        float specBoost = lerp(g_vSpecBoostRange.x, g_vSpecBoostRange.y, roughness); //강한부분강하게 약한부분 약하게//(맵핑심무성)
        float specBostMap = lerp(g_vSpecBosstMapRange.x, g_vSpecBosstMapRange.y, roughness); //강한부분강하게 약한부분 약하게//(맵핑심무성)
        float specBoostMonster = lerp(g_vSpecMonsterBoostRange.x, g_vSpecMonsterBoostRange.y, roughness);
        
        float3 currentSpecular;
        if (vDepthDesc.z == 12.f) //머리면 speuclar 다르게 계산
        {
            float hairShadeFade = smoothstep(0.0, 0.3, NdotL);
            currentSpecular = float4(specular * lightColor * g_fSpecularIntensity * hairShadeFade, 1.0f);
        }
        else if (vDepthDesc.z == 5.f) //맵, 무기 sepuclar
        {
            currentSpecular = float4(specular * lightColor * NdotL * g_fSpecularMapIntensity * specBostMap * shadeFade, 1.0f);
        }
        else if (vDepthDesc.z == 18.f)
        {
            currentSpecular = float4(specular * lightColor * NdotL * g_fMonsterSpecularIntensity * specBoostMonster * shadeFade, 1.0f);
        }
        else
        {
            currentSpecular = float4(specular * lightColor * NdotL * g_fSpecularIntensity * specBoost * shadeFade, 1.0f);
        }

        // 최종 컬러에 누적
        finalShade += currentShade;
        finalSpecular += currentSpecular;
    }

    Out.vShade = float4(finalShade, 1.0f);
    Out.vShade.a = 1.f;
    Out.vSpecular = float4(finalSpecular, 1.0f);
    return Out;
}
   
PS_OUT_LIGHT PS_MAIN_SPOTLIGHT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    Out.vShade = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Out.vSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //Depth
    // [PBR] 배경 픽셀은 라이팅 안 함
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    if (vDepthDesc.y <= 0.001f || vDepthDesc.z == 2.f)
        return Out;

    //Normal
    float4 vNormalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);
    // 0~1 -> -1~1
    float3 N = vNormalDesc.xyz * 2.f - 1.f;
    N = normalize(N);
    
    //specular
    float4 vSpecularDesc = g_TextureMtrlSpecular.Sample(LinearSampler, In.vTexcoord);
    float3 T;
    float Noise;
    float Shift;
    if (vDepthDesc.z == 12.f)//머리라면 기본으로 돌리기
    {
        
        Shift = vSpecularDesc.w;
        Noise = vDepthDesc.w;
        T = vSpecularDesc.xyz * 2.f - 1.f;
        T = normalize(T);
        
        vSpecularDesc = DefaultSpecular;
    }
    
    float4 SSSCheck = g_TextureRoughness.Sample(LinearSampler, In.vTexcoord);
    
    //PBR
    float4 PBR = g_TexturePBR.Sample(LinearSampler, In.vTexcoord);
    
    //b가 SSS, Metal, r AO공통, g를 러프니스로 썼었음
    //g가 AO r은? b SSS 스무스니스 반전해서 러프니스

    //피부용 SSS? 옷은 메탈릭이래
    float Metalic = 0;
    float SSS = 0;
    float roughness = 0;
    if (SSSCheck.r >= 0.5f)
    {
        SSS = PBR.r;
        roughness = max(PBR.g, g_fSkinRoughnessMin); //피부면 높여서 플라스틱 느낌 줄이기
    }
    else
    {
        Metalic = PBR.r; //r
        roughness = max(PBR.g, g_fMtrlRoughnessMin); //일반이면 맨들맨들하게
    }
    float AO = PBR.b; //g
    float alpha = roughness * roughness; //GGX사용
   
    //diffuse
    float4 vDiffuseDesc = g_TextureDiffuse.Sample(LinearSampler, In.vTexcoord);
    float3 Albedo = vDiffuseDesc.xyz;
        
    float fViewZ = vDepthDesc.y;
    float4 vWorldPos;
    //TexCorrd를 NDC 좌표로 바꿔준다

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

    //투영행렬 역나누기
    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    //뷰 역행렬 나누기
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);

    //이제 월드 좌표로 바꼈음(카메라에서 픽셀보는 방향벡터)
    //뷰, 빛방향, 하프벡터
    float3 V = normalize(g_vCamPosition.xyz - vWorldPos.xyz); //카메라 방향
    float3 L = g_vLightPosition.xyz - vWorldPos.xyz;

    float fDistance = length(L);
    L = normalize(L);
    float3 H = normalize(V + L); //하프벡터
    /* 빛을 받으면 1에 가깝게, 빛을 안받으면 0에 가깝게 */ 
    //좀더물리적이라는데?
    float fDistRatio = fDistance / g_fLightRange; //0~1로 정규화
    float fWindow = saturate(1.0f - fDistRatio * fDistRatio); //가까울때는 천천히 어두워지고 멀어질수록 빠르게 어두워짐(지수방식?)
    fWindow = fWindow * fWindow; // 끝에서 사르르 사라지게
    float fAtt = fWindow;
    
    //콘감쇠 스포트 라이트
    float fCosAngle = dot(-L, normalize(g_vLightDirection.xyz));
    float fSpotFade = saturate((fCosAngle - g_fSpotOuterCone) / (g_fSpotInnerCone - g_fSpotOuterCone));
    fSpotFade = fSpotFade * fSpotFade; // 부드러운 감쇠
    fAtt *= fSpotFade;
    
    float3 specular;
    float3 diffuse_factor;
    float NdotL = saturate(dot(N, L)); // +  + 0.1f
    //일반물체 스페큘러 계산
    if (vDepthDesc.z == 12.f) //머리면 speuclar 다르게 계산
    {
        float3 t1 = ShiftTangent(T, N, Shift + g_fPlayerHighlight1Shift);
        float3 t2 = ShiftTangent(T, N, Shift + g_fPlayerHighlight2Shift);
        
        float spec1 = KajiyaKaySpec(t1, H, g_fPlayerHighlightPower);
        float spec2 = KajiyaKaySpec(t2, H, g_fPlayerHighlightPower);
        
        float3 specColor1 = float3(1, 1, 1);
        float3 specColor2 = Albedo;

        specular = spec1 * g_fPlayerHighlight1Strength * lerp(0.7, 1.0, Noise)
                + spec2 * g_fPlayerHighlight2Strength * specColor2 * lerp(0.7, 1.0, Noise);
        
        diffuse_factor = (1.0f - Metalic) / 3.14f;
    }
    else
    {
        //프레넬 계산
        float3 specLuma = dot(vSpecularDesc.rgb, float3(0.2126, 0.7152, 0.0722)); // 그레이스케일화(밝기만 추출 공식임)
        float3 baseF0 = clamp(specLuma, 0.02, 0.08); // 비금속 F0 범위로 제한(0.02~0.08범위로)
        float3 F0 = lerp(baseF0, Albedo, Metalic); //비금속이면 위의값 쓰고 메탈이면 Albedo색상

        //DistributionGGX(미세면분포)
        float alpha2 = alpha * alpha;
        float NdotH = saturate(dot(N, H));
        float denomD = max((NdotH * NdotH * (alpha2 - 1.0f) + 1.0f), 0.00001f); // lobe 넓이 0.05f
        float D = alpha2 / (denomD * denomD * 3.14f);
    
        //GeometrySmith(미세면자기가림)
        float NdotV = saturate(dot(N, V));
        //NdotL위치였음
    
        float k = (roughness + 1.0f) * (roughness + 1.0f) / g_fKValue; // direct light용
        float G_V = NdotV / (NdotV * (1.0f - k) + k + 0.00001);
        float G_L = NdotL / (NdotL * (1.0f - k) + k + 0.00001);
        float G = G_V * G_L;
    
        //FresnelSchlick(시선에 따라 반사율)
        float3 F = F0 + (1.0f - F0) * pow(1.0 - saturate(dot(H, V)), 5.0f); //specular영향 5.f값이랑
    
        //Cook-Torrance Specular
        float3 numerator = D * G * F;
        float denom2 = 4.0 * max(NdotV * NdotL, 0.01f);
        //float specFade = smoothstep(0.0f, 0.15f, NdotV); //에지에서 부드럽게 감쇠
        specular = (numerator / denom2); // * specFade;
        //specular *= 0.45f;
        //diffuse(최종에 diffus해주기 때문에 뺌)
        float3 kD = (1.0f - F) * (1.0f - Metalic);
        diffuse_factor = max(kD, g_fMinDiffuse) / 3.14f; //3.14빼서 밝기 확보(툰) //이거 다시 넣어보기
    }

    //Direct Light
    float3 lightColor = g_vLightDiffuse.rgb * g_fLightMultiplier * fAtt; // 3.0 ~ 20.0 정도로 테스트 (HDR 느낌)
    // NdotL에 약간의 soft bias (그림자 너무 깊지 않게, 어깨처럼 부드럽게)
    
    //툰이라서 추가
    //float NdotL_soft = saturate(dot(N, L) * 1.2f + 0.05f); // 곡률 보정 + 약한 bias
    // 기존 NdotL을 부드러운 2단계로
    float3 direct;
    float mtrNdotL;
    if (vDepthDesc.z == 12.f)
    {
        mtrNdotL = saturate(dot(N, L) * 0.5 + 0.5); // half-lambert
        direct = lightColor * mtrNdotL;
    }
    else
    {
        mtrNdotL = smoothstep(g_vToonShadowRange.x, g_vToonShadowRange.y, NdotL);
        direct = lightColor * lerp(g_vToonBrightnessRange.x, g_vToonBrightnessRange.y, mtrNdotL); //밝기?
    }
    //float3 direct = lightColor * NdotL_soft;
    
    //SSS적용
    float backLight = pow(saturate(dot(-N, L)), g_fSSSPower); // // 뒤에서 오는 빛
    float3 sssContrib = backLight * SSS * g_fSSSIntensity * lightColor * g_vSSSColor; // // SSS 기여
    
    //Ambient
   
    //뒷면보정 어두울수록 ambient 추가해줌
    //float backFaceBrightness = 1.0f - toonNdotL; // 뒷면일수록 1에 가까움
    //float ambientBoost = lerp(0.8f, 1.2f, backFaceBrightness); // 뒷면이면 ambient 3배

    //Ambient + AO
    float3 ambientBase = g_vLightAmbient.rgb; // float3(0.3f, 0.3f, 0.3f);
    float3 ambient = ambientBase * g_fAmbientStrength * AO * fAtt; //* ambientBoost;
    

    //최소Ambient보장
    float3 ambientFloor = max(Albedo * g_vAmbientFloorMin.x, g_vAmbientFloorMin.y); // Albedo가 어두워도 최소 보장
    ambient = max(ambient, ambientFloor);
    
    //옷 뒷면 보존용
    float backFill = (1.0f - mtrNdotL) * g_fBackFillStrength; // 뒷면일수록 최대 추가
    ambient += backFill * fAtt;
    //ambient = max(ambient, float3(0.08f, 0.08f, 0.08f));
    
    // 림라이트 (SSS 이후, Shade 출력 전에 추가) - 툰이라서 추가2
    //float3 smoothN = normalize(N + V * 0.3f); // [EDITED] 시선 방향으로 살짝 당겨서 곡면 부드럽게
    //float rim = 1.0f - saturate(dot(smoothN, V));
    //rim = smoothstep(0.55f, 0.75f, rim); // [EDITED] 고정 범위로 깔끔하게 자르기, g_fRimPower 대신
    //float rimMask = smoothstep(g_vRimMaskRange.x, g_vRimMaskRange.y, NdotL);
    //float3 rimColor = lightColor * g_fRimIntensity;
    //float3 rimContrib = rim * rimColor * rimMask;

    //메탈 diffuse 0인거 방지
    float metalMask = smoothstep(0.3f, 0.7f, Metalic);
    float MetalAmbientIntesity = 1.f;
    if (vDepthDesc.z == 11.f) //블러드웨폰만 임시로 3올리기
        MetalAmbientIntesity = 1.f;
    float3 metalAmbient = metalMask * Albedo * ambient * MetalAmbientIntesity;
    
     //피부
    if (SSSCheck.r >= 0.5f)
    { // 최종 Shade = direct + ambient diffusefactor* 색감위해서 뺌
        Out.vShade.rgb = (diffuse_factor * (direct + ambient) + sssContrib) * 0.5f; //+ rimContrib; * g_vSkinTint
    }
    //나머지
    else
    {
        Out.vShade.rgb = diffuse_factor * (direct + ambient) + metalAmbient; //+ rimContrib; //+sssContrib + rimContrib;
    }

    Out.vShade.a = 1.0f;

    float shadeFade = pow(smoothstep(0.0f, 0.7f, mtrNdotL), 2.0f); // 제곱으로 더 급격하게
    float specBoost = lerp(g_vSpecBoostRange.x, g_vSpecBoostRange.y, roughness); //강한부분강하게 약한부분 약하게//(맵핑심무성)
    float specBostMap = lerp(g_vSpecBosstMapRange.x, g_vSpecBosstMapRange.y, roughness); //강한부분강하게 약한부분 약하게//(맵핑심무성)
    float specBoostMonster = lerp(g_vSpecMonsterBoostRange.x, g_vSpecMonsterBoostRange.y, roughness);
    float UnderSpecZero = g_fUnderMapZeroDirSpecular;
    
    if (vDepthDesc.z == 12.f) //머리면 speuclar 다르게 계산
    {
        float hairShadeFade = smoothstep(0.0, 0.3, NdotL);
        Out.vSpecular = float4(specular * lightColor * g_fSpecularIntensity * hairShadeFade, 1.0f);
    }
    else if (vDepthDesc.z == 5.f) //맵, 무기 sepuclar
    {
        Out.vSpecular = float4(specular * lightColor * NdotL * g_fSpecularMapIntensity * specBostMap * shadeFade, 1.0f);
    }
    else if (vDepthDesc.z == 18.f) //몬스터
    {
        Out.vSpecular = float4(specular * lightColor * NdotL * g_fMonsterSpecularIntensity * specBoostMonster * shadeFade, 1.0f);
    }
    else //나머지(죽순, 캐릭터, 세이브포인터, NPC)
    {
        Out.vSpecular = float4(specular * lightColor * NdotL * g_fSpecularIntensity * specBoost * shadeFade, 1.0f);
    }
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float4 vDiffuse = g_TextureDiffuse.Sample(LinearSampler, In.vTexcoord);
    float4 vShade = g_TextureShade.Sample(LinearSampler, In.vTexcoord);
    float4 vSpecular = g_TextureSpecular.Sample(LinearSampler, In.vTexcoord);
    //스카이박스 보여주기 위해서 알파가 0이면 discard
    if (vDiffuse.a <= 0.0f)
        discard;

    //색상 추가 보정 vDiffuse.rgb * 
    float3 finalColor = vDiffuse.rgb * vShade.rgb + vSpecular.rgb;


    //Gamma correction (linear ->sRGB)
    //finalColor = pow(finalColor, 1.0f / 2.2f);
    
    //추가밝기
    //finalColor *= 1.4f;
    
    // 또는 boost 대신 채도(saturation) 보정 추가
    //float lum = dot(finalColor, float3(0.3, 0.59, 0.11));
    //finalColor = lerp(float3(lum, lum, lum), finalColor, 1.15f);
    
    Out.vColor = float4(finalColor, 1.0f);
    
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y;
    
    //TexCoord를 NDC 좌표로 바꾼다
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

    //월드좌표된거를 다시 쉐도우라이트 뷰, 투영 z나누기를 해준다
    vWorldPos = mul(vWorldPos, g_LightViewMatrix);
    vWorldPos = mul(vWorldPos, g_LightProjMatrix);

    float fDepth = vWorldPos.w;
    float fObjDepth = vWorldPos.z / vWorldPos.w;
    //NDC좌표를 쉐도우뎁스랑 비교하게 TexCoord로 바꿔준다
    /* 광원기준으로 그려낸 장면상에서, 현재 픽셀이 그려졌어야할 위치에 이미 기록되어있던 깊이를 꺼내온다 */
    float2 vTexcoord;
    vTexcoord.x = (vWorldPos.x / fDepth) * 0.5f + 0.5f;
    vTexcoord.y = (vWorldPos.y / fDepth) * -0.5f + 0.5f;

    //vTexcoord.y = 0.5 + (vTexcoord.y - 0.5) * (shadowAspect / screenAspect);
    // x -> 0 ~ 1 | 투영 z / 뷰스페이스 z 까지 완료한 상태
    // y -> n ~ f | 뷰스페이스 z
    // 아무것도 안 그려져있으면 (1, 1, 1, 1)
    //여기서 뽑을때 맞춰준다?
    float4 vLightDepthDesc = g_TextureShadow1.Sample(LinearSampler, vTexcoord); //이게 저거 좌표로 TexCoord 받아오는거

    if (vLightDepthDesc.x == 1.f && vLightDepthDesc.y == 1.f && vLightDepthDesc.z == 1.f && vLightDepthDesc.w == 1.f)
    {
        return Out;
    }
    
    // OldZ = n ~ f
    float fShadowOldZ = vLightDepthDesc.x; //y는 near~far 그림자는 거의 0에 가까움

    //Depth는 현재픽셀   
    float bias = 0.0007f; //(0.0003 ~ 0.002 사이에서 튜닝)
    if (fObjDepth > fShadowOldZ) //현재픽셀 거의 1
    {

        //float bias1 = 0.005f; // 현재 당신 bias

        //float shadow = 0.0025;
        //float2 texelsize = 4.0 / float2(1600.0, 900.0); // 2.0~3.0 추천

        //for (int x = -2; x <= 2; x++)        // -2 ~ +2 = 5×5
        //{
        //    for (int y = -2; y <= 2; y++)
        //    {
        //        float2 offset = float2(x, y) * texelsize;
        //        float shadowdepth = g_textureshadow.sample(linearsampler, vtexcoord + offset).r;

        //        if (fdepth - bias1 > shadowdepth)
        //            shadow += 0.0f;
        //        else
        //            shadow += 1.0f;
        //    }
        //}

        //shadow /= 25.0f; // 25개 평균

        // 캐릭터 까매짐 방지
        //shadow = max(shadow, 0.65f); // 최소 65% 밝기
        //Out.vColor *= shadow;
        Out.vColor *= 0.5;
    }

    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED_CASCADETEST(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float4 vDiffuse = g_TextureDiffuse.Sample(LinearSampler, In.vTexcoord);
    float4 vShade = g_TextureShade.Sample(LinearSampler, In.vTexcoord);
    float4 vSpecular = g_TextureSpecular.Sample(LinearSampler, In.vTexcoord);
    float4 vSpecularColor = g_TextureMtrlSpecular.Sample(LinearSampler, In.vTexcoord);
    float4 vEmissive = g_TextureEmissive.Sample(LinearSampler, In.vTexcoord);
    float vSSAO = g_TextureSSAOBLUR.Sample(LinearSampler, In.vTexcoord);
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    float4 vSkin = g_TextureRoughness.Sample(LinearSampler, In.vTexcoord);
    
    //옷이라면 스페큘러 소프트 적용
    float3 clampedSpec;
    //if (vDepthDesc.z == 6.f)
    //{
    //    //스페큘러 소프트 클램프(멀리서보면 점처럼 반짝이는거 제거)
    //    //float specLuma = dot(vSpecular.rgb, float3(0.2126, 0.7152, 0.0722));
    //    //float specClamp = 1.0f / (1.0f + specLuma * g_fSpecSoftClamp); // g_fSpecSoftClamp = 1.0~5.0
    //    //clampedSpec = vSpecular.rgb * specClamp;
    //    clampedSpec = min(vSpecular.rgb, g_fSpecSoftClamp); // 0.5~2.0 ImGui
    //}
    //피부 빛 너무 안나게
    if (vSkin.r >= 0.5f) //else 
    {

        clampedSpec = min(vSpecular.rgb, 0.08f); // 0.5~2.0 ImGui 0.1
    }
    else
    {
        clampedSpec = vSpecular.rgb;
    }
    
    
    vSSAO = lerp(1.0f, vSSAO, 0.9f);
    
    //스카이박스 보여주기 위해서 알파가 0이면 discard
    if (vDiffuse.a <= 0.0f)
        discard;
    // [Emissive] 균일 강도 (EmissiveColorGain이 텍스처에 베이크됨)
    float3 emissive = vEmissive.rgb;
    float emissiveStrength = 3.0f; // 이 값 하나만 조절
    float3 emissiveResult = emissive * emissiveStrength;
    float3 finalColor;
    //색상 추가 보정 vDiffuse.rgb *  * vSSAOSS
      
    if (vDepthDesc.z == 15.f) //GodRay용도 빼준다
        finalColor = vDiffuse.rgb + emissiveResult * vDiffuse.rgb; //shade 빼줌
    else if (vDepthDesc.z == 7.f || vDepthDesc.z == 12.f || vDepthDesc.z == 6.f) //죽순용 + 머리용 + 옷용 + 몬스터들  || vDepthDesc.z == 18.f
        finalColor = vDiffuse.rgb * vShade.rgb * vSSAO + emissiveResult + clampedSpec;
    else if (g_SSAOOption.x >= 1.f)
        finalColor = vDiffuse.rgb * vShade.rgb * vSSAO + emissiveResult * vDiffuse.rgb + clampedSpec;
    else
        finalColor = vDiffuse.rgb * vShade.rgb + emissiveResult * vDiffuse.rgb + clampedSpec;
    
    Out.vColor = float4(finalColor.rgb, 1.0f);
    

    float fViewZ = vDepthDesc.y;
    //배경 그림자 계산안함?
    if (vDepthDesc.y <= 0.001f || vDepthDesc.z == 2.f || vDepthDesc.z == 15.f)
        return Out;
    //TexCoord를 NDC 좌표로 바꾼다
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
    
    ////여기서 OutSplit계산해서 그림자 계산
    //cascade 선택 후 범위 밖이면 다음 cascade로 폴백
    float4 shadowPos = float4(0, 0, 0, 0);
    int ShadowCasCadeNum = 0;

  //현재 fViewZ에 해당하는 cascade부터 시작해서 폴백
    int startCascade = 0;
    if (fViewZ <= g_ShadowSplit.x)
        startCascade = 0;
    else if (fViewZ <= g_ShadowSplit.y)
        startCascade = 1;
    else if (fViewZ <= g_ShadowSplit.z)
        startCascade = 2;
    else if (fViewZ <= g_ShadowSplit.w)
        startCascade = 3;
    else
        return Out;
    
//해당 cascade부터 시도, UV 밖이면 다음으로
    [unroll]
    for (int c = startCascade; c < 4; c++)
    {
        float4 testPos = mul(float4(vWorldPos.xyz, 1.f), g_ShadowCascade_MixMaxtrix[c]);
        float2 uv = testPos.xy / testPos.w;
    
        float margin = 0.001f;
        if (uv.x >= -1.0f + margin && uv.x <= 1.0f - margin && uv.y >= -1.0f + margin && uv.y <= 1.0f - margin)
        {
            shadowPos = testPos;
            ShadowCasCadeNum = c + 1;
            break;
        }
    }
    float shadow = 1.f;
    //월드좌표된거를 다시 쉐도우라이트 뷰, 투영 z나누기를 해준다
    
    if(ShadowCasCadeNum > 0)
    {
        float fDepth = shadowPos.w;
        float fObjDepth = shadowPos.z / shadowPos.w;
        
        //범위밖 픽셀 그림자 계산을 스킵(그림자 멀어지면 생기는거 해결?)
        if (fObjDepth >= 0.0f && fObjDepth <= 1.0f)
        {
            shadow = 0.0f;
    //NDC좌표를 쉐도우뎁스랑 비교하게 TexCoord로 바꿔준다
    /* 광원기준으로 그려낸 장면상에서, 현재 픽셀이 그려졌어야할 위치에 이미 기록되어있던 깊이를 꺼내온다 */
            float2 vTexcoord;
            vTexcoord.x = (shadowPos.x / fDepth) * 0.5f + 0.5f;
            vTexcoord.y = (shadowPos.y / fDepth) * -0.5f + 0.5f;

    //vTexcoord.y = 0.5 + (vTexcoord.y - 0.5) * (shadowAspect / screenAspect);
    // x -> 0 ~ 1 | 투영 z / 뷰스페이스 z 까지 완료한 상태
    // y -> n ~ f | 뷰스페이스 z
    // 아무것도 안 그려져있으면 (1, 1, 1, 1)
    //여기서 뽑을때 맞춰준다?
    
    //3x3 수동 샘플링
    //float bias1 = 0.0001f; //0.005 0.0021
            shadow = 0.0;
            float2 texelSize = 1.0 / 2048.f;
            float bias1 = 0.0001f;
            int PCF = 1;
            
            for (int x = -PCF; x <= PCF; ++x)
            {
                for (int y = -PCF; y <= PCF; ++y)
                {
                    float2 offset = float2(x, y) * texelSize;
            
                //각 샘플마다 자동 2x2 PCF!
                    if (ShadowCasCadeNum == 1)
                        shadow += g_TextureShadow1.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
                    else if (ShadowCasCadeNum == 2)
                        shadow += g_TextureShadow2.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
                    else if (ShadowCasCadeNum == 3)
                        shadow += g_TextureShadow3.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
                    else if (ShadowCasCadeNum == 4)
                        shadow += g_TextureShadow4.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
                }
            }
            shadow /= 9.0; // 9개 평균
        }
    }
   
    //poision disk sampling
    //float shadow = 0.0;
    //float2 texelSize = 1.0 / 2048.f;
    //float bias1 = 0.0f;
    //float radius = 2.0; // 반경 (크면 더 부드러움)
    
    //for (int i = 0; i < 16; ++i)
    //{
    //    float2 offset = poissonDisk[i] * texelSize * radius;
    //    if (ShadowCasCadeNum == 1)
    //        shadow += g_TextureShadow1.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
    //    else if (ShadowCasCadeNum == 2)
    //        shadow += g_TextureShadow2.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
    //    else if (ShadowCasCadeNum == 3)
    //        shadow += g_TextureShadow3.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
    //    else if (ShadowCasCadeNum == 4)
    //        shadow += g_TextureShadow4.SampleCmpLevelZero(ShadowSampler, vTexcoord + offset, fObjDepth - bias1);
    //}
    //shadow /= 16.0;

    //정적은 빛방향반대면 그림자 생기게
    float4 normalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);
    float3 worldNormal = normalDesc.xyz * 2.0f - 1.0f;
    float NdotL = dot(normalize(worldNormal), -normalize(g_ShadowDir.xyz));
    float backFaceShadow = smoothstep(g_vBackFaceShaowRange.x, g_vBackFaceShaowRange.y, NdotL); //0.0 0.1
    backFaceShadow = max(backFaceShadow, g_fShadowBackBright); //메쉬단면의 그림자 최소밝기
    //정적그림자 추가
    // [StaticShadow] 정적 섀도우맵 샘플링
    float4 staticShadowPos = mul(float4(vWorldPos.xyz, 1.f), g_Shadow_ViewProjMatrix);
    float2 staticUV;
    staticUV.x = (staticShadowPos.x / staticShadowPos.w) * 0.5f + 0.5f;
    staticUV.y = (staticShadowPos.y / staticShadowPos.w) * -0.5f + 0.5f;
    

    float staticShadow = 1.0f;
    // [StaticShadow] UV 범위 안에 있을 때만 샘플링
    if (staticUV.x >= 0.0f && staticUV.x <= 1.0f && staticUV.y >= 0.0f && staticUV.y <= 1.0f)
    {
        float staticObjDepth = staticShadowPos.z / staticShadowPos.w;
        float2 staticTexelSize = 1.0f / 8192.f; // 섀도우맵 해상도에 맞게
        staticShadow = 0.0f;
        int PCF = 1;
        for (int sx = -PCF; sx <= PCF; ++sx)
        {
            for (int sy = -PCF; sy <= PCF; ++sy)
            {
                float2 offset = float2(sx, sy) * staticTexelSize;
                staticShadow += g_TextureBakeShadow.SampleCmpLevelZero(ShadowSampler, staticUV + offset, staticObjDepth - 0.0001f);
            }
        }
        staticShadow /= 9.0f;
    }
    
    staticShadow = min(staticShadow, backFaceShadow);
    
    // [Shadow] 동적(CSM) + 정적 그림자 합성 (더 어두운 쪽 사용)
    float finalShadow = min(shadow, staticShadow);
    if (vDepthDesc.z == 11.f || vSkin.r >= 0.5f || vDepthDesc.z == 8.f) //특수무기 or 피부 or 눈
        Out.vColor.rgb *= max(finalShadow, g_fShadowBright); //지하 이렇게하고
    else if (vDepthDesc.z == 18.f || vDepthDesc.z == 6.f) 
        Out.vColor.rgb *= max(finalShadow, 0.4f); //디졸브 되는애들
    else
        Out.vColor.rgb *= max(finalShadow, 0.3f); //맵
    //if (vDepthDesc.z != 15.f) 
    //    Out.vColor.rgb += clampedSpec * max(finalShadow, 0.8); //80프로만 남기기
    
    
    //림라이트1
    //float3 smoothN = normalize(N + V * 0.3f); // [EDITED] 시선 방향으로 살짝 당겨서 곡면 부드럽게
    //float rim = 1.0f - saturate(dot(smoothN, V));
    //rim = smoothstep(0.55f, 0.75f, rim); // [EDITED] 고정 범위로 깔끔하게 자르기, g_fRimPower 대신
    //float rimMask = smoothstep(g_vRimMaskRange.x, g_vRimMaskRange.y, NdotL);
    //float3 rimColor = lightColor * g_fRimIntensity;
    //float3 rimContrib = rim * rimColor * rimMask;
    
    //float3 N = normalize(worldNormal);
    //float3 V = normalize(g_vCamPosition.xyz - vWorldPos.xyz); //카메라 방향
    //float3 smoothN = normalize(lerp(N, V, 0.5));
    //float rimNdotL = saturate(dot(smoothN, -normalize(g_vLightDirection.xyz)));
    //float rim = 1.0f - saturate(dot(smoothN, V));
    //rim = pow(rim, g_fRimPower); //림 두께/강도 조정
    //rim *= rimNdotL;
    //if (vDepthDesc.z == 5.f)
    //    rim = 0.0f;

    //Out.vColor.rgb += rim * g_vMidtoneTintColor * g_fRimIntensity;
    
    //림라이트2
    //Out.vColor.rgb += rim * g_vMidtoneTintColor * g_fRimIntensity;
    //계산은 각 월드 local로 계산해서 내준다(binormal 그런거 계산안된값)
    //if (vDepthDesc.z == 10.f)
    //{
    //    float rim = pow(vDepthDesc.w, g_fRimPower);
    //    Out.vColor.rgb += rim * g_vMidtoneTintColor.xyz * g_fRimIntensity;
    //}
    
    //아웃라인(뎁스기반)
    float2 rimTexel = float2(1.0f / 1600.f, 1.0f / 900.f); //한픽셀의 uv크기

    float cD = vDepthDesc.y;
    float rD = g_TextureDepth.Sample(LinearSampler, In.vTexcoord + float2(rimTexel.x, 0)).y; //오른쪽
    float lD = g_TextureDepth.Sample(LinearSampler, In.vTexcoord - float2(rimTexel.x, 0)).y; //왼쪽
    float uD = g_TextureDepth.Sample(LinearSampler, In.vTexcoord + float2(0, rimTexel.y)).y; //위
    float dD = g_TextureDepth.Sample(LinearSampler, In.vTexcoord - float2(0, rimTexel.y)).y; //아래

    float depthEdge = abs(rD - lD) + abs(uD - dD); //좌우차이, 상하차이(이값이 크면 경계선)
    depthEdge /= (cD + 0.001f); //거리보정(멀리있는물체는 깊이차이가 작아지니까)

    float Outline = smoothstep(0.1f, 0.5f + 0.01f, depthEdge); //0.1은 없애고 0.1~0.51은 부드럽게 0.51이상은 1로

    float3 OutlineColor = Out.vColor.rgb * 0.15; //아웃라인색을 현재픽셀색의 15프로
    //if (vDepthDesc.z == 5.f)
    //    Outline = 0.0f;
    Out.vColor.rgb = lerp(Out.vColor.rgb, OutlineColor.xyz, Outline); //아웃라인 0이면 그대로 1이면 어두운 아웃라인색
    
  
    return Out;
}


//Original HDR
PS_OUT_BACKBUFFER PS_MAIN_HDR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    Out.vColor = g_TextureHDR.Sample(LinearSampler, In.vTexcoord);
    
    return Out;
}

//BrightExtract(일단 기본틀하고 나중에 더 작업)
PS_OUT_BACKBUFFER PS_MAIN_HDR_BrightExtract(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    //해상도 내리면 4개 평균내서 부드럽게
    float2 texelSize = 1.0 / float2(1600, 900); // 원본 해상도
    float2 uv = In.vTexcoord;
    
    // 정확히 대응되는 2x2 영역 샘플링
    float3 c0 = g_TextureHDR.Sample(LinearClampSampler, uv + float2(-0.5, -0.5) * texelSize).rgb; //왼쪽위
    float3 c1 = g_TextureHDR.Sample(LinearClampSampler, uv + float2(0.5, -0.5) * texelSize).rgb; //오른쪽위
    float3 c2 = g_TextureHDR.Sample(LinearClampSampler, uv + float2(-0.5, 0.5) * texelSize).rgb; //왼쪽아래
    float3 c3 = g_TextureHDR.Sample(LinearClampSampler, uv + float2(0.5, 0.5) * texelSize).rgb; //오른쪽아래
    float3 emissive = g_TextureEmissive.Sample(LinearClampSampler, uv).rgb; //emissive;
    float3 EffectEmissive = g_TextureEffectEmissive.Sample(LinearClampSampler, uv).rgb; //emissive;
    float3 GodRay = g_TextureGodRay.Sample(LinearClampSampler, uv).rgb; //GodRay
    
    float3 avgColor = (c0 + c1 + c2 + c3) * 0.25;
    
    float3 CombinedColor = avgColor + GodRay + EffectEmissive;
    
    float brightness = dot(avgColor, float3(0.2126, 0.7152, 0.0722)); //사람 눈기준 밝기 sRGB표준(크면 민감)
    float emissivebirhgt = dot(emissive, float3(0.2126, 0.7152, 0.0722));
    float Effectemissivebirhgt = dot(EffectEmissive, float3(0.2126, 0.7152, 0.0722));
    float GodRaybright = dot(GodRay, float3(0.2126, 0.7152, 0.0722)); //사람 눈기준 밝기 sRGB표준(크면 민감)
    
    // emissive bloom 기여를 별도로 계산 (씬 전체색이 bloom에 오염되는 것 방지)
    float3 emissiveContrib = emissive * 3.0f; // Combined 패스의 emissiveStrength와 동일
    float emissiveLuma = dot(emissiveContrib, float3(0.2126, 0.7152, 0.0722));
    
    // 씬 밝기가 threshold 넘으면 bloom
    if (brightness > g_HDROption.w) //1.2~1.5추천받음
        Out.vColor = float4(CombinedColor, 1.f);
    else if (emissivebirhgt >= 0.01f)
        Out.vColor = float4(CombinedColor, 1.f);
    else if (Effectemissivebirhgt >= 0.01f)
        Out.vColor = float4(CombinedColor, 1.f);
    else if (GodRaybright >= 0.01f)
        Out.vColor = float4(CombinedColor, 1.f);
    else
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
      
    return Out;
}

//2x2 DownSampling
PS_OUT_BACKBUFFER PS_MAIN_HDR_DownSampling(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    //해상도 내리면 4개 평균내서 부드럽게
    float2 texelSize = 1.0 / g_TextureSize; // 원본 해상도
    float2 uv = In.vTexcoord;
    
    // 정확히 대응되는 2x2 영역 샘플링
    float3 c0 = g_TextureBright.Sample(LinearClampSampler, uv + float2(-0.5, -0.5) * texelSize).rgb;
    float3 c1 = g_TextureBright.Sample(LinearClampSampler, uv + float2(0.5, -0.5) * texelSize).rgb;
    float3 c2 = g_TextureBright.Sample(LinearClampSampler, uv + float2(-0.5, 0.5) * texelSize).rgb;
    float3 c3 = g_TextureBright.Sample(LinearClampSampler, uv + float2(0.5, 0.5) * texelSize).rgb;
    
    float3 avgColor = (c0 + c1 + c2 + c3) * 0.25;
    
    Out.vColor = float4(avgColor, 1.f);
    
    return Out;
}

//가우시안 가중치(가우시안블러) - 중심에서 멀수록 영향급격히 감소
static const float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

//Blur Horizontal(한픽셀당 9번 샘플링 좌4 + 중앙 + 우4)
PS_OUT_BACKBUFFER PS_MAIN_HDR_BlurHorizontal(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    
    float2 uv = In.vTexcoord;
    
    float3 result = g_TextureSampling.Sample(LinearClampSampler, uv).rgb * weights[0]; //현재픽셀읽고 가중치 제일큰거 곱함
    
    float2 texelSize = 1.f / float2(400.f, 225.f); //픽셀 1칸 크기
    // 좌우로 샘플링 (가로 방향으로만!)
    for (int i = 1; i < 5; i++)
    {
        float offset = i * texelSize.x; // x방향으로만 이동
        
        // 오른쪽
        result += g_TextureSampling.Sample(LinearClampSampler, uv + float2(offset, 0)).rgb * weights[i]; //현재픽셀에서 가로로 +offset만큼 이동
        
        // 왼쪽
        result += g_TextureSampling.Sample(LinearClampSampler, uv - float2(offset, 0)).rgb * weights[i];
    }
    
    Out.vColor = float4(result, 1.0);
    
    return Out;
}

//Blur Vertical
PS_OUT_BACKBUFFER PS_MAIN_HDR_BlurVertical(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    
    float2 uv = In.vTexcoord;
    
    float3 result = g_TextureSampling.Sample(LinearClampSampler, uv).rgb * weights[0];
    
    float2 texelSize = 1.f / float2(400.f, 225.f);
    // 위아래로 샘플링 (세로 방향으로만!)
    for (int i = 1; i < 5; i++)
    {
        float offset = i * texelSize.y; // y방향으로만 이동
        
        // 아래
        result += g_TextureSampling.Sample(LinearClampSampler, uv + float2(0, offset)).rgb * weights[i];
        
        // 위
        result += g_TextureSampling.Sample(LinearClampSampler, uv - float2(0, offset)).rgb * weights[i];
    }
    
    Out.vColor = float4(result, 1.0);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_HDR_UpSampling(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    Out.vColor = g_TextureSampling.Sample(LinearClampSampler, In.vTexcoord); //Linear가 자동으로 확대해줌(자동으로 주변 4픽셀 섞어줌)
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_HDR_UpSamplingPlus(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float4 Add1 = g_TextureSampling.Sample(LinearClampSampler, In.vTexcoord);
    float4 Add2 = g_TextureSamplingAdd.Sample(LinearClampSampler, In.vTexcoord); //Linear가 자동으로 확대해줌(자동으로 주변 4픽셀 섞어줌)
    Out.vColor = Add1 + Add2;
    
    return Out;
}


float2 ComputeSeparateFog(float depth, float worldY)
{
     //거리포그
    float distFog;

    if (g_fogType < 0.5f)
        distFog = saturate((g_fFogEndDist - depth) / (g_fFogEndDist - g_fFogStartDist)); //선형방식 0~1사이로 안개짙어짐
    else
        distFog = exp(-g_fFogDensity * depth); //지수감쇠방식 자연스럽게 감쇠
    
    float distAmount = 1.0 - distFog; //뒤집어서 안개양으로 0이면 안개X 1이면 안개O

    //높이포그
    //지수 감쇠로 변경, base 위로 급격히 사라짐
    float heightAboveBase = max(worldY - g_fFogBaseHeight, 0.0); //g_fFogBaseHeight 아래면 0으로 클램프
    float t = saturate(heightAboveBase / max(g_fFogFadeHeight, 0.001)); //g_fFogFadeHeight에서 얼마나 올라갔는지 0~1로 정규화
    float heightFalloff = exp(-t * 3.0) * (1.0 - t); //높이에 따른 안개 감쇠 (올라갈수록 지수적감소) * (fadeHeight끝에서 0으로)
    float heightAmount = heightFalloff * g_fHeightFogIntensity; //강도곱해줌

    return float2(distAmount, heightAmount); //따로반환
}   
//HDR Mapping(블룸-밝기조절-톤매핑-뎁스포그-칼라그레이딩-감마조절-감마조절후채도조절)
PS_OUT_BACKBUFFER PS_MAIN_HDR_MAPPING(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float3 vHDRColor = g_TextureHDR.Sample(LinearSampler, In.vTexcoord).rgb;
   
    float3 vBloom = g_TextureSampling.Sample(LinearClampSampler, In.vTexcoord).rgb;
    
    //원본색상 + 블룸조합
    float3 finalColor = vHDRColor + vBloom * g_HDROption.x;
    
    //노출조정(밝기)
    //float exposure = 1.0f;
    finalColor *= g_HDROption.y;

    //ACES전에 채도 보호? HDR공간에서 미리올려줌//(맵핑심무성)
    float3 presat_luma = dot(finalColor, float3(0.2126, 0.7152, 0.0722));
    finalColor = lerp(presat_luma, finalColor, 1.3f); // HDR에서 살짝 채도 부스트
    
    //ACESFilm(톤매핑) - 밝은건 눌러주고 어두운건 선형으로 0~무한대값을 -> 0~1로 바꿔준다(모니터는 0~1만 표현)
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    float3 ldrColor = saturate((finalColor * (a * finalColor + b)) / (finalColor * (c * finalColor + d) + e));
    
    // Reinhard 톤매핑 (채도를 덜 먹음)
    //float3 ldrColor = finalColor / (finalColor + 1.0f);
    float4 vDepthInfo = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    
    //Depth Fog
    if (g_bFogEnable >= 1.f)
    {
        float fViewZ = vDepthInfo.y;
        float3 vWorldPos = g_TexturePick.Sample(LinearSampler, In.vTexcoord).xyz;

        float2 fog = ComputeSeparateFog(fViewZ, vWorldPos.y);

    // distance fog 따로 (maxOpacity 제한)
        float distFog = min(fog.x, g_fFogIntensity); //적용강도 0~1

        
    // height fog 따로 (별도 색상 + 별도 maxOpacity)
        float heightFog = min(fog.y, g_fFogIntensity);

        //if (vDepthInfo.z != 5.f)
        //{
        //    distFog *= 0.6f; // 맵의 30%만
        //    heightFog *= 0.6f;
        //}
        ldrColor = lerp(ldrColor, g_FogColor, distFog); //원본색이랑 안개색 섞기
        ldrColor = lerp(ldrColor, g_FogColor, heightFog);
    }
    
    //ColorGrading
    if(g_fEnableColorGrading >= 1.f)
    {
        float lum = dot(ldrColor, float3(0.2126, 0.7152, 0.0722));
    
        float shadowMask = 1.0 - smoothstep(0.0, g_fShadowRange, lum);
    
        float highlightMask = smoothstep(g_fHighLightRange, 1.0, lum);
    
        float midtoneMask = 1.0 - shadowMask - highlightMask;
        midtoneMask = saturate(midtoneMask);
    
    // Multiply blend: tint * weight + (1 - weight) = lerp towards tint
        float3 shadowColor = lerp(float3(1, 1, 1), g_vShadowTintColor, g_fShadowTintWeight);
        float3 midtoneColor = lerp(float3(1, 1, 1), g_vMidtoneTintColor, g_fMidtoneTintWeight);
        float3 highlightColor = lerp(float3(1, 1, 1), g_vHighlightTintColor, g_fHighlightTintWeight);
    
        float3 tint = shadowColor * shadowMask
                + midtoneColor * midtoneMask
                + highlightColor * highlightMask;
    
        ldrColor *= tint;
    }
   
    // 감마
    // ldrColor = pow(ldrColor, 1.0 / 2.2f);
    ldrColor = pow(ldrColor, 1.0 / g_HDROption.z);

    // [색보정] 따뜻한 톤 (원본 게임 느낌)
    //ldrColor *= float3(1.08f, 1.02f, 0.92f); // R 올리고 B 내리기
    // 감마 후 채도 복구
    float luma = dot(ldrColor, float3(0.2126, 0.7152, 0.0722));
    ldrColor = lerp(luma, ldrColor, 1.15f); //(맵핑심무성) 1.35
    //ldrColor = lerp(ldrColor, luma, 0.08f);

    Out.vColor = float4(ldrColor, 1.f);
    return Out;
}

//SSAO용
// [SSAO] 텍스쳐 좌표에서 뷰스페이스 위치 복원
float3 GetViewPos(float2 _uv)
{
    float4 depthDesc = g_TextureDepth.Sample(LinearSampler, _uv);
    float ndcZ = depthDesc.x;
    float viewZ = depthDesc.y;

    // NDC xy 복원
    float2 ndc;
    ndc.x = _uv.x * 2.0f - 1.0f;
    ndc.y = _uv.y * -2.0f + 1.0f;

    // 수정됨 : perspective projection에서 view-space xy 직접 계산
    // InverseProj._11 = 1/Proj._11, InverseProj._22 = 1/Proj._22
    float3 viewPos;
    viewPos.x = ndc.x * g_InverseProjMatrix._11;
    viewPos.y = ndc.y * g_InverseProjMatrix._22;

    // 수정됨 : ndcZ에서 viewZ 복원 후 xy에 곱하기
    // perspective에서 NDC.xy = ViewPos.xy * Proj / ViewZ 이므로
    // ViewPos.xy = NDC.xy * InvProj._11/_22 * ViewZ
    viewPos.xy *= viewZ;
    viewPos.z = viewZ;

    return viewPos;
}


PS_OUT_BACKBUFFER PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    //배경이면 AO 계산 안 함
    //플레이어 피부도 계산안함
    float4 depthCheck = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    float4 SSSCheck = g_TextureRoughness.Sample(LinearSampler, In.vTexcoord);

    if (depthCheck.y <= 0.001f || depthCheck.z == 2 || SSSCheck.r >= 0.5)
    {
        Out.vColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        return Out;
    }
    
    //현재 픽셀의 뷰스페이스 위치
    float3 fragPos = GetViewPos(In.vTexcoord);
    
    //노멀 가져오기 (G-Buffer는 월드스페이스로 저장됨)
    float4 normalDesc = g_TextureNormal.Sample(LinearSampler, In.vTexcoord);
    float3 worldNormal = normalDesc.xyz * 2.0f - 1.0f;
    worldNormal = normalize(worldNormal);
    
    //월드 노멀 → 뷰스페이스 노멀로 변환
    float3 normal = mul(float4(worldNormal, 0.0f), g_CamViewMatrix).xyz;
    normal = normalize(normal);
    
    //해시 기반 랜덤 회전
    float2 noiseUV = In.vTexcoord * g_NoiseScale;
    float n1 = frac(sin(dot(noiseUV, float2(12.9898f, 78.233f))) * 43758.5453f);
    float n2 = frac(sin(dot(noiseUV, float2(93.9898f, 67.345f))) * 24578.1459f);
    float3 randomVec = normalize(float3(
        n1 * 2.0f - 1.0f,
        n2 * 2.0f - 1.0f,
        0.0f
    ));
    
    //TBN 매트릭스 구성
    float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    //샘플링
    float occlusion = 0.0f;
    float validSamples = 0.0f;
    
    // projection 파라미터 루프 밖에서 한번만 추출
    float projScale11 = g_CamProjMatrix._11;
    float projScale22 = g_CamProjMatrix._22;

    //거리기반 감쇠로 bias도 감소하게
    float attenuation = saturate(depthCheck.y / 5.f);
    float adjustedRadius = g_SSAORadius * attenuation;
    
    for (int i = 0; i < 16; ++i) //32였는데 줄임
    {
        float3 sampleDir = mul(g_Samples[i].xyz, TBN);
        float3 samplePos = fragPos + sampleDir * adjustedRadius;
        
        // 4x4 행렬곱 제거, projection 파라미터로 직접 투영
        float2 projXY;
        projXY.x = samplePos.x * projScale11 / samplePos.z;
        projXY.y = samplePos.y * projScale22 / samplePos.z;

        float2 sampleUV;
        sampleUV.x = projXY.x * 0.5f + 0.5f;
        sampleUV.y = projXY.y * -0.5f + 0.5f; // y반전 정리 (-0.5 + 0.5)
        
        float sampleDepth = GetViewPos(sampleUV).z;
        
        //배경 무시
        if (sampleDepth <= 0.001f)
        {
            continue;
        }
        
        //깊이 차이 너무 크면 무시 (하얀 후광 방지)
        if (abs(fragPos.z - sampleDepth) > g_SSAORadius * 3.0f)
        {
            continue;
        }
        
        occlusion += (sampleDepth >= samplePos.z + g_SSAOBias ? 1.0f : 0.0f);
        validSamples += 1.0f;
    }
    //유효샘플적으면 AO 없음처리
    if (validSamples < 8.0f)
    {
        Out.vColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        return Out;
    }
    
    //유효 샘플로 나누기
    float ao = occlusion / max(validSamples, 1.0f);
    ao = saturate(ao);
    ao = pow(ao, 2.f);
    
    //거리기반 감쇠(가까우면 덜먹게)
    float finalAO = lerp(1.0, ao, attenuation);
    
    Out.vColor = float4(finalAO, finalAO, finalAO, 1.0f);
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_SSAO_BLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    float centerDepth = g_TextureDepth.Sample(LinearSampler, In.vTexcoord).y;
    
    float ao = 0.0f;
    float totalWeight = 0.0f;   
    float2 texelSize = 2.0f / (g_NoiseScale * 4.0f);
    int Weights = 3;
    //7x7 깊이 기반 블러
    for (int x = -Weights; x <= Weights; ++x)
    {
        for (int y = -Weights; y <= Weights; ++y)
        {
            float2 offset = float2((float) x, (float) y) * texelSize;
            float2 sampleUV = In.vTexcoord + offset;
            
            float sampleAO = g_TextureSSAO.Sample(LinearSampler, sampleUV).r;
            float sampleDepth = g_TextureDepth.Sample(LinearSampler, sampleUV).y;
            
            //깊이 차이 크면 블러 안 함 (경계 보존)
            float depthDiff = abs(centerDepth - sampleDepth);
            float weight = 1.0f / (1.0f + depthDiff * 100.0f);
            
            ao += sampleAO * weight;
            totalWeight += weight;
        }
    }
    
    ao /= totalWeight;
    Out.vColor = float4(ao, ao, ao, 1.0f);
    return Out;
}

//GodRay부분만 뽑기
PS_OUT_BACKBUFFER PS_MAIN_GODRAY_COPY(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float4 vDepth = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    float vColor = g_TextureHDR.Sample(LinearSampler, In.vTexcoord);
    float4 vColorfinal;
    if (vDepth.z == 15.f)
        vColorfinal = vColor;
    else
        vColorfinal = float4(0.f, 0.f, 0.f, 1.f);
    
    Out.vColor = vColorfinal;
    return Out;
}

#define NUM_SAMPLES 64

//radial blur
PS_OUT_BACKBUFFER PS_MAIN_GODRAY(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
        
    float4 vWorldPos = g_vGodRayPosition; //태양위치  _float4(-122.29f, 105.36f, -153.458f, 1.f); 
    float4 clipPos = mul(vWorldPos, g_CamViewMatrix); //뷰
    clipPos = mul(clipPos, g_CamProjMatrix); //투영
    clipPos.xyz /= clipPos.w;  //z나누기
    
    //-1~1범위를 0~1(UV범위)로 바꿈
    float2 LightUV;
    LightUV.x = clipPos.x * 0.5f + 0.5f;
    LightUV.y = -clipPos.y * 0.5f + 0.5f; // Y 반전
    
    float2 deltaUV = (In.vTexcoord - LightUV); //현재 픽셀에서 태양까지 방향구하기(태양->현재픽셀)
    // 샘플 간격
    deltaUV *= (1.0f / NUM_SAMPLES) * g_fGodRayDensity; //태양까지 sample수만큼 보폭정함(보폭)
    
    float2 uv = In.vTexcoord;
    float illumination = 0.0f;
    
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        uv -= deltaUV; //한걸음이동
        // 갓레이에서 샘플링
        float sample = g_TextureGodRayCopy.Sample(LinearSampler, uv).r; //GodRay조명 넣어놓은거 읽어온다
        // 거리에 따라 감쇠
        sample *= pow(g_fGodRayDecay, (float) i); //태양에서 멀면 멀수록 감쇠
        illumination += sample * g_fGodRayWeight; //각 샘플의 기여도
    }
    
    Out.vColor = float4(illumination * g_fGodRayExposure * g_vGodRayColor.xyz, 1.0f);
    return Out;
}


PS_OUT_BACKBUFFER PS_MAIN_CAMVELOCITY(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float2 exist = g_TextureCamVelocity.Sample(LinearSampler, In.vTexcoord);
    
    if (length(exist) > 0.001f)
    {
        Out.vColor = float4(exist, 0.f, 1.f);
        return Out;
    }
    
    // [PBR] 배경, 플레이어 패스
    float4 vDepthDesc = g_TextureDepth.Sample(LinearSampler, In.vTexcoord);
    if (vDepthDesc.y <= 0.001f || vDepthDesc.z == 2.f || vDepthDesc.z == 6.f || vDepthDesc.z == 12.f || vDepthDesc.z == 8.f)
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
        return Out;
    }
        
    //TexCoord를 NDC 좌표로 바꾼다

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
    vWorldPos *= vDepthDesc.y;

    // Loacl * World * View * Proj  => Loacl * World * View
    vWorldPos = mul(vWorldPos, g_InverseProjMatrix);
    
    // Loacl * World * View  => Loacl * World
    vWorldPos = mul(vWorldPos, g_InverseViewMatrix);
    
    //이전 UV좌표복구
    float4 vPrevNDC = mul(vWorldPos, g_PrevViewMatrix);
    vPrevNDC = mul(vPrevNDC, g_PrevProjMatrix);
    
    float2 PrevUV = vPrevNDC.xy / vPrevNDC.w;
    PrevUV.x = PrevUV.x * 0.5f + 0.5f;
    PrevUV.y = PrevUV.y * -0.5f + 0.5f;
     
    float2 velocity = In.vTexcoord - PrevUV;
    Out.vColor = float4(velocity, 0.f, 1.f);
    
    return Out;
}

//모션블러
PS_OUT_BACKBUFFER PS_MAIN_CAMMOTIONBLUR(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;

    float2 uv = In.vTexcoord;

    float2 velocity = g_TextureCamVelocity.Sample(LinearSampler, uv).rg;

    //블러강도
    velocity *= g_fCamMotionBlurIntensity; 

    // /* 최대 velocity clamp (너무 긴 블러 방지) */
    float speed = length(velocity);
    float fMaxVelocity = g_fCamMotionBlurMaxVelocity;
    if (speed > fMaxVelocity) //더 길면 클램프
        velocity = (velocity / speed) * fMaxVelocity; //방향그대로 길이만 max로 자름

    //변화적으면 그대로 반환
    if (speed < 0.001f)
    {
        Out.vColor = g_TextureHDR.Sample(LinearSampler, uv);
        return Out;
    }

    // /* velocity 방향으로 샘플 누적 */
    float4 color = float4(0.f, 0.f, 0.f, 0.f);
    const float BlurSample = 12;
    
    for (int i = 0; i < BlurSample; ++i)
    {
        // /* -0.5 ~ +0.5 범위: 현재 위치 기준 양방향 샘플링 */
        float t = (float(i) / float(BlurSample - 1)) - 0.5f; //샘플수만큼 uv 위치를 -0.5~0.5범위로 만듬
        float2 sampleUV = uv + velocity * t; //화면에 이동값 * t로 어디위치에 색을 합칠지 봄

        float sampleFlag = g_TextureDepth.Sample(LinearClampSampler, sampleUV).z;
        if (sampleFlag == 6.f || sampleFlag == 12.f || sampleFlag == 8.f || sampleFlag == 2.f)
        {
            color += g_TextureHDR.Sample(LinearClampSampler, uv);
        }
        else
        {
            color += g_TextureHDR.Sample(LinearClampSampler, sampleUV);
        }
    }

    color /= float(BlurSample); //나눠서 원본밝기 유지
    Out.vColor = color;
    return Out;
}