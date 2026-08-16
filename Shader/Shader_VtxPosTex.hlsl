// Shader_VtxPosTex.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

// Texture2D + SamplerState 분리해서 선언
Texture2D<float4> g_Texture : register(t0);
Texture2D<float4> g_TextureMask : register(t1);
Texture2D<float4> g_NoiseMask : register(t2);
Texture2D<float4> g_BlurTexture: register(t3);
Texture2D<float4> g_MinimapTexture : register(t4);




// 이렇게 해도 되고
//SamplerState DefaultSampler : register(s0)
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//    AddressU = WRAP;
//    AddressV = WRAP;
//};

// 이렇게 해도 된다
// SamplerState LinearSampler : register(s0);

// 단, 위의 방식을 사용할때는 Sampler클래스를 생성할때 들어간 값과 완전히 동일해야 한다

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
   
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
  
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;  
    Out.vProjPos = vPosition;
    
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
   
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
   
    In.vTexcoord += g_ScrollTime;
    
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (vColor.a == 0.f)
        discard;
    
    Out.vColor = vColor;
    
    if (g_vColor.r >= 1.1f && g_vColor.g >= 1.1f && g_vColor.b >= 1.1f)
        Out.vColor.a = g_vColor.a;
    else
        Out.vColor.a = 1.f;
    
    
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;
    

    return Out;
}

PS_OUT PS_MAIN_COMMNET(PS_IN In)
{
    PS_OUT Out;
  

    float4 vColor = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
       
    Out.vColor = vColor;
    Out.vColor.a *= 0.6f;
    
    Out.vColor *= g_vColor;
    Out.vColor.a *= g_AlphaOffSet;
    
    return Out;
}

PS_OUT PS_MAIN_MASKING(PS_IN In)
{
    PS_OUT Out;
   
    float4 vSrc = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
    float4 vDest = g_TextureMask.Sample(LinearClampSampler, In.vTexcoord);

    if(vDest.r<0.1f)
        discard;
       
    Out.vColor = vSrc;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;
    
    return Out;
}

PS_OUT PS_MAIN_COLOR_MUL(PS_IN In)
{
    PS_OUT Out;
   
   
    float4 vTexColor = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor.rgb = vTexColor.rgb * g_vColor.rgb;

    Out.vColor *= g_UIColor;
    Out.vColor.a = vTexColor.a * g_vColor.a*g_AlphaOffSet;
  
    return Out;
}

PS_OUT PS_NO_DISCARD(PS_IN In)
{
    PS_OUT Out;
    In.vTexcoord += g_UVScroll;
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vColor;
  
    Out.vColor *= g_UIColor *g_fUIIntensity;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}

//////////프로그래스 fill이 사용하는 pass////////////////
PS_OUT PS_PROGRESS_HORIZONTAL(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (In.vTexcoord.x > g_UIProgress)
        discard;
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}


PS_OUT PS_PROGRESS_HORIZONTAL_RE(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (In.vTexcoord.x < g_UIProgress)
        discard;
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}

PS_OUT PS_PROGRESS_VERTICAL(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (In.vTexcoord.y < (1 - g_UIProgress))
        discard;
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}

PS_OUT PS_PROGRESS_VERTICAL_RE(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (In.vTexcoord.y > g_UIProgress)
        discard;
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}


PS_OUT PS_PROGRESS_RADIAL(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    //중점기준의 점
    float2 vPos = In.vTexcoord - float2(0.5f, 0.5f);
    
    // 시계방향
    float fAngle = atan2(vPos.x ,- vPos.y);
    
    //0~1로 보정
    float fProgress = (fAngle / (3.141592f * 2.0f)) + 0.5f;
    
    if (fProgress < g_UIProgress)
        discard;
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}

PS_OUT PS_PROGRESS_CENTER(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float fDistanceFromCenter = abs(In.vTexcoord.x - 0.5f);


    if (fDistanceFromCenter > g_UIProgress * 0.5f)
        discard;


    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}
PS_OUT PS_NORMAL(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (vColor.a <= 0.5f)
        discard;
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}

PS_OUT PS_NOISE(PS_IN In)
{
    PS_OUT Out;
    
    float2 vResultTexcoord = In.vTexcoord + g_UVScroll;
    float4 vNoiseColor = g_NoiseMask.Sample(LinearSampler, vResultTexcoord);

    //noise이미지의 r값(밝기)를 가져와서 비틀기정도로 사용
    float2 distortion = (vNoiseColor.rg - 0.5f) * 0.05f;
    float2 finalUV = In.vTexcoord + distortion;
    float4 vColor = g_Texture.Sample(LinearSampler, finalUV);
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}

PS_OUT PS_SMOKE_TEST(PS_IN In) : SV_TARGET
{
    PS_OUT Out;
    float4 vBase = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    // 연기 텍스처에만 애니메이션되는 UV를 적용
    float2 vScrollingUV = In.vTexcoord + g_UVScroll;
    float4 vSmoke = g_NoiseMask.Sample(LinearSampler, vScrollingUV);
    
    // 베이스 텍스처의 특정 알파값이나 마스크 영역에서만 연기를 합성
    float4 vFinal = vBase;
    if (vBase.a > 0.1f && vBase.a < 0.9f) // 경계면 즈음
    {
        vFinal.rgb += vSmoke.rgb * vSmoke.a;
    }
    
    Out.vColor = vFinal;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}

PS_OUT PS_SPRITE(PS_IN In):SV_Target0
{
    PS_OUT Out;
    
    float2 FinalUV = In.vTexcoord * g_UVScale + g_UVOffSet;
    
    float4 vColor = g_Texture.Sample(LinearSampler, FinalUV);
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;
}


PS_OUT PS_HPBarInner(PS_IN In) : SV_Target0
{
    PS_OUT Out;
    
    //g_Speed; 
                                    //출렁이는정도
    if (In.vTexcoord.x > g_UIProgress)
        discard;

    float2 noiseUV = In.vTexcoord + (g_ScrollTime * 0.2f);
    float4 vNoise = g_NoiseMask.Sample(LinearSampler, noiseUV);
    
    float2 distortedUV = In.vTexcoord;
    distortedUV.y += (vNoise.r - 0.5f) * 0.5f;
    
                        //메인텍스처가 흐르는정도
    distortedUV.x += (vNoise.g - 0.5f) * 0.5f;

    distortedUV += g_ScrollTime;
    
    float4 vColor = g_Texture.Sample(LinearSampler, distortedUV);
 
    
    Out.vColor = vColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;

    return Out;

}

PS_OUT PS_WorldDissolve(PS_IN In) : SV_Target0
{
    PS_OUT Out;
    
 
    
    float2 globalUV = In.vWorldPos.xy * 0.005f; //0.002f는 임의의 값( uv좌표를 월드좌표로 맞춰주기위해)
    
    //노이즈 텍스처 샘플링
    float noise = g_NoiseMask.Sample(LinearSampler, globalUV).r;
    
    //부모로부터 전파받은 g_UIProgress와 비교
    if (noise < (1.f - g_UIProgress))
        discard;
    
    
    //일반이미지
    float4 vTexColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vTexColor;
    Out.vColor *= g_UIColor;
    Out.vColor.a *= g_AlphaOffSet;
    
    return Out;
    

}

PS_OUT PS_BlurUI(PS_IN In) : SV_Target
{
    PS_OUT Out;
    float2 vScreenUV = In.vPosition.xy / float2(1600.0f, 900.0f);

    float4 blurColor = g_BlurTexture.Sample(LinearSampler, vScreenUV);
    float4 UIColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    
    Out.vColor = float4(blurColor.rgb, 1.0f) * g_UIColor * UIColor;
    Out.vColor.a= g_AlphaOffSet*UIColor.a;
    return Out;
}


PS_OUT PS_MinimapUI(PS_IN In):SV_Target
{
    PS_OUT Out;
    
    float2 vUV = (In.vTexcoord - 0.5f) * g_fZoom + g_vCenterUV;
    
    //바깥으로나갔을떄처리
    if (vUV.x < 0.f || vUV.x > 1.f || vUV.y < 0.f || vUV.y > 1.f)
    {
        Out.vColor = float4(0.f, 0.f, 0.f, 0.f);
        return Out;
    }
    
    
    Out.vColor = g_MinimapTexture.Sample(LinearClampSampler, vUV);
    
    return Out;
}

PS_OUT PS_CENTERED_MINIMAP(PS_IN In) : SV_Target
{
    PS_OUT Out;
    
    float2 vUV = In.vTexcoord - 0.5f;
        
    float fSin, fCos;
    sincos(g_fCameraAngle, fSin, fCos);

    float2 vRotatedUV;
    vRotatedUV.x = vUV.x * fCos - vUV.y * fSin;
    vRotatedUV.y = vUV.x * fSin + vUV.y * fCos;
    
 
    float2 vFinalUV = (vRotatedUV * g_fZoom) + g_vPlayerUV;
    
    //바깥으로나갔을떄처리
    float4 vColor = g_MinimapTexture.Sample(LinearClampSampler, vFinalUV);
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_Distortion(PS_IN In)
{
    PS_OUT Out;
    //노이즈텍스처 시간에 따라서샘플링
    float2 noiseUV = In.vTexcoord + float2(0.f, g_ScrollTime);
    float3 noise = g_NoiseMask.Sample(LinearSampler, noiseUV).rgb;
    
    //노이즈값으로  uv왜곡시키기
    float2 distortedUV = In.vTexcoord + (noise.r - 0.5) * g_fUIValue;
    

    float2 finalUV = distortedUV;
    finalUV.x += (g_ScrollTime * 0.5f);
    
    float4 vColor = g_Texture.Sample(LinearSampler, finalUV);
    

    if(vColor.a<0.4f)
        vColor *= g_UIColor;
    
    if (In.vWorldPos.x < g_fClip.x 
        || In.vWorldPos.y < g_fClip.y)
        discard;
    Out.vColor = vColor*g_fUIIntensity;
    return Out;


}

PS_OUT PS_WorldUI_Blur(PS_IN In) : SV_Target
{
    PS_OUT Out;
    float2 vScreenUV;
    vScreenUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    
    vScreenUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    float4 blurColor = g_BlurTexture.Sample(LinearSampler, vScreenUV);

    
    float4 uiTex = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (uiTex.r < 0.2f)
        discard;

    
    Out.vColor.rgb = blurColor.rgb * g_UIColor.rgb;
    Out.vColor.a = uiTex.a * g_AlphaOffSet;
    clip(uiTex.a - 0.01f);
 
    return Out;
}


PS_OUT PS_TEXT_TEST(PS_IN In) : SV_Target
{
    PS_OUT Out;
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vColor = float4(1.f, 0.f, 0.f, 1.f);
    
    Out.vColor = vColor;
    return Out;
 
}

float3 HSVtoRGB(float3 hsv)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(hsv.xxx + K.xyz) * 6.0 - K.www);
    return hsv.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), hsv.y);
}

PS_OUT PS_PaletteArea(PS_IN In) : SV_Target
{
    PS_OUT OUT;
    
    float2 uv = In.vTexcoord;
    
    float h = uv.x; //0(빨강) ~ 0.5(하늘) ~ 1.0(빨강)
    float s = 1.0f; // 채도는 일단 풀(Full)로 고정,슬라이더로 이 값전달
    float v = 1.0f - uv.y; // 위는 밝고 아래는 어둡게

    // RGB 변환
    float3 rgb = HSVtoRGB(float3(h, s, v));

    OUT.vColor = float4(rgb, 1.0f);
    return OUT;
    
}

PS_OUT  PS_TITLE(PS_IN In)
{
    PS_OUT Out;

  
    float2 seedScrollVec = float2(0.1f, 0.1f);
    float2 scrolledSeedUV = (In.vTexcoord * g_fUIValue) + (g_ScrollTime * seedScrollVec);

  
    float3 seed = g_Texture.Sample(LinearSampler, scrolledSeedUV).rgb;

  
    float2 moveVec1 = float2(0.15f, 0.15f); // 노이즈도 대각선으로
    float2 moveVec2 = float2(-0.1f, 0.2f);


    float2 uv1 = In.vTexcoord + (g_ScrollTime * moveVec1);
    float flow1 = g_NoiseMask.Sample(LinearSampler, uv1).r;

    float2 uv2 = (In.vTexcoord * 1.5f) + (g_ScrollTime * moveVec2);
    float flow2 = g_NoiseMask.Sample(LinearSampler, uv2).r;

 
    float combinedNoise = flow1 * flow2;

  
    float sharp = pow(combinedNoise, 2.0f);
    float medium = pow(combinedNoise, 0.8f) * 0.1f;
    float wide = pow(combinedNoise, 0.3f) * 0.3f;
    
    float bloomEffect = (sharp * 1.5f) + medium + wide;
    float finalSparkle = seed.r * bloomEffect;

    // 6. 결과 출력
    float pulse = (sin(g_ScrollTime * 1.5f) * 0.2f + 1.0f);
    
    Out.vColor.rgb = finalSparkle * g_UIColor.rgb * (pulse * g_fUIIntensity);
    Out.vColor.a = finalSparkle;

    return Out;
    
}

PS_OUT PS_LIGHT(PS_IN In)
{
    PS_OUT Out;

    // 1. 반짝이 알갱이 (g_Texture) - 위치 고정
    // [수정] g_ScrollTime을 더하지 않음. 타일링(g_fUIValue)만 적용해서 고정시킴.
    float2 fixedSeedUV = In.vTexcoord * g_fUIValue;
    float3 seed = g_Texture.Sample(LinearSampler, fixedSeedUV).rgb;

    // 2. 노이즈 설정 (움직이는 부분)
    // 노이즈가 흐르면서 고정된 seed 위를 지나가며 반짝임을 만듦
    float2 moveVec1 = float2(-0.1f, 0.0f); // 흐르는 방향

    // 첫 번째 노이즈: 스크롤 적용
    float2 uv1 = In.vTexcoord;
    float flow1 = g_NoiseMask.Sample(LinearSampler, uv1).r;

    // 두 번째 노이즈: 고정 (혹은 다른 방향으로 스크롤해서 깜빡임 극대화 가능)
    float2 uv2 = In.vTexcoord;
    float flow2 = g_NoiseMask.Sample(LinearSampler, uv2).r;

    float combinedNoise = flow1 * flow2;

    float sharp = pow(combinedNoise, 2.0f);
    float medium = pow(combinedNoise, 0.8f) * 0.1f;
    float wide = pow(combinedNoise, 0.3f) * 0.3f;
    
    float bloomEffect = (sharp * 1.5f) + medium + wide;
    
    // 고정된 seed의 r값과 움직이는 bloomEffect를 곱함
    float finalSparkle = seed.r * bloomEffect;

    // 4. 결과 출력
    float pulse = (sin(g_ScrollTime * 1.5f) * 0.2f + 1.0f);
    
    Out.vColor.rgb = finalSparkle * g_UIColor.rgb * (pulse * g_fUIIntensity);
    Out.vColor.a = finalSparkle;

    return Out;
}

PS_OUT PS_NORMAL_SHADERTOOL_TEST(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (vColor.a <= 0.5f)
        discard;
    
    Out.vColor = vColor;

    return Out;
}



