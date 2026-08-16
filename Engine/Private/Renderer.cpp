#include "Engine_Define.h"
#include "Renderer.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "UIComponent.h"
#include "UIObject.h"
#include "OcTree.h"
#include "InstanceBuffer.h"
#include "Mesh.h"
#include "Occlusion.h"
#include "RenderTargetManager.h"
#include "HiZBuffer.h"
#include "DebugDraw.h"

static constexpr _uint INSTANCE_THRESHOLD = 5;
static constexpr _float OCCLUDEE_MIN_AREA = 5.f;

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Renderer::Renderer()
{
}

Engine::Renderer::Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);

    m_vecObjects[UINT(RENDER_GROUP::PRIORITY)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::SHADOW)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::NONBLEND)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::NONLIGHT)].reserve(20);
    m_vecObjects[UINT(RENDER_GROUP::EFFECT_NONBLEND)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::EFFECT_BLEND)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::BLOBSHADOW)].reserve(20);
    m_vecObjects[UINT(RENDER_GROUP::BLEND)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::UI)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::CAMERA)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::SHADOW_BAKE)].reserve(10);
    m_vecObjects[UINT(RENDER_GROUP::GODRAY)].reserve(10);
}

Engine::Renderer::~Renderer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Engine::Renderer::Initialize()
{
#ifdef _DEBUG
    /* 프로파일링용 */
    //m_gpuProfiler.Init(m_pDevice, m_pContext);
#endif // _DEBUG



    // 현재 윈도우 사이즈 받아오기
    _float wincx{}, wincy{};
    m_pGameInstance->Get_Winsize(&wincx, &wincy);

    m_iViewportWidth = (_uint)wincx;
    m_iViewportHeight = (_uint)wincy;

    m_fShadowMapOffset = 1.f;

    m_iNumDSV = 4;
    m_pDSVCascade.resize(m_iNumDSV);
    m_iShadowSize = 2048; //비율상관없다
    m_iShodowBakeSize = m_iShadowSize * 4;

    //HDR
    m_vHDROption.x = 0.1f; //"BloomIntensity"
    m_vHDROption.y = 0.7f; //"HDR_Exposure", 
    m_vHDROption.z = 2.2f; //"Gamma"
    m_vHDROption.w = 0.8f; //"BrightExtract"

    //SSAO
    m_vSSAOOption.x = 1.f; //ON/OFF
    m_vSSAOOption.y = 0.5f; //SSAORadius
    m_vSSAOOption.z = 0.01f; //SSAOBias

    //칼라그레딩용
    m_vShaderDesc.g_fEnableColorGrading = 1.f; //칼라그레딩 켜고 끄기
    m_vShaderDesc.vShadowTintColor = _float3(0.85f, 0.85f, 1.15f);  //_float3(0.85f, 0.85f, 1.15f);
    m_vShaderDesc.fShadowTintWeight = 0.6f;
    m_vShaderDesc.vMidtoneTintColor = _float3(0.92f, 0.90f, 1.05f); //_float3(0.92f, 0.90f, 1.05f);
    m_vShaderDesc.fMidtoneTintWeight = 0.5f;
    m_vShaderDesc.vHighlightTintColor = _float3(0.95f, 0.93f, 1.08f); //_float3(0.95f, 0.93f, 1.08f);
    m_vShaderDesc.fHighlightTintWeight = 0.7f;
    m_vShaderDesc.fShadowRange = 0.3f;
    m_vShaderDesc.fHighLightRange = 0.5f;

    //포그용
    m_vShaderDesc.fFogEnable = 1.f; //ON/OFF
    m_vShaderDesc.vFogColor = _float3(0.05f, 0.1f, 0.25f); //색깔 (0.55f, 0.48f, 0.38f)
    m_vShaderDesc.fFogStartDist = 20.f; //포그 시작거리 //20
    m_vShaderDesc.fFogEndDist = 300.f; //포그 적용거리 //300
    m_vShaderDesc.fFogDensity = 0.00015f; //포그 빽빽함
    m_vShaderDesc.fogType = 0.f; //포그타입 0.5로 나뉨
    m_vShaderDesc.fFogBaseHeight = -62.1f; //포그 가장 짙은 높이
    m_vShaderDesc.fFogFadeHeight = 42.8f; //포그 사라질 위치
    m_vShaderDesc.fHeightFogIntensity = 2.44f; //높이 포그 강도
    m_vShaderDesc.fFogIntensity = 0.73f; //전체 포그 강도

    //Light
    m_vShaderDesc.fLightMultiplier = 1.0f; //전체조명세기
    m_vShaderDesc.fAmbientStrength = 0.15f; //앰비언트세기
    m_vShaderDesc.vToonShadowRange = _float2(0.05f, 0.25f);  //X~Y까지 부드럽게 전환(X이하값 0 Y이상값 1)
    m_vShaderDesc.vToonBrightnessRange = _float2(0.05f, 1.f); //그림자 밝은면 최소/최대(x 어두운면 y밝은면)
    m_vShaderDesc.vAmbientFloorMin = _float2(0.08f, 0.02f); //최소 앰비언트(비례값, 절대값) 둘중 큰거 사용됨 비례는 곱하는거고 절대값은 무조건 그값이상

    //SSS/SKIN
    m_vShaderDesc.vSSSColor = _float3(0.9f, 0.82f, 0.72f); //SSS색상
    m_vShaderDesc.fSSSPower = 4.f; //SSS투과정도(올리면 영역 좁아짐)
    m_vShaderDesc.fSSSIntensity = 0.1f; //SSS강도(올리면 얇은부분 더 빛남)
    m_vShaderDesc.vSkinTint = XMFLOAT3(0.873f, 0.790f, 0.736f); //피부톤
    m_vShaderDesc.fSkinRoughnessMin = 0.5f; //피부 Rough최소값

    //RimLight
    m_vShaderDesc.fRimPower = 5.f; //색상강도
    m_vShaderDesc.fRimIntensity = 5.f; //림 강도(올리면 윤곽선 강해짐)
    m_vShaderDesc.vRimMaskRange = _float2(0.1f, 0.5f); //림 마스크 범위(안씀)

    //Specular
    m_vShaderDesc.vSpecBoostRange = _float2(4.f, 1.f);//x축 매끈한면 부스트 y축 거친면 부스트 //0.07 0.3
    m_vShaderDesc.fSpecularIntensity = 1.f; //전체 스펙 세기 0.7
    m_vShaderDesc.vSpecBosstMapRange = _float2(2.f, 2.f); //맵 스페큘러 부스트
    m_vShaderDesc.fSpecularMapIntensity = 1.f; //맵 스페큘러 세기
    m_vShaderDesc.fUnderMapZeroDirSpecular = 0.005f; //지하에는 directionlight 안되게
    m_vShaderDesc.fMinDiffuse = 0.05f; //디퓨즈 보장(메탈릭이라도 까맣게 안되게?)
    m_vShaderDesc.fSpecSoftClamp = 0.2f; //옷에 spec 감소(상한선)

    //PBR관련
    m_vShaderDesc.fKValue = 8.f; //올리면 스페큘러 넓어짐
    m_vShaderDesc.fMtrlRoughnessMin = 0.15f; //물체최소 Rough 0.3해줬다가 변경하기

    //Shadow
    m_vShaderDesc.vBackFaceShaowRange = _float2(-0.2f, 0.3f); //뒷면그림자범위
    m_vShaderDesc.fShadowBright = 0.9f; //그림자 밝기
    m_vShaderDesc.fShadowBackBright = 0.3f; //뒷면 그림자 밝기
    m_vShaderDesc.fBackFillStrength = 0.7f; //뒷면 보정강도

    //GodRaty 진짜 설정용
    m_vShaderDesc.fGodRayDensity = 1.0f;    // 빛줄기 길이 (높을수록 길어짐)
    m_vShaderDesc.fGodRayDecay = 0.98f;   // 거리 감쇠 (1에 가까울수록 멀리까지)
    m_vShaderDesc.fGodRayWeight = 0.1f;    // 샘플당 기여도
    m_vShaderDesc.fGodRayExposure = 0.5f;    // 최종 밝기 다단계블러 먹이면서 1.5->0.5
    m_vShaderDesc.vGodRayColor = _float4{ 1.f, 1.f, 1.f, 1.f };   // 갓레이 색
    m_vShaderDesc.vGodRayPosition = _float4{ -122.29f, 105.36f, -153.458f, 1.f }; //-122.29f, 105.36f, -153.458f, 1.f -185.89, 95.86, -43.45, 1.f
    //GodRay메쉬 설정용
    m_vShaderDesc.fGodRayMeshScrollSpeed = 0.02f;    // UV 스크롤 속도
    m_vShaderDesc.fGodRayMeshSwayFreq = 1.f;      // 좌우 흔들림 빈도
    m_vShaderDesc.fGodRayMeshSwayPhase = 1.f;      // 높이별 위상 차이
    m_vShaderDesc.fGodRayMeshSwayAmp = 0.01f;    // 좌우 흔들림 폭
    m_vShaderDesc.vGodRayMeshColor = _float3{ 1.f, 0.9f, 0.7f }; // 빛 색상
    m_vShaderDesc.fGodRayMeshContrast = 2.0f;     // 노이즈 대비
    m_vShaderDesc.fGodRayMeshIntensity = 2.5f;      // 전체 밝기
    m_vShaderDesc.fGodRayMeshEdgeFade = 0.6f;    // 가장자리 페이드 폭
    m_vShaderDesc.fSoftParticleRange = 5.f;    //경계페이드
    m_vShaderDesc.fGodRayMeshAngleFadePower = 3.5f; //옆에서 보면 흐려지는정도
    m_vShaderDesc.fGodRayMeshFadeNear = 1.f;
    m_vShaderDesc.fGodRayMeshFadeFar = 3.5f;

    //죽순밝기
    m_vShaderDesc.fBambooEmission = 30.f; //다단블러 70 -> 30
    m_vShaderDesc.fUnderBambooEmission = 1000.f;
    //동굴 wet정도
    m_vShaderDesc.fCaveMapWet = 0.8f;

    //캠모션블러
#ifdef _DEBUG
    m_vShaderDesc.fCamMotionBlurIntensity = 1.0f; //0.3f
#else
    m_vShaderDesc.fCamMotionBlurIntensity = 1.f; //0.3f
#endif

    m_vShaderDesc.fCamMotionBlurMaxVelocity = 0.05f;
    m_vShaderDesc.fCamMotionBlurSamples = 12.f; //이거 최적화 때문에 loop해야들어가서 빼둠

    //몬스터 스페큘러
    m_vShaderDesc.fMonsterSpecularIntensity =  1.f;
    m_vShaderDesc.vSpecMonsterBoostRange = _float2(0.5f, 1.f);

    //존마다 나눌거(0 지하 1 동굴벗어나는곳 2 지상)
    m_ZonDesc[0].fExposure = 0.7f;
    m_ZonDesc[0].fLightMultiplier = 1.0f;
    m_ZonDesc[0].fAmbientStrength = 0.15f;
    m_ZonDesc[0].vFogColor = _float3(0.05f, 0.1f, 0.25f);
    m_ZonDesc[0].fFogStartDist = 20.f;
    m_ZonDesc[0].vFogType = 0.f;
    m_ZonDesc[0].vShadowTint = _float3(0.85f, 0.85f, 1.15f);
    m_ZonDesc[0].vMidtoneTint = _float3(0.92f, 0.90f, 1.05f);
    m_ZonDesc[0].vHighlightTint = _float3(0.95f, 0.93f, 1.08f);
    m_ZonDesc[0].vfUnderMapZeroDirSpecular = 0.005f;
    m_ZonDesc[0].vSpecBoostRange = _float2(4.f, 1.f); //0.07 0.3
    m_ZonDesc[0].vSpecMapBoostRange = _float2(2.f, 2.f); //0.07 0.3
    m_ZonDesc[0].vSpecMonsterBoostRange = _float2(0.5f, 1.f);
    m_ZonDesc[0].fBambooEmissionIntensity = 30.f;
    m_ZonDesc[0].fBackFillStr = 0.7f;

    //케이브이동
    m_ZonDesc[0].fCaveWet = 0.8f;
    m_ZonDesc[0].fMinMtrlRough = 0.15f;

    m_ZonDesc[1].fCaveWet = 0.1f;
    m_ZonDesc[1].fMinMtrlRough = 0.15f;
    m_ZonDesc[1].vSpecMapBoostRange = _float2(3.f, 1.f);
    m_ZonDesc[1].vSpecMonsterBoostRange = _float2(1.f, 1.f);

    //지상
    m_ZonDesc[2].fExposure = 0.7f;
    m_ZonDesc[2].fLightMultiplier = 2.0f;
    m_ZonDesc[2].fAmbientStrength = 0.3f;
    m_ZonDesc[2].vFogColor = _float3(0.55f, 0.48f, 0.38f);
    m_ZonDesc[2].fFogStartDist = 50.f;
    m_ZonDesc[2].vShadowTint = _float3(1.10f, 0.95f, 0.8f);
    m_ZonDesc[2].vMidtoneTint = _float3(1.15f, 1.05f, 0.85f);
    m_ZonDesc[2].vHighlightTint = _float3(1.15f, 1.07f, 0.78f);
    m_ZonDesc[2].vfUnderMapZeroDirSpecular = 1.f;
    m_ZonDesc[2].vSpecMonsterBoostRange = _float2(0.6f, 0.6f);
    m_ZonDesc[2].vSpecBoostRange = _float2(4.f, 1.f); //1.2 0.
    //m_ZonDesc[2].vSpecMapBoostRange = _float2(5.f, 2.f); //0.07 0.33
    m_ZonDesc[2].vFogType = 15.f;
    m_ZonDesc[2].fBambooEmissionIntensity = 300.f; //다단블러 500 -> 300
    m_ZonDesc[2].fBackFillStr = 1.2f;
    //SSAO 초기화용
    dist = uniform_real_distribution<_float>(0.0f, 1.0f); //0~1 균등하게 실수뽑아주는 분포객체
    m_iSSAO_Size = 32;

    // 렌더 타겟을 생성
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Diffuse, wincx, wincy, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Normal, wincx, wincy, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Depth, wincx, wincy, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Pick, wincx, wincy, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_U, wincx, wincy, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_MtrlSpecular, wincx, wincy, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Roughness, wincx, wincy, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Emissive, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);

    // 멀티 렌더 타겟을 등록
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_Diffuse), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_Normal), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_Depth), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_Pick), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_U), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_MtrlSpecular), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_Roughness), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GAMEOBJECT, RenderTargets::Target_Emissive), E_FAIL);

    // LightShade용 렌더 타겟 생성
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Shade, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Specular, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    // LightShade용 렌더 타겟 멀티 렌더 타겟 등록
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::LIGHTACC, RenderTargets::Target_Shade), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::LIGHTACC, RenderTargets::Target_Specular), E_FAIL);

    // Shadow용 렌더 타겟 생성
    /*CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Shadow1, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Shadow2, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Shadow3, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Shadow4, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);*/
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget_Shadow(RenderTargets::Target_Shadow1, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget_Shadow(RenderTargets::Target_Shadow2, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget_Shadow(RenderTargets::Target_Shadow3, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget_Shadow(RenderTargets::Target_Shadow4, m_iShadowSize, m_iShadowSize, DXGI_FORMAT_R32_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget_Shadow(RenderTargets::Target_Shadow_Bake, m_iShodowBakeSize, m_iShodowBakeSize, DXGI_FORMAT_R32_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    
    // Shadow용 렌더 타겟 멀티 렌더 타겟 등록
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::SHADOW, RenderTargets::Target_Shadow1), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::SHADOW, RenderTargets::Target_Shadow2), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::SHADOW, RenderTargets::Target_Shadow3), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::SHADOW, RenderTargets::Target_Shadow4), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::SHADOW_BAKE, RenderTargets::Target_Shadow_Bake), E_FAIL);

    //HDR관련
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_HDR, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(g_vHDRColor.x, g_vHDRColor.y, g_vHDRColor.z, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_HDR_EFFECT_EMISSIVE, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL); //이펙트용 emissive
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR, RenderTargets::Target_HDR), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR, RenderTargets::Target_HDR_EFFECT_EMISSIVE), E_FAIL);

    //밝은부분 추출용
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Bright, wincx * 0.5f, wincy * 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BRIGHT, RenderTargets::Target_Bright), E_FAIL);

    //다운샘플버퍼0(400 * 225)
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_DownSample, wincx * 0.25f, wincy * 0.25f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_DOWNSAMPLE, RenderTargets::Target_DownSample), E_FAIL);

    //다운샘플버퍼1(200 * 112.5)
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_DownSample1, wincx * 0.125f, wincy * 0.125f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_DOWNSAMPLE1, RenderTargets::Target_DownSample1), E_FAIL);

    //다운샘플버퍼2(100 * 56.25)
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_DownSample2, wincx * 0.0625f, wincy * 0.0625f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_DOWNSAMPLE2, RenderTargets::Target_DownSample2), E_FAIL);

    //다운샘플버퍼3(50 * 28.125)
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_DownSample3, wincx * 0.03125, wincy * 0.03125, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_DOWNSAMPLE3, RenderTargets::Target_DownSample3), E_FAIL);

    //블러버퍼0 A
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurHorizontal, wincx * 0.25f, wincy * 0.25f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURHORIZONTAL, RenderTargets::Target_BlurHorizontal), E_FAIL);

    //블러버퍼0 B
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurVertical, wincx * 0.25f, wincy * 0.25f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURVERTICAL, RenderTargets::Target_BlurVertical), E_FAIL);

    //블러버퍼1 A
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurHorizontal1, wincx * 0.125f, wincy * 0.125f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURHORIZONTAL1, RenderTargets::Target_BlurHorizontal1), E_FAIL);

    //블러버퍼1 B
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurVertical1, wincx * 0.125f, wincy * 0.125f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURVERTICAL1, RenderTargets::Target_BlurVertical1), E_FAIL);

    //블러버퍼2 A
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurHorizontal2, wincx * 0.0625f, wincy * 0.0625f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURHORIZONTAL2, RenderTargets::Target_BlurHorizontal2), E_FAIL);

    //블러버퍼2 B
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurVertical2, wincx * 0.0625f, wincy * 0.0625f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURVERTICAL2, RenderTargets::Target_BlurVertical2), E_FAIL);

    //블러버퍼3 A
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurHorizontal3, wincx * 0.03125f, wincy * 0.03125f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURHORIZONTAL3, RenderTargets::Target_BlurHorizontal3), E_FAIL);

    //블러버퍼3 B
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_BlurVertical3, wincx * 0.03125f, wincy * 0.03125f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR_BLURVERTICAL3, RenderTargets::Target_BlurVertical3), E_FAIL);
    
    //SSAO버퍼
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_SSAO, wincx, wincy, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::SSAO, RenderTargets::Target_SSAO), E_FAIL);

    //SSAO블러버퍼
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_SSAOBLur, wincx, wincy, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::SSAO_Blur, RenderTargets::Target_SSAOBLur), E_FAIL);

    //GodRay버퍼
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_GodRayCopy, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GODRAYCOPY, RenderTargets::Target_GodRayCopy), E_FAIL);

    //GodRay radial블러 버퍼
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_GodRay, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::GODRAY, RenderTargets::Target_GodRay), E_FAIL);

    //카메라 모션블러먹이기 위해 velocity계산
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_CamVelocity, wincx, wincy, DXGI_FORMAT_R16G16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::CAMVELOCITY, RenderTargets::Target_CamVelocity), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::HDR, RenderTargets::Target_CamVelocity), E_FAIL);

    //물체 모션 + 카메라 모션저장용
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_VelocityOut, wincx, wincy, DXGI_FORMAT_R16G16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::VELOCITYOUT, RenderTargets::Target_VelocityOut), E_FAIL);

    //카메라 모션블러
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_CamMotionBlur, wincx, wincy, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::CAMMOTIONBLUR, RenderTargets::Target_CamMotionBlur), E_FAIL);

    //UI블러버퍼
    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_UI_BlurHorizontal, wincx * 0.25f, wincy * 0.25f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::UI_BLURHORIZONTAL, RenderTargets::Target_UI_BlurHorizontal), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_RenderTarget(RenderTargets::Target_UI_BlurVertical, wincx * 0.25f, wincy * 0.25f, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_MultiRenderTarget(MRT::UI_BLURVERTICAL, RenderTargets::Target_UI_BlurVertical), E_FAIL);

    // 행렬 설정
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(wincx, wincy, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(wincx, wincy, 0.f, 1.f));

    m_tCameraBuffer.g_ViewMatrix = m_ViewMatrix;
    m_tCameraBuffer.g_ProjMatrix = m_ProjMatrix;


    // 쉐이더에 등록할 Texture2D용 버퍼 생성
    m_pVIBuffer = VIBuffer_Rect::Create(m_pDevice, m_pContext);
    CHECK_NULLPTR(m_pVIBuffer);

    //그림자용 깊이버퍼(Offset들어있음)
    //Ready_DepthStencilView();
    Ready_DepthStencilViewCasCade();

    Ready_Shader();

    // 백버퍼 텍스처 복사용 초기화
    Ready_TextureSRV(wincx, wincy);

    //UI블러용 초기화
    Ready_TextureSRV_For_BlurUI(wincx, wincy);

    //SSAO초기화
    Ready_SSAOSetUp();
 
    // 최대 인스턴스 개수 5000개의 InstanceBuffer 생성
    m_iMaxInstanceNum = 5000;
    m_pInstanceBuffer = InstanceBuffer::Create(m_pDevice, m_pContext, m_iMaxInstanceNum);
    CHECK_NULLPTR(m_pInstanceBuffer);


#ifdef _DEBUG
    Initialize_DebugDraw();
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Diffuse, 100.f, 100.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Normal, 300.f, 100.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Shade, 500.f, 100.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_U, 700.f, 100.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Roughness, 900.f, 100.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_MtrlSpecular, 1100.f, 100.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Emissive, 1300.f, 100.f, 200.f, 200.f), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Shadow1, 100.f, 300.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Shadow2, 300.f, 300.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Shadow3, 500.f, 300.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Shadow4, 700.f, 300.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Shadow_Bake, 900.f, 300.f, 200.f, 200.f), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Specular, 100.f, 500.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_HDR, 300.f, 500.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_Bright, 500.f, 500.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_SSAO, 700.f, 500.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_SSAOBLur, 900.f, 500.f, 200.f, 200.f), E_FAIL);
    //CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_DownSample, 500.f, 500.f, 200.f, 200.f), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_BlurHorizontal, 100.f, 700.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_BlurVertical, 300.f, 700.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_GodRayCopy, 500.f, 700.f, 200.f, 200.f), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Ready_RT_Debug(RenderTargets::Target_HDR_EFFECT_EMISSIVE, 700.f, 700.f, 200.f, 200.f), E_FAIL);

    
#endif

    m_vecInstanceGroup.resize(400);
    //m_pOcclusion = Occlusion::Create(m_pDevice, m_pContext, 640, 360); // ( 저해상도 256 x 144) (팝핑 현상 일어나면 더 키우기)
    //CHECK_NULLPTR(m_pOcclusion);

    //m_pHiZBuffer = HiZBuffer::Create(m_pDevice, m_pContext, wincx, wincy);
    //CHECK_NULLPTR(m_pHiZBuffer);

    //m_gpuProfiler.Init(m_pDevice, m_pContext);

    return S_OK;
}

//그림자용?
HRESULT Engine::Renderer::Ready_DepthStencilView()
{
    ID3D11Texture2D* pDepthStencilTexture = { nullptr };

    D3D11_TEXTURE2D_DESC	TextureDesc{};

    _float wincx{}, wincy{};
    m_pGameInstance->Get_Winsize(&wincx, &wincy);

    /* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 테스트가 가능해진다. */
    /* 픽셀의 수가 다르면 아에 렌더링을 못함. */
    TextureDesc.Width = (_uint)(wincx * m_fShadowMapOffset);
    TextureDesc.Height = (_uint)(wincy * m_fShadowMapOffset);
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    /* 동적? 정적?  */
    TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
    /* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
        /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDSV)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);
    //깃 테스트용
    return S_OK;
}

HRESULT Engine::Renderer::Ready_DepthStencilViewCasCade()
{
    ID3D11Texture2D* pDepthStencilTexture = { nullptr };

    D3D11_TEXTURE2D_DESC	TextureDesc{};

    _float wincx{}, wincy{};
    m_pGameInstance->Get_Winsize(&wincx, &wincy);

    /* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 테스트가 가능해진다. */
    /* 픽셀의 수가 다르면 아에 렌더링을 못함. */
    TextureDesc.Width = (_uint)(wincx * m_fShadowMapOffset);
    TextureDesc.Height = (_uint)(wincy * m_fShadowMapOffset);
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    /* 동적? 정적?  */
    TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
    /* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
        /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;


    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDSV)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);

    return S_OK;
}

HRESULT Engine::Renderer::Ready_Shader()
{
    // 쉐이더 진입점 등록
    SHADERENTRY entry[22] = {
        {"VS_MAIN", "PS_DEBUG_MAIN"},                  //0
        {"VS_MAIN", "PS_MAIN_DIRECTIONAL"},            //1
        {"VS_MAIN", "PS_MAIN_POINT"},                  //2
        {"VS_MAIN", "PS_MAIN_DIRECTIONAL_TOON" },      //3
        {"VS_MAIN", "PS_MAIN_DIRECTIONAL_PBR" },       //4
        {"VS_MAIN", "PS_MAIN_SPOTLIGHT" },             //5
        {"VS_MAIN", "PS_MAIN_COMBINED"},               //6
        {"VS_MAIN", "PS_MAIN_COMBINED_CASCADETEST" },  //7
        {"VS_MAIN", "PS_MAIN_HDR" },                   //8
        {"VS_MAIN", "PS_MAIN_HDR_BrightExtract" },     //9
        {"VS_MAIN", "PS_MAIN_HDR_DownSampling" },      //10
        {"VS_MAIN", "PS_MAIN_HDR_BlurHorizontal" },    //11
        {"VS_MAIN", "PS_MAIN_HDR_BlurVertical" },      //12
        {"VS_MAIN", "PS_MAIN_HDR_UpSampling" },        //13
        {"VS_MAIN", "PS_MAIN_HDR_MAPPING" },           //14
        {"VS_MAIN", "PS_MAIN_SSAO" },                  //15
        {"VS_MAIN", "PS_MAIN_SSAO_BLUR" },             //16
        {"VS_MAIN", "PS_MAIN_GODRAY_COPY" },           //17
        {"VS_MAIN", "PS_MAIN_GODRAY" },                //18
        {"VS_MAIN", "PS_MAIN_CAMVELOCITY" },           //19
        {"VS_MAIN", "PS_MAIN_CAMMOTIONBLUR" },         //20
        {"VS_MAIN", "PS_MAIN_HDR_UpSamplingPlus" },    //21
    };
    SHADERENTRIES entries{};
    entries.pEntries = entry;
    entries.iNumpass = sizeof(entry) / sizeof(SHADERENTRY);
    
    // 디퍼드 쉐이더 생성
    m_pShader = Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Deferred.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries);

    SHADERENTRY InstanceEntry[2] = {
        {"VS_MAIN", "PS_MAIN"},
        {"VS_MAIN_SHADOW", "PS_MAIN_SHADOW"}
    };
    entries.pEntries = InstanceEntry;
    entries.iNumpass = sizeof(InstanceEntry) / sizeof(SHADERENTRY);


    m_pGameInstance->Add_Shader(L"Prototype_Component_Shader_VtxInstance", L"../../Shader/Shader_VtxInstance.hlsl", VTXINSTANCEMESH::Elements, VTXINSTANCEMESH::iNumElements, entries);
    m_pInstanceShader = RCAST(Shader*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Shader_VtxInstance"));

    /* 순수 카메라버퍼 + World행렬 바인딩용 */
    SHADERENTRY entrybind[1] = {
    {"VS_MAIN", "PS_BIND_ONLY"}
    };
    entries.pEntries = entrybind;
    entries.iNumpass = sizeof(entrybind) / sizeof(SHADERENTRY);

    // 사용하는 쉐이더 아님 진짜 버퍼 바인딩용임 그래도 지우지마 지우면 누가 지웠는지 찾아내서 발표때 그 사람에게 끌리는 광역 어그로를 끌어버릴것
    m_pOnlyBindingShader = Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Deferred.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries);

    return S_OK;
}

HRESULT Engine::Renderer::Ready_TextureSRV(const _float fWinCX, const _float fWinCY)
{
  

    // 백버퍼와 같은 사이즈, 포맷으로 텍스처 생성
    D3D11_TEXTURE2D_DESC    textureDesc{};
    textureDesc.Width = (_uint)fWinCX;
    textureDesc.Height = (_uint)fWinCY;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    CHECK_FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pSceneTexture), E_FAIL);

    // 쉐이더에서 Texture2D 만든거 읽어야 되니까 SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pSceneTexture, &srvDesc, &m_pSceneSRV), E_FAIL);

    return S_OK;
}
HRESULT Engine::Renderer::Ready_TextureSRV_For_BlurUI(const _float fWinCX, const _float fWinCY)
{
    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = (_uint)fWinCX;
    textureDesc.Height = (_uint)fWinCY;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 백버퍼와 동일하게
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

    if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pSceneTexture_ForUI)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pSceneTexture_ForUI, nullptr, &m_pSceneSRV_ForUI)))
        return E_FAIL;

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Renderer::Add_RenderObject(RENDER_GROUP _rendergroup, GameObject* _gameobject)
{
    MSG_NULL(_gameobject, L"전달받을 객체가 없습니다.", L"렌더러 추가 실패", E_FAIL);

    m_vecObjects[UINT(_rendergroup)].push_back(_gameobject);

    Safe_AddRef_PerFrame(_gameobject);

    return S_OK;
}

HRESULT Engine::Renderer::ResizeBuffers(_float _width, _float _height)
{
    // 버퍼의 크기가 변했기 때문에 행렬 갱신 필요
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(_width, _height, 1.f));
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(_width, _height, 0.f, 1.f));

    m_iViewportWidth = (_uint)_width;
    m_iViewportHeight = (_uint)_height;

    Safe_Release(m_pDSV);
    CHECK_FAILED(Ready_DepthStencilView(), E_FAIL);

    // Hi-Z 리사이즈
    //if (m_pHiZBuffer)
    //    m_pHiZBuffer->Resize(m_iViewportWidth, m_iViewportHeight);

    return S_OK;
}
/******************************************************* 컨테이너 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Renderer::Render(const _float fTimeDelta)
{
    if (m_pGameInstance->KeyDown(DIK_G))
    {
        m_bCapture = !m_bCapture;
    }
    //COUT(m_vHDROption.x);
    _uint currentElementCount = 0;

    m_pContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    m_pGameInstance->Clear_RTV(MRT::HDR);

    //m_pGameInstance->Bind_PipeLineMatrix(m_pShader, "g_ViewMatrix", D3DTS_VIEW);
    //m_pGameInstance->Bind_PipeLineMatrix(m_pShader, "g_ProjMatrix", D3DTS_PROJ);
    //m_pGameInstance->Bind_CameraPosition(m_pShader, "g_vCamPosition");
    //m_pShader->Bind_Resources(0);

    // 업데이트 된 역행렬을 구조체에 복사(구조체 한 번에 던지기 위해)
    m_tCameraBuffer.g_InverseProjMatrix = m_pGameInstance->Get_PipeLineInversMatrix(D3DTS_PROJ);
    m_tCameraBuffer.g_InverseViewMatrix = m_pGameInstance->Get_PipeLineInversMatrix(D3DTS_VIEW);
    m_tCameraBuffer.g_vCamPosition = m_pGameInstance->Get_CameraPosition();
    ////SSAO용
    memcpy(&m_tCameraBuffer.g_Samples, ssaoKernel, sizeof(_float4) * m_iSSAO_Size);
    m_tCameraBuffer.g_SSAORadius = m_vSSAOOption.y; //0.5f;
    m_tCameraBuffer.g_SSAOBias = m_vSSAOOption.z; //0.01;
    m_tCameraBuffer.g_NoiseScale.x = m_iViewportWidth / 4.f;
    m_tCameraBuffer.g_NoiseScale.y = m_iViewportHeight / 4.f;
    m_tCameraBuffer.g_CamViewMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_VIEW);
    m_tCameraBuffer.g_CamProjMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_PROJ);
    m_tCameraBuffer.g_PrevViewMatrix = m_pGameInstance->Get_PrevPipeLineMatrix(D3DTRANSFORM::D3DTS_VIEW);
    m_tCameraBuffer.g_PrevProjMatrix = m_pGameInstance->Get_PrevPipeLineMatrix(D3DTRANSFORM::D3DTS_PROJ);
    m_pGameInstance->Bind_PipeLine_All(m_pOnlyBindingShader);
    m_pOnlyBindingShader->Bind_Resources(0);

    Query_StaticObjects_ForInstancing();

    m_pShader->Bind_EntireBuffer_BySlot(5, &m_vShaderDesc, sizeof(ShaderDesc));

    // 그림자 그리기용 RT을 먼저 그려줌
    Render_Shadow(fTimeDelta);
    if (m_bCapture)
    {
        Render_Shadow_Bake(fTimeDelta); //모든 모델400의 모델 그림자를 그리고
        m_bCapture = false; //한번굽고 끄기
    }
        
    {   /* MRT 한 번 교체로 묶음 */

        /* Diffuse + Normal 을 따로 RTV에 그린다 */
        CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::GAMEOBJECT, m_pDSV, true), S_OK);

        // 가장 먼저 그려질 객체들(스카이박스, 배경 등)
//m_pGameInstance->Begin_RT(RenderTargets::Target_Diffuse, m_pDSV);
        Render_Priority(fTimeDelta);

        m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
        m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
        m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

        //m_gpuProfiler.Begin();

        // 블렌딩이 들어가지 않을 객체들
        Render_NonBlend(fTimeDelta);
        Render_NonBlend_Instanced(fTimeDelta);

        //_double gpuMs = m_gpuProfiler.End();
        //_string outputmsg = to_string(gpuMs) + "\n";
        //OutputDebugStringA(ou tputmsg.c_str());

        CHECK_FAILED(m_pGameInstance->End_MRT(), S_OK);
    }   /* MRT 한 번 교체로 묶음 */

    // MRT에 등록된 애들을 제대로 그리기 위해 직교투영 행렬로 다시 맞춰줌
    m_pOnlyBindingShader->Bind_Matrix_ByHandle(g_WorldMatrix, m_WorldMatrix);
    m_pOnlyBindingShader->Bind_EntireBuffer_BySlot(BUFFER_CAMERA, &m_tCameraBuffer, sizeof(Camera_Buffer));
    m_pOnlyBindingShader->Bind_Resources(0);

    //SSAO
    Render_SSAO(fTimeDelta);
    Render_SSAO_Blur(fTimeDelta);
    

    // 노말 텍스쳐를 기준으로 빛을을 계산할 객체들
    Render_Lights(fTimeDelta);

    // 합성 단계
    Render_Combined(fTimeDelta); //여기 앞에있는거밖에 픽셀피킹안됨 

    //갓레이
    Render_GodRayCopy(fTimeDelta);
    Render_GodRay(fTimeDelta);
    // 깊이값이 다 저장된 뒤에 오클루전 비교를 위해 Depth 가져옴
    //ID3D11ShaderResourceView* pDepthSRV = m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Depth);
    //if (pDepthSRV != nullptr)
    //    m_pHiZBuffer->Generate_MipChain(pDepthSRV);
   

    // World객체들을 그려주기 위해 다시 투영행렬로 바꿔줌
    m_pGameInstance->Bind_PipeLine_All(m_pOnlyBindingShader);
    m_pOnlyBindingShader->Bind_Resources(0);

#ifdef _DEBUG
    if (g_toolType == TOOLTYPE::ANIMATION_TOOL || g_toolType == TOOLTYPE::UI_TOOL)
        Render_Grid();
#endif // _DEBUG    


    Render_Decal(fTimeDelta);

    Render_NonLight(fTimeDelta);

    //Render_BlobShadow(fTimeDelta);

    Render_Blend(fTimeDelta);

    Render_GodRayMesh(fTimeDelta);

    //월드 ui
    Render_WorldUI(fTimeDelta);

    //백 버퍼의 현재 상태를 복사
    ID3D11RenderTargetView* pBackRTV = m_pGameInstance->Get_RenderTargetView(RenderTargets::Target_HDR);
    ID3D11Resource* pBackBuffer = nullptr;
    pBackRTV->GetResource(&pBackBuffer);                        // RTV에서 실제 텍스처 정보 꺼내기
    m_pContext->CopyResource(m_pSceneTexture, pBackBuffer);     // 복사
    pBackBuffer->Release();                                     // Get_Resource가 AddRef를 한다네요
    m_pContext->PSSetShaderResources(2, 1, &m_pSceneSRV);

    Render_Effect_NonBlend(fTimeDelta);
    Render_Effect_Blend(fTimeDelta);            // 이펙트를 여기서 그리니까 이거 이전에 해야함(?)
    
    Render_Camera(fTimeDelta);

    //HDR은 제일 마지막에 적용한다
    m_pOnlyBindingShader->Bind_Matrix_ByHandle(g_WorldMatrix, m_WorldMatrix);
    m_pOnlyBindingShader->Bind_EntireBuffer_BySlot(BUFFER_CAMERA, &m_tCameraBuffer, sizeof(Camera_Buffer));
    m_pOnlyBindingShader->Bind_Resources(0);

    //HDR
    Render_HDR_Copy(fTimeDelta);
    //카메라블러
    Render_CamVelocity(fTimeDelta);
    Render_CamMotionBLur(fTimeDelta);

    Render_HDR_BrightExtract(fTimeDelta); 
    Render_HDR_DownSampling(fTimeDelta);
    Render_HDR_BlurHorizontal(fTimeDelta);
    Render_HDR_BlurVertical(fTimeDelta);
    Render_HDR_UPSamplingPlus(fTimeDelta);
    Render_HDR_UpSampling(fTimeDelta);

    Render_HDR(fTimeDelta);

    //m_pGameInstance->Bind_PipeLineMatrix_View(m_pShader);
    //m_pGameInstance->Bind_PipeLineMatrix_Proj(m_pShader);
    ////m_pGameInstance->Bind_CameraPosition(m_pShader);
    //m_pShader->Bind_Matrix_ByHandle(g_WorldMatrix, m_WorldMatrix);
    //m_pShader->Bind_Matrix_ByHandle(g_ViewMatrix, m_ViewMatrix);
    //m_pShader->Bind_Matrix_ByHandle(g_ProjMatrix, m_ProjMatrix);
    //m_pShader->Commit(0);
    
    Render_UI(fTimeDelta);


#ifdef _DEBUG
    //m_pGameInstance->Bind_PipeLineMatrix_View(m_pShader);
    //m_pGameInstance->Bind_PipeLineMatrix_Proj(m_pShader);
    if (g_bDrawRTV)
        Render_DebugRTV(fTimeDelta);

    //m_pGameInstance->Bind_PipeLine_All(m_pOnlyBindingShader);
    //m_pOnlyBindingShader->Bind_Resources(0);
    if(g_bDrawDebugCollider)
    {
        Render_Debug(fTimeDelta);
    }


    // 렌더모드 아닐때 혹시라도 계속 추가될 수 있으니까 클리어
    if(!m_DebugComponents.empty())
    {
        for (auto& debugCom : m_DebugComponents)
        {
            if (debugCom != nullptr)
            {
                Safe_Release(debugCom);
            }
        }
        m_DebugComponents.clear();
    }
    if(!m_vecDebugSphere.empty())
        m_vecDebugSphere.clear();
    if (!m_vecDebugCapsule.empty())
        m_vecDebugCapsule.clear();
    if (!m_vecDebugLine.empty())
        m_vecDebugLine.clear();
    if (!m_vecDebugFan.empty())
        m_vecDebugFan.clear();
#endif

    _uint totalCount{};
    for (_uint i = 0; i < _UINT(RENDER_GROUP::END); ++i)
    {
        totalCount += m_iMaxGroupElementCount[i];
    }

    if (m_iMaxElementCount < totalCount)
        m_iMaxElementCount = totalCount;

    Clear_Renderer();

    //카메라 모션블러 위해서 이전매트릭스 저장
    m_pGameInstance->Update_PrevMatrices();

    return S_OK;
}


void Engine::Renderer::Render_Priority(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::PRIORITY)].empty())
        return;

    m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::SKY);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    for (auto& priority : m_vecObjects[UINT(RENDER_GROUP::PRIORITY)])
    {
        if (priority != nullptr)
            priority->Render(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::PRIORITY)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::PRIORITY)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::PRIORITY)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::PRIORITY)].size();

}

void Engine::Renderer::Render_Shadow(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::SHADOW)].empty())
        return;

    m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_SHADOW);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    Setup_ViewportDesc(m_iShadowSize, m_iShadowSize);

    m_pDSVShadow = m_pGameInstance->Begin_MRT_ShadowCascade(MRT::SHADOW, m_vecObjects, fTimeDelta, m_iNumDSV, RENDER_GROUP::SHADOW);
    if (m_pDSVShadow == nullptr)
        return;

    for (int i = 0; i < m_iNumDSV; i++)
    {
        m_pContext->ClearDepthStencilView(m_pDSVShadow[i], D3D11_CLEAR_DEPTH, 1.f, 0);
        // [Modified] Vector Access
        //m_vecMultiRenderTargets[iIndex][i]->ClearRTV();

        m_pContext->OMSetRenderTargets(0, nullptr, m_pDSVShadow[i]);
        
        //여기서 공통적으로 해줘야함
        //다른거라서 그런가?(근데 이전에 무기는 됐는데?)
        if (m_vecObjects[UINT(RENDER_GROUP::SHADOW)][0])
            m_pShaderShadowCom = m_vecObjects[UINT(RENDER_GROUP::SHADOW)][0]->Get_Shader();

        m_pShaderShadowCom->Bind_RawValue_ByHandle(g_ShadowCascadeNum, &i, sizeof(_int));
        m_pShaderShadowCom->Bind_Matrix_ByHandle(g_ShadowCascade_ViewProjMatrix, m_pGameInstance->Get_ShadowCascade_Matrix(D3DTRANSFORM::D3DTS_VIEWPROJ)[(i + 1) - 1]);
        //여기서 돌면 15000
        for (auto& shadow : m_vecObjects[UINT(RENDER_GROUP::SHADOW)])
        {
            if (shadow != nullptr)
                shadow->Render_Shadow(fTimeDelta, i + 1);
        }
    }

    //Setup_ViewportDesc(410.f, 720.f);


    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::SHADOW)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::SHADOW)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::SHADOW)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::SHADOW)].size();

    m_pGameInstance->End_MRT();

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);

}

void Engine::Renderer::Render_Shadow_Bake(const _float fTimeDelta)
{
    //임시로 주석처리


    m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_SHADOW);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    Setup_ViewportDesc(m_iShodowBakeSize, m_iShodowBakeSize);

    m_pDSVShadow = m_pGameInstance->Begin_MRT_ShadowCascade(MRT::SHADOW_BAKE, m_vecObjects, fTimeDelta, 1, RENDER_GROUP::SHADOW_BAKE);
    if (m_pDSVShadow == nullptr)
        return;

    m_pContext->ClearDepthStencilView(m_pDSVShadow[0], D3D11_CLEAR_DEPTH, 1.f, 0);

    //무조건 클리어하도록
    if (m_vecObjects[UINT(RENDER_GROUP::SHADOW_BAKE)].empty())
    {
        m_pGameInstance->End_MRT();
        return;
    }

    for (int i = 0; i < 1; i++)
    {
        m_pContext->OMSetRenderTargets(0, nullptr, m_pDSVShadow[i]);

        if (m_vecObjects[UINT(RENDER_GROUP::SHADOW_BAKE)][0])
            m_pShaderShadowCom = m_vecObjects[UINT(RENDER_GROUP::SHADOW_BAKE)][0]->Get_Shader();

        m_pShaderShadowCom->Bind_RawValue_ByHandle(g_ShadowCascadeNum, &i, sizeof(_int));
        m_pShaderShadowCom->Bind_Matrix_ByHandle(g_ShadowCascade_ViewProjMatrix, m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEWPROJ));
        for (auto& shadow : m_vecObjects[UINT(RENDER_GROUP::SHADOW_BAKE)])
        {   
            if (shadow != nullptr)
                shadow->Render_Shadow(fTimeDelta, i + 1);
        }
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::SHADOW_BAKE)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::SHADOW_BAKE)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::SHADOW_BAKE)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::SHADOW_BAKE)].size();

    m_pGameInstance->End_MRT();

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);
}

void Engine::Renderer::Render_NonBlend(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::NONBLEND)].empty())
        return;

    for (auto& nonblend : m_vecObjects[UINT(RENDER_GROUP::NONBLEND)])
    {
        if (nonblend == nullptr)
            continue;

        // 맵 오브젝트 그룹은 스텐실 1 기록
        if (nonblend->Get_OBJType() == OBJTYPE::TYPE_MAP)
            m_pGameInstance->Set_DepthStencilState(DSSTATE::STENCIL_WRITE, 1);
        else
            m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);

        nonblend->Render(fTimeDelta);
    }

    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::NONBLEND)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::NONBLEND)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::NONBLEND)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::NONBLEND)].size();
}

void Engine::Renderer::Render_Lights(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::NODEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::ADDITIVE);

    /* Shade를 따로 RTV에 그린다 */
    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::LIGHTACC), );

    // 26.02.11 Mtrl 지움
    //_float4 mtrlAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    //_float4 mtrlSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    //m_pShader->Bind_RawValue_ByHandle(g_vMtrlAmbient, &mtrlAmbient, sizeof(_float4));
    //m_pShader->Bind_RawValue_ByHandle(g_vMtrlSpecular, &mtrlSpecular, sizeof(_float4));

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Diffuse, m_pShader, 1); //diffuse추가
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Normal, m_pShader, 2);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_U, m_pShader, 7);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_MtrlSpecular, m_pShader, 14);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Roughness, m_pShader, 15);
    
    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer, fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_GodRayCopy(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::GODRAYCOPY), );

    //m_pShader->Bind_SRV("g_TextureHDR", m_pSceneSRV);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_HDR, m_pShader, 11); //diffuse추가
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Emissive, m_pShader, 16);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4);

    m_pShader->Begin(_UINT(DEFERRED::GODRAY_COPY));
    m_pShader->Bind_Resources(_UINT(DEFERRED::GODRAY_COPY));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_GodRay(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::GODRAY), );

    //m_pShader->Bind_SRV("g_TextureHDR", m_pSceneSRV);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_GodRayCopy, m_pShader, 21);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Pick, m_pShader, 16);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4);

    m_pShader->Begin(_UINT(DEFERRED::GODRAY));
    m_pShader->Bind_Resources(_UINT(DEFERRED::GODRAY));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_GodRayMesh(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::GODRAY)].empty())
        return;
    //이거 갓레이 메쉬용으로 쓰고 있어서 SOLID_NONE 해놓은거 이거 쓰면 말해줘 다른렌더로 빼든지함
    m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::ADDITIVE); //논블렌드가 아니라 가산(빛있는애들할때?)

    CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV), );
    m_pContext->PSSetShaderResources(2, 1, &m_pSceneSRV);

    ID3D11ShaderResourceView* pDepthSRV = m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Depth);
    m_pContext->PSSetShaderResources(30, 1, &pDepthSRV);

    for (auto& effect : m_vecObjects[UINT(RENDER_GROUP::GODRAY)])
    {
        if (effect != nullptr)
            effect->Render(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::GODRAY)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::GODRAY)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::GODRAY)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::GODRAY)].size();

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_Combined(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::NODEPTH); //NODEPTH
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV, false), );

    //이것도 고쳐야되고
  /*  m_pShader->Bind_Matrix_FullSlot(BUFFER_LIGHT, "g_LightViewMatrix", m_pGameInstance->Get_LightMatrix(D3DTS_VIEW));
    m_pShader->Bind_Matrix_FullSlot(BUFFER_LIGHT, "g_LightProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTS_PROJ));*/
    m_pShader->Bind_RawValue_ByHandle(g_ShadowSplit, &m_pGameInstance->Get_ShadowCascade_Split(), sizeof(_float4));
    m_pShader->Bind_RawValue_ByHandle(g_ShadowDir, &m_pGameInstance->Get_ShadowLightDir(), sizeof(_float4));
    m_pShader->Bind_Matrices_ByHandle(g_ShadowCascade_MixMaxtrix, m_pGameInstance->Get_ShadowCascade_Matrix(D3DTRANSFORM::D3DTS_VIEWPROJ), 4);
    m_pShader->Bind_Matrix_ByHandle(g_Shadow_ViewProjMatrix, m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEWPROJ));
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Diffuse, m_pShader, 1);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Normal, m_pShader, 2);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Shade, m_pShader, 3);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Specular, m_pShader, 5);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Shadow1, m_pShader, 6);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Shadow2, m_pShader, 8);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Shadow3, m_pShader, 9);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Shadow4, m_pShader, 10);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Emissive, m_pShader, 16);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_SSAOBLur, m_pShader, 18);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Shadow_Bake, m_pShader, 19);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Roughness, m_pShader, 15);
  
    m_pShader->Bind_RawValue_FullSlot(11, "g_SSAOOption", &m_vSSAOOption, sizeof(_float4));

    m_pShader->Begin(_UINT(DEFERRED::COMBINED_CASCADE)); //
    m_pShader->Bind_Resources(_UINT(DEFERRED::COMBINED_CASCADE)); //5

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_NonLight(const _float fTimeDelta)
{
    //이거 원래대로 되돌려놓고 갓레이메쉬용 따로 빼서 사용
    if (m_vecObjects[UINT(RENDER_GROUP::NONLIGHT)].empty())
        return;
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::BLEND);

    CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV), );
    m_pContext->PSSetShaderResources(2, 1, &m_pSceneSRV);

    for (auto& effect : m_vecObjects[UINT(RENDER_GROUP::NONLIGHT)])
    {
        if (effect != nullptr)
            effect->Render(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::NONLIGHT)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::NONLIGHT)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::NONLIGHT)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::NONLIGHT)].size();

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_BlobShadow(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::BLOBSHADOW)].empty())
        return;

    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::BLEND);

    for (auto& blobshadow : m_vecObjects[UINT(RENDER_GROUP::BLOBSHADOW)])
    {
        if (blobshadow != nullptr)
            blobshadow->Render_BlobShadow(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::BLOBSHADOW)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::BLOBSHADOW)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::BLOBSHADOW)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::BLOBSHADOW)].size();
}

void Engine::Renderer::Render_Decal(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::DECAL)].empty())
        return;

    // 렌더 스테이트 설정
    m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DECAL_READ, 1);         // 스텐실이 1인 것만 통과
    m_pGameInstance->Set_BlendState(BSTATE::BLEND);

    // HDR 타겟에 그리기 (Combined가 쓴 같은 타겟)
    CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV, false), );

    // Target_Depth SRV를 t4에 바인딩 (데칼 셰이더에서 깊이 복원에 사용)
    ID3D11ShaderResourceView* pDepthSRV = m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Depth);
    m_pContext->PSSetShaderResources(4, 1, &pDepthSRV);

    for (auto& decal : m_vecObjects[UINT(RENDER_GROUP::DECAL)])
    {
        if (decal != nullptr)
            decal->Render(fTimeDelta);
    }

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_Effect_NonBlend(const _float fTimeDelta)
{
    //이거 원래대로 되돌려놓고 갓레이메쉬용 따로 빼서 사용
    if (m_vecObjects[UINT(RENDER_GROUP::EFFECT_NONBLEND)].empty())
        return;
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::NONBLEND);

    CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV), );
    m_pContext->PSSetShaderResources(2, 1, &m_pSceneSRV);

    for (auto& effect : m_vecObjects[UINT(RENDER_GROUP::EFFECT_NONBLEND)])
    {
        if (effect != nullptr)
            effect->Render(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::EFFECT_NONBLEND)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::EFFECT_NONBLEND)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::EFFECT_NONBLEND)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::EFFECT_NONBLEND)].size();

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_Effect_Blend(const _float fTimeDelta) //모든 이펙트가 이경로를 탐
{
    if (m_vecObjects[UINT(RENDER_GROUP::EFFECT_BLEND)].empty())
        return;

    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::BLEND); //알파값으로 섞기 반반 

   CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV), );
   m_pContext->PSSetShaderResources(2, 1, &m_pSceneSRV);

   // Soft Particle 용으로 사용할 깊이 텍스처 바인딩
   ID3D11ShaderResourceView* pDepthSRV = m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_Depth);
   m_pContext->PSSetShaderResources(30, 1, &pDepthSRV);

    for (auto& effect : m_vecObjects[UINT(RENDER_GROUP::EFFECT_BLEND)])
    {
        if (effect != nullptr)
            effect->Render(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::EFFECT_BLEND)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::EFFECT_BLEND)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::EFFECT_BLEND)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::EFFECT_BLEND)].size();

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_Blend(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::BLEND)].empty())
        return;

    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::BLEND);

    m_pGameInstance->Reset_MRT();

    CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV), );

    for (auto& blend : m_vecObjects[UINT(RENDER_GROUP::BLEND)])
    {
        if (blend != nullptr)
            blend->Render(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::BLEND)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::BLEND)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::BLEND)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::BLEND)].size();

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_UI(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::UI)].empty())
        return;

    m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::NODEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::BLEND);

    sort(m_vecObjects[UINT(RENDER_GROUP::UI)].begin(), m_vecObjects[UINT(RENDER_GROUP::UI)].end(), [&](GameObject* a, GameObject* b)
        {
            if (a && b)
            {
                return a->Get_ZOrder() < b->Get_ZOrder();
            }

            return false;
        });

    for (auto& UI : m_vecObjects[UINT(RENDER_GROUP::UI)])
    {
        if (UI != nullptr)
        {
            if (UI->IsBlurUI())
            {
                if (UI->Is_Active())
                {
                    //캡쳐시작!!
                    m_bCaptured_Blur_At_OneFrame = true;
                    Captured_Blur_At_OneFrame();
                    //캡쳐이후 다시되돌려놓기..(이프레임은 이제 캡쳐안함)
                    m_bCaptured_Blur_At_OneFrame = false;
                }
            }
            UI->Render(fTimeDelta);
        }
            
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::UI)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::UI)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::UI)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::UI)].size();
}

void Engine::Renderer::Render_Camera(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::CAMERA)].empty())
        return;

    for (auto& Camera : m_vecObjects[UINT(RENDER_GROUP::CAMERA)])
    {
        if (Camera != nullptr)
            Camera->Render(fTimeDelta);
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::CAMERA)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::CAMERA)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::CAMERA)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::CAMERA)].size();
}

void Engine::Renderer::Render_HDR(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_CamMotionBlur, m_pShader, 11);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4);
    //얘도 재활용한거임
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Bright, m_pShader, 13);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Pick, m_pShader, 20);

    m_pShader->Begin(_UINT(DEFERRED::HDR_MAPPING));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_MAPPING));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

}

void Engine::Renderer::Render_CamVelocity(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::VELOCITYOUT, false, false), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4); //뎁스
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_CamVelocity, m_pShader, 23); //뎁스
    
    m_pShader->Begin(_UINT(DEFERRED::CAMVELOCITY));
    m_pShader->Bind_Resources(_UINT(DEFERRED::CAMVELOCITY));
        
    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_CamMotionBLur(const _float fTimeDelta) //캠 모션블러
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::NODEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::CAMMOTIONBLUR), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_HDR, m_pShader, 11); //화면
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_VelocityOut, m_pShader, 23); //캠벨로시티
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4); //뎁스

    m_pShader->Begin(_UINT(DEFERRED::CAMMOTIONBLUR));
    m_pShader->Bind_Resources(_UINT(DEFERRED::CAMMOTIONBLUR));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_HDR_Copy(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    //CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR), );

    //m_pShader->Bind_SRV("g_TextureHDR", m_pSceneSRV);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_CamMotionBlur, m_pShader, 11);

    m_pShader->Begin(_UINT(DEFERRED::HDR));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    //CHECK_FAILED(m_pGameInstance->End_MRT(), );

}

void Engine::Renderer::Render_HDR_BrightExtract(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);
    
    Setup_ViewportDesc(m_iViewportWidth * 0.5f, m_iViewportHeight * 0.5f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BRIGHT, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_CamMotionBlur, m_pShader, 11);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Emissive, m_pShader, 16);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_GodRay, m_pShader, 22);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_HDR_EFFECT_EMISSIVE, m_pShader, 26);

    m_pShader->Bind_RawValue_ByHandle(g_HDROption, &m_vHDROption, sizeof(_float4));

    m_pShader->Begin(_UINT(DEFERRED::HDR_BRIHGTEXTRACT));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BRIHGTEXTRACT));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);
}

void Engine::Renderer::Render_HDR_DownSampling(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

#pragma region 400*225 다운샘플링
    Setup_ViewportDesc(m_iViewportWidth * 0.25f, m_iViewportHeight * 0.25f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_DOWNSAMPLE, true), );

    _float2 WinSize = { 800.f, 450.f }; //다운샘플링전 원래 사이즈
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Bright, m_pShader, 12);

    m_pShader->Begin(_UINT(DEFERRED::HDR_SAMPLING));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_SAMPLING));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 200 * 112.5 다운샘플링
    Setup_ViewportDesc(m_iViewportWidth * 0.125f, m_iViewportHeight * 0.125f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_DOWNSAMPLE1, true), );

    WinSize = { 400.f, 225.f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_DownSample, m_pShader, 12);

    m_pShader->Begin(_UINT(DEFERRED::HDR_SAMPLING));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_SAMPLING));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 100 * 56.25 다운샘플링
    Setup_ViewportDesc(m_iViewportWidth * 0.0625f, m_iViewportHeight * 0.0625f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_DOWNSAMPLE2, true), );

    WinSize = { 200.f, 112.5f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_DownSample1, m_pShader, 12);

    m_pShader->Begin(_UINT(DEFERRED::HDR_SAMPLING));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_SAMPLING));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 50 * 28.125 다운샘플링
    Setup_ViewportDesc(m_iViewportWidth * 0.03125f, m_iViewportHeight * 0.03125f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_DOWNSAMPLE3, true), );

    WinSize = { 100.f, 56.25f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_DownSample2, m_pShader, 12);

    m_pShader->Begin(_UINT(DEFERRED::HDR_SAMPLING));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_SAMPLING));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);
}

void Engine::Renderer::Render_HDR_BlurHorizontal(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

#pragma region 400*225 가로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.25f, m_iViewportHeight * 0.25f);

    _float2 WinSize = { 400.f, 225.f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURHORIZONTAL, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_DownSample, m_pShader, 13);
 

    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURHORIZONTAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURHORIZONTAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 200 * 112.5 가로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.125f, m_iViewportHeight * 0.125f);

    WinSize = { 200.f, 112.5f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURHORIZONTAL1, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_DownSample1, m_pShader, 13);


    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURHORIZONTAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURHORIZONTAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 100 * 56.25 가로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.0625f, m_iViewportHeight * 0.0625f);

    WinSize = { 100.f, 56.25f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURHORIZONTAL2, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_DownSample2, m_pShader, 13);


    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURHORIZONTAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURHORIZONTAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 50 * 28.125 가로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.03125f, m_iViewportHeight * 0.03125f);
    WinSize = { 50.f, 28.125f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURHORIZONTAL3, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_DownSample3, m_pShader, 13);


    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURHORIZONTAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURHORIZONTAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);
}

void Engine::Renderer::Render_HDR_BlurVertical(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

#pragma region 400*225 세로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.25f, m_iViewportHeight * 0.25f);

    _float2 WinSize = { 400.f, 225.f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURVERTICAL, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurHorizontal, m_pShader, 13);


    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURVERTICAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURVERTICAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 200 * 112.5 세로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.125f, m_iViewportHeight * 0.125f);

    WinSize = { 200.f, 112.5f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURVERTICAL1, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurHorizontal1, m_pShader, 13);


    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURVERTICAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURVERTICAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 100 * 56.25 세로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.0625f, m_iViewportHeight * 0.0625f);

    WinSize = { 100.f, 56.25f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURVERTICAL2, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurHorizontal2, m_pShader, 13);


    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURVERTICAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURVERTICAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 50 * 28.125 세로블러
    Setup_ViewportDesc(m_iViewportWidth * 0.03125f, m_iViewportHeight * 0.03125f);
    WinSize = { 50.f, 28.125f };
    m_pShader->Bind_RawValue_ByHandle(g_TextureSize, &WinSize, sizeof(_float2));

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURVERTICAL3, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurHorizontal3, m_pShader, 13);


    m_pShader->Begin(_UINT(DEFERRED::HDR_BLURVERTICAL));
    m_pShader->Bind_Resources(_UINT(DEFERRED::HDR_BLURVERTICAL));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);
}

void Engine::Renderer::Render_HDR_UPSamplingPlus(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

#pragma region 50 * 28.125 -> 100 * 56.25 업샘플링
    Setup_ViewportDesc(m_iViewportWidth * 0.0625f, m_iViewportHeight * 0.0625f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURHORIZONTAL2, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurVertical2, m_pShader, 25);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurVertical3, m_pShader, 13);

    m_pShader->Begin(_UINT(DEFERRED::UPSAMPLINGPLUS));
    m_pShader->Bind_Resources(_UINT(DEFERRED::UPSAMPLINGPLUS));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 100 * 56.25 -> 200 * 112.5 업샘플링
    Setup_ViewportDesc(m_iViewportWidth * 0.125f, m_iViewportHeight * 0.125f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURHORIZONTAL1, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurVertical1, m_pShader, 25);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurHorizontal2, m_pShader, 13);

    m_pShader->Begin(_UINT(DEFERRED::UPSAMPLINGPLUS));
    m_pShader->Bind_Resources(_UINT(DEFERRED::UPSAMPLINGPLUS));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

#pragma region 200 * 112.5 -> 400 * 225 업샘플링
    Setup_ViewportDesc(m_iViewportWidth * 0.25f, m_iViewportHeight * 0.25f);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BLURHORIZONTAL, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurVertical, m_pShader, 25);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurHorizontal1, m_pShader, 13);

    m_pShader->Begin(_UINT(DEFERRED::UPSAMPLINGPLUS));
    m_pShader->Bind_Resources(_UINT(DEFERRED::UPSAMPLINGPLUS));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
#pragma endregion

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);
}

void Engine::Renderer::Render_HDR_UpSampling(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    Setup_ViewportDesc(m_iViewportWidth * 0.5f, m_iViewportHeight * 0.5f);

    //Bright 재사용
    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::HDR_BRIGHT, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_BlurHorizontal, m_pShader, 13);

    m_pShader->Begin(_UINT(DEFERRED::UP_SAMPLING1));
    m_pShader->Bind_Resources(_UINT(DEFERRED::UP_SAMPLING1));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);
}

void Engine::Renderer::Render_SSAO(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::SSAO, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Normal, m_pShader, 2);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Roughness, m_pShader, 15);

    m_pShader->Begin(_UINT(DEFERRED::SSAO));
    m_pShader->Bind_Resources(_UINT(DEFERRED::SSAO));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_SSAO_Blur(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT::SSAO_Blur, true), );

    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_SSAO, m_pShader, 17);
    m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Depth, m_pShader, 4);

    m_pShader->Begin(_UINT(DEFERRED::SSAO_BLUR));
    m_pShader->Bind_Resources(_UINT(DEFERRED::SSAO_BLUR));

    m_pVIBuffer->Bind_Resource();
    m_pVIBuffer->Render(fTimeDelta);

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 뷰포트 세팅 함수 ////////////////////////////////////////////////////////
void Engine::Renderer::Setup_ViewportDesc(_float _width, _float _height)
{
    D3D11_VIEWPORT desc{};
    desc.TopLeftX = 0.f;
    desc.TopLeftY = 0.f;
    desc.Width = _width;
    desc.Height = _height;
    desc.MinDepth = 0.f;
    desc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &desc);
}

/******************************************************* 뷰포트 세팅 함수 *******************************************************/



//////////////////////////////////////////////////////// SSAO 세팅 함수 ////////////////////////////////////////////////////////
void Engine::Renderer::Ready_SSAOSetUp()
{
    ////커널생성
    for (int i = 0; i < m_iSSAO_Size; ++i)
    {
        XMFLOAT4 sample;
        sample.x = dist(generator) * 2.0f - 1.0f; // -1~1
        sample.y = dist(generator) * 2.0f - 1.0f; // -1~1
        sample.z = dist(generator);                 // 0~1 (반구니까 양수만)
        sample.w = 0.0f;

        XMVECTOR v = XMLoadFloat4(&sample);
        v = XMVector3Normalize(v);
        float scale = (float)i / (float)m_iSSAO_Size;
        scale = 0.1f + scale * scale * 0.9f; // lerp(0.1, 1.0, scale^2)
        v = XMVectorScale(v, scale);
        XMStoreFloat4(&ssaoKernel[i], v);
    }
}
/******************************************************* SSAO 세팅 함수 *******************************************************/



//////////////////////////////////////////////////////// 디버그 전용 함수 ////////////////////////////////////////////////////////
#ifdef _DEBUG
HRESULT Engine::Renderer::Initialize_DebugDraw()
{
    // DebugDraw용 변수들 생성
    m_pDebugBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pDebugEffect = new BasicEffect(m_pDevice);
    m_pDebugEffect->SetVertexColorEnabled(true);
    m_pDebugEffect->SetLightingEnabled(false);

    // InputLayout 만들때 필요한 값들을 BasicEffect에 내장된 쉐이더에서 꺼내옴
    const void* shaderByteCode = {};
    size_t byteCodeLength = {};
    m_pDebugEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    // 인풋 레이아웃 생성
    MSG_FAIL(m_pDevice->CreateInputLayout(
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount,
        shaderByteCode, byteCodeLength,
        &m_pDebugInputLayout)
        , L"Debug Render의 InputLayout 생성에 실패했습니다!", L"디버그 생성 실패"
        , E_FAIL);

    return S_OK;
}

HRESULT Engine::Renderer::Add_DebugComponent(Component* pComponent)
{
    m_DebugComponents.push_back(pComponent);

    Safe_AddRef(pComponent);

    return S_OK;
}

void Engine::Renderer::Debug_Render_Begin()
{
    _float4x4 view = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_VIEW);
    _float4x4 proj = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_PROJ);

    m_pDebugEffect->SetWorld(XMMatrixIdentity());
    m_pDebugEffect->SetView(XMLoadFloat4x4(&view));
    m_pDebugEffect->SetProjection(XMLoadFloat4x4(&proj));
    m_pDebugEffect->Apply(m_pContext);

    m_pContext->IASetInputLayout(m_pDebugInputLayout);

    m_pDebugBatch->Begin();
}

void Engine::Renderer::Debug_Render_End()
{
    m_pDebugBatch->End();
}

void Engine::Renderer::Render_Debug(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::NODEPTH);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    Debug_Render_Begin();

    for (auto& debugCom : m_DebugComponents)
    {
        if (debugCom != nullptr)
        {
            debugCom->Render_Debug(m_pDebugBatch);
            Safe_Release(debugCom);
        }
    }
    m_DebugComponents.clear();
    Render_DebugSphere();
    Render_DebugCapsule();
    Render_DebugLookLine();
    Render_DebugFan();

    Render_Debug_PhysX();

    Debug_Render_End();
}

void Engine::Renderer::Render_DebugRTV(const _float fTimeDelta)
{
    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    CHECK_FAILED(m_pVIBuffer->Bind_Resource(), );

    CHECK_FAILED(m_pShader->Bind_Matrix_FullSlot(0, "g_ViewMatrix", m_ViewMatrix), );
    CHECK_FAILED(m_pShader->Bind_Matrix_FullSlot(0, "g_ProjMatrix", m_ProjMatrix), );

    m_pGameInstance->Render_RT(MRT::GAMEOBJECT, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::LIGHTACC, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::SHADOW, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::HDR, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::HDR_BRIGHT, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::HDR_DOWNSAMPLE, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::HDR_BLURHORIZONTAL, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::HDR_BLURVERTICAL, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::SSAO, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::SSAO_Blur, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::SHADOW_BAKE, m_pShader, m_pVIBuffer, fTimeDelta);
    m_pGameInstance->Render_RT(MRT::GODRAY, m_pShader, m_pVIBuffer, fTimeDelta);
}

void Engine::Renderer::Render_DebugSphere()
{
    if (m_vecDebugSphere.empty())
        return;

    for (auto& bound : m_vecDebugSphere)
    {
        DX::Draw(m_pDebugBatch, bound.Sphere, XMVectorSetW(XMLoadFloat3(&bound.vColor), 1.f));
    }
}

void Engine::Renderer::Add_Debug_Sphere(BoundingSphere _sphere, _float3 _color)
{
    DebugDraw debug;
    debug.Sphere = _sphere;
    debug.vColor = _color;

    m_vecDebugSphere.push_back(debug);
}

void Engine::Renderer::Render_DebugCapsule()
{
    if (m_vecDebugCapsule.empty())
        return;

    for (auto& capsule : m_vecDebugCapsule)
    {
        XMVECTOR color = XMVectorSetW(XMLoadFloat3(&capsule.vColor), 1.f);
        XMVECTOR center = XMLoadFloat3(&capsule.vCenter);
        XMVECTOR orientation = XMLoadFloat4(&capsule.qOrientation);

        XMVECTOR up = XMVector3Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), orientation);
        XMVECTOR right = XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), orientation);
        XMVECTOR forward = XMVector3Rotate(XMVectorSet(0.f, 0.f, 1.f, 0.f), orientation);

        XMVECTOR topCenter = XMVectorAdd(center, XMVectorScale(up, capsule.fHalfHeight));
        XMVECTOR bottomCenter = XMVectorSubtract(center, XMVectorScale(up, capsule.fHalfHeight));

        // majorAxis/minorAxis에 radius를 곱해서 전달
        XMVECTOR majorRight = XMVectorScale(right, capsule.fRadius);
        XMVECTOR majorForward = XMVectorScale(forward, capsule.fRadius);
        XMVECTOR majorUp = XMVectorScale(up, capsule.fRadius);

        // 상단 반구 링 3개
        DX::DrawRing(m_pDebugBatch, topCenter, majorRight, majorForward, color);
        DX::DrawRing(m_pDebugBatch, topCenter, majorRight, majorUp, color);
        DX::DrawRing(m_pDebugBatch, topCenter, majorForward, majorUp, color);

        // 하단 반구 링 3개
        DX::DrawRing(m_pDebugBatch, bottomCenter, majorRight, majorForward, color);
        DX::DrawRing(m_pDebugBatch, bottomCenter, majorRight, majorUp, color);
        DX::DrawRing(m_pDebugBatch, bottomCenter, majorForward, majorUp, color);

        // 중간 연결 직선 4개
        XMVECTOR offsets[4] =
        {
            majorRight,
            XMVectorNegate(majorRight),
            majorForward,
            XMVectorNegate(majorForward)
        };

        for (int i = 0; i < 4; ++i)
        {
            VertexPositionColor topVert, bottomVert;

            _float3 topPos, bottomPos;
            XMStoreFloat3(&topPos, XMVectorAdd(topCenter, offsets[i]));
            XMStoreFloat3(&bottomPos, XMVectorAdd(bottomCenter, offsets[i]));

            _float4 vColor;
            XMStoreFloat4(&vColor, color);

            topVert.position = topPos;
            topVert.color = vColor;
            bottomVert.position = bottomPos;
            bottomVert.color = vColor;

            m_pDebugBatch->DrawLine(topVert, bottomVert);
        }
    }
}

void Engine::Renderer::Add_Debug_Capsule(CAPSULE_DESC _capsule)
{
    m_vecDebugCapsule.push_back(_capsule);
}

void Engine::Renderer::Add_Debug_Capsule(_float3& _center, _float _radius, _float _halfHeight, _float4& _quaternion, _float3& _color)
{
    DEBUG_CAPSULE_DESC desc;
    desc.vCenter = _center;
    desc.fRadius = _radius;
    desc.fHalfHeight = _halfHeight;
    desc.qOrientation = _quaternion;
    desc.vColor = _color;

    m_vecDebugCapsule.push_back(desc);
}

void Engine::Renderer::Render_DebugLookLine()
{
    for(auto& line : m_vecDebugLine)
    {
        VertexPositionColor start{}, end{};
        start.position = line.vStartPos;
        start.color = line.vColor;
        end.position = line.vEndPos;
        end.color = _float4(1.f, 0.f, 0.f, 0.f);
        m_pDebugBatch->DrawLine(start, end);
    }
}

void Engine::Renderer::Add_Debug_Fan(const DebugFan& _fan)
{
    m_vecDebugFan.push_back(_fan);
}

void Engine::Renderer::Render_DebugFan()
{
    if (m_vecDebugFan.empty())
        return;

    constexpr _int SEGMENT_COUNT = 20;

    for (auto& fan : m_vecDebugFan)
    {
        _float4 color = { fan.vColor.x, fan.vColor.y, fan.vColor.z, 1.f };

        // Look 방향 각도
        _float baseAngle = atan2f(fan.vLook.x, fan.vLook.z);
        _float halfRad = fan.fHalfAngle * (XM_PI / 180.f);
        _float startAngle = baseAngle - halfRad;
        _float angleStep = (halfRad * 2.f) / (_float)SEGMENT_COUNT;

        // 살짝 위로 띄워서 바닥에 묻히지 않게
        _float3 center = { fan.vCenter.x, fan.vCenter.y + 0.05f, fan.vCenter.z };

        VertexPositionColor origin;
        origin.position = center;
        origin.color = color;

        VertexPositionColor prevVert;
        prevVert.position = {
            center.x + sinf(startAngle) * fan.fRadius,
            center.y,
            center.z + cosf(startAngle) * fan.fRadius
        };
        prevVert.color = color;

        // 원점 -> 첫 번째 호 점 (시작 변)
        m_pDebugBatch->DrawLine(origin, prevVert);

        // 호를 따라 라인 연결
        for (_int i = 1; i <= SEGMENT_COUNT; ++i)
        {
            _float angle = startAngle + angleStep * i;

            VertexPositionColor currVert;
            currVert.position = {
                center.x + sinf(angle) * fan.fRadius,
                center.y,
                center.z + cosf(angle) * fan.fRadius
            };
            currVert.color = color;

            // 호 라인
            m_pDebugBatch->DrawLine(prevVert, currVert);

            prevVert = currVert;
        }

        // 마지막 호 점 -> 원점 (끝 변)
        m_pDebugBatch->DrawLine(prevVert, origin);
    }
}


void Engine::Renderer::Render_Debug_PhysX()
{
    // PhysX RenderBuffer에서 디버그 프리미티브 추출 후 렌더링
    const PxRenderBuffer* pRenderBuffer = m_pGameInstance->Get_PhysXRenderBuffer();
    if (pRenderBuffer == nullptr)
        return;

    // PhysX 라인 렌더링 (콜리전 셰이프, 축, 법선 등)
    PxU32 lineCount = pRenderBuffer->getNbLines();
    const PxDebugLine* pLines = pRenderBuffer->getLines();

    for (PxU32 i = 0; i < lineCount; ++i)
    {
        const PxDebugLine& line = pLines[i];

        VertexPositionColor v0, v1;
        v0.position = _float3(line.pos0.x, line.pos0.y, line.pos0.z);
        v1.position = _float3(line.pos1.x, line.pos1.y, line.pos1.z);

        // PxU32 ARGB -> _float4 RGBA 변환
        v0.color = PhysX_Color_To_Float4(line.color0);
        v1.color = PhysX_Color_To_Float4(line.color1);

        m_pDebugBatch->DrawLine(v0, v1);
    }

    // PhysX 트라이앵글 렌더링 (와이어프레임으로 그리기)
    PxU32 triCount = pRenderBuffer->getNbTriangles();
    const PxDebugTriangle* pTriangles = pRenderBuffer->getTriangles();

    for (PxU32 i = 0; i < triCount; ++i)
    {
        const PxDebugTriangle& tri = pTriangles[i];

        VertexPositionColor v0, v1, v2;
        v0.position = _float3(tri.pos0.x, tri.pos0.y, tri.pos0.z);
        v1.position = _float3(tri.pos1.x, tri.pos1.y, tri.pos1.z);
        v2.position = _float3(tri.pos2.x, tri.pos2.y, tri.pos2.z);

        _float4 color = PhysX_Color_To_Float4(tri.color0);
        v0.color = color;
        v1.color = color;
        v2.color = color;

        // 트라이앵글을 와이어프레임 라인 3개로 분해
        m_pDebugBatch->DrawLine(v0, v1);
        m_pDebugBatch->DrawLine(v1, v2);
        m_pDebugBatch->DrawLine(v2, v0);
    }

    // PhysX 포인트 렌더링 (접점 등) - 작은 십자가로 표현
    PxU32 pointCount = pRenderBuffer->getNbPoints();
    const PxDebugPoint* pPoints = pRenderBuffer->getPoints();

    const _float fPointSize = 0.05f;	// 접점 표시 크기

    for (PxU32 i = 0; i < pointCount; ++i)
    {
        const PxDebugPoint& point = pPoints[i];
        _float4 color = PhysX_Color_To_Float4(point.color);
        _float3 pos = _float3(point.pos.x, point.pos.y, point.pos.z);

        // X축 방향 작은 라인
        VertexPositionColor left, right;
        left.position = _float3(pos.x - fPointSize, pos.y, pos.z);
        left.color = color;
        right.position = _float3(pos.x + fPointSize, pos.y, pos.z);
        right.color = color;
        m_pDebugBatch->DrawLine(left, right);

        // Y축 방향 작은 라인
        VertexPositionColor bottom, top;
        bottom.position = _float3(pos.x, pos.y - fPointSize, pos.z);
        bottom.color = color;
        top.position = _float3(pos.x, pos.y + fPointSize, pos.z);
        top.color = color;
        m_pDebugBatch->DrawLine(bottom, top);

        // Z축 방향 작은 라인
        VertexPositionColor back, front;
        back.position = _float3(pos.x, pos.y, pos.z - fPointSize);
        back.color = color;
        front.position = _float3(pos.x, pos.y, pos.z + fPointSize);
        front.color = color;
        m_pDebugBatch->DrawLine(back, front);
    }
}

// PhysX ARGB(PxU32) -> _float4 RGBA 변환 유틸
_float4 Engine::Renderer::PhysX_Color_To_Float4(PxU32 _color)
{
    // PxDebugColor는 ARGB 포맷
    _float a = ((_color >> 24) & 0xFF) / 255.f;
    _float r = ((_color >> 16) & 0xFF) / 255.f;
    _float g = ((_color >> 8) & 0xFF) / 255.f;
    _float b = ((_color >> 0) & 0xFF) / 255.f;

    return _float4(r, g, b, a);
}

void Engine::Renderer::Add_Debug_LookLine(_float3& _pos, _float3& _target, _float4& _color)
{
    LINE_DESC desc;
    desc.vStartPos = _pos;
    desc.vEndPos = _target;
    desc.vColor = _color;
    m_vecDebugLine.push_back(desc);
}

void Engine::Renderer::Render_Grid()
{
    ID3D11DepthStencilView* pMainDSV = m_pGameInstance->Get_DepthStencilView();
    m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV, false);

    m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::SKY);
    m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    _float4x4 viewmat = m_pGameInstance->Get_PipeLineMatrix(D3DTS_VIEW);
    _float4x4 projmat = m_pGameInstance->Get_PipeLineMatrix(D3DTS_PROJ);
    m_pDebugEffect->SetView(XMLoadFloat4x4(&viewmat));
    m_pDebugEffect->SetProjection(XMLoadFloat4x4(&projmat));
    m_pDebugEffect->SetWorld(XMMatrixIdentity());
    m_pDebugEffect->Apply(m_pContext);

    m_pContext->IASetInputLayout(m_pDebugInputLayout);

    m_pDebugBatch->Begin();

    DX::DrawGrid(
        m_pDebugBatch,
        XMVectorSet(10.0f, 0.0f, 0.0f, 0.0f),
        XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f),
        g_XMZero,
        100, 100,
        Colors::GhostWhite
    );

    m_pDebugBatch->End();

    m_pGameInstance->End_MRT();
}
#endif
/******************************************************* 디버그 전용 함수 *******************************************************/



/*
//////////////////////////////////////////////////////// 오브젝트 피킹 함수 ////////////////////////////////////////////////////////
_uint Engine::Renderer::Picking_Object(_int _mousex, _int _mousey) {
    // RS 바인드 (Scissor ON)
    m_pContext->RSSetState(m_pGameInstance->Get_PickingRS());

    // 피킹 RT/DS 바인드
    ID3D11RenderTargetView* pRTV = m_pGameInstance->Get_PickingRTV();
    ID3D11DepthStencilView* pDSV = m_pGameInstance->Get_PickingDSV();
    m_pContext->OMSetRenderTargets(1, &pRTV, pDSV);

    // 클리어 (ID=0, Z=1.0)
    _float color[4] = { 0.f, 0.f, 0.f, 0.f };
    m_pContext->ClearRenderTargetView(pRTV, color);
    m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

    // Viewport 전체 (Scissor로 제한)
    D3D11_VIEWPORT vp = {};
    m_pGameInstance->Get_Winsize(&vp.Width, &vp.Height);
    vp.TopLeftX = 0.f; vp.TopLeftY = 0.f;
    vp.MinDepth = 0.f; vp.MaxDepth = 1.f;
    m_pContext->RSSetViewports(1, &vp);

    // Y Flip (필요 시, 테스트 후 결정 - 이전 로그 359 = 위쪽)
    // _mousey = vp.Height - 1 - _mousey;  // 주석 해제 if inverted

    // Scissor 1x1 (마우스 위치만 렌더)
    D3D11_RECT scissorRect = { (LONG)_mousex, (LONG)_mousey, (LONG)_mousex + 1, (LONG)_mousey + 1 };
    m_pContext->RSSetScissorRects(1, &scissorRect);

    // DSS 바인드 (Z-테스트 ON)
    ID3D11DepthStencilState* pPickingDSState = m_pGameInstance->Get_PickingDSS();
    m_pContext->OMSetDepthStencilState(pPickingDSState, 0);

    // 모든 오브젝트 버퍼에 그리기
    for (_uint i = 0; i < _UINT(RENDER_GROUP::UI); i++)
    {
        for (auto& object : m_vecObjects[i])
        {
            if (object->Get_PickingShader() == nullptr) continue;

            if (FAILED(object->Bind_ShaderPicking()))
                continue;
        }
    }

    // 언바인드 (피킹 끝)
    ID3D11RenderTargetView* nullRTV = nullptr;
    m_pContext->OMSetRenderTargets(1, &nullRTV, nullptr);

    // 복사 (1x1 box)
    ID3D11Texture2D* staging = m_pGameInstance->Get_PickingStaging();
    D3D11_BOX box = { (UINT)_mousex, (UINT)_mousey, 0, (UINT)_mousex + 1, (UINT)_mousey + 1, 1 };
    m_pContext->CopySubresourceRegion(staging, 0, 0, 0, 0, m_pGameInstance->Get_PickingTexture(), 0, &box);

    // 읽기
    _uint pickedID = 0;
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (SUCCEEDED(m_pContext->Map(staging, 0, D3D11_MAP_READ, 0, &mapped))) {
        _float4* pixel = (_float4*)mapped.pData;
        pickedID = (_uint)pixel->w;
        XMFLOAT3 worldPos = { pixel->x, pixel->y, pixel->z };
        m_pContext->Unmap(staging, 0);

        // 로그
        wstring log = L"Picked ID: " + to_wstring(pickedID) + L", WorldPos: (" + to_wstring(worldPos.x) + L", " + to_wstring(worldPos.y) + L", " + to_wstring(worldPos.z) + L")\n";
        OutputDebugStringW(log.c_str());
    }

    //  원래 상태 복구 (피킹 후 렌더 복귀)
    ID3D11RenderTargetView* backRTV = m_pGameInstance->Get_BackBufferRTV();
    ID3D11DepthStencilView* backDSV = m_pGameInstance->Get_DepthStencilView();
    m_pContext->OMSetRenderTargets(1, &backRTV, backDSV);
    ID3D11DepthStencilState* pDefaultDSState = m_pGameInstance->Get_DefaultDSS();
    m_pContext->OMSetDepthStencilState(pDefaultDSState, 0);
    D3D11_VIEWPORT backVP = {};
    m_pGameInstance->Get_Viewport(&backVP);
    m_pContext->RSSetViewports(1, &backVP);
    m_pContext->RSSetState(nullptr);  // 기본 RS (Scissor OFF)

    return pickedID;
}
/******************************************************* 오브젝트 피킹 함수 *******************************************************/



void Engine::Renderer::Captured_Blur_At_OneFrame()
{
    /*나중에 캐싱하는 쪽으로 수정해야할수도있음!!!!!*/
    m_pOnlyBindingShader->Bind_Matrix_ByHandle(g_WorldMatrix, m_WorldMatrix);
    m_pOnlyBindingShader->Bind_EntireBuffer_BySlot(BUFFER_CAMERA, &m_tCameraBuffer, sizeof(Camera_Buffer));
    m_pOnlyBindingShader->Bind_Resources(0);


    m_pBackRTV = m_pGameInstance->Get_BackBufferRTV();
    m_pBackRTV->GetResource(&pBackRes);


    m_pContext->CopyResource(m_pSceneTexture_ForUI, pBackRes);
    Safe_Release(pBackRes);

    _float2 vRes = _float2(m_iViewportWidth * 0.25f, m_iViewportHeight * 0.25f);
    // 가로 블러
    Setup_ViewportDesc(vRes.x, vRes.y);
    if (SUCCEEDED(m_pGameInstance->Begin_MRT(MRT::UI_BLURHORIZONTAL, true)))
    {
        // 소스를 g_TextureSampling (t13) 슬롯
        m_pShader->Bind_SRV_FullSlot(13, m_pSceneSRV);
        m_pShader->Begin(11); // 가로 블러 패스
        m_pVIBuffer->Bind_Resource();
        m_pVIBuffer->Render(0.f);
        m_pGameInstance->End_MRT();
    }

    // 세로 블러
    if (SUCCEEDED(m_pGameInstance->Begin_MRT(MRT::UI_BLURVERTICAL, true)))
    {

        ID3D11ShaderResourceView* pHorizontalSRV = m_pGameInstance->Get_ShaderResourceView(RenderTargets::Target_UI_BlurHorizontal);
        m_pShader->Bind_SRV_FullSlot(13, pHorizontalSRV);

        m_pShader->Begin(12);
        m_pVIBuffer->Bind_Resource();
        m_pVIBuffer->Render(0.f);
        m_pGameInstance->End_MRT();
    }

    Setup_ViewportDesc((_float)m_iViewportWidth, (_float)m_iViewportHeight);



}


//////////////////////////////////////////////////////// 맵변환 함수 ////////////////////////////////////////////////////////
void Engine::Renderer::ChangeMap(_float _t)
{
    m_vHDROption.y = m_ZonDesc[0].fExposure * (1.f - _t) + m_ZonDesc[2].fExposure * _t;
    m_vShaderDesc.fLightMultiplier = m_ZonDesc[0].fLightMultiplier * (1.f - _t) + m_ZonDesc[2].fLightMultiplier * _t;
    m_vShaderDesc.fAmbientStrength = m_ZonDesc[0].fAmbientStrength * (1.f - _t) + m_ZonDesc[2].fAmbientStrength * _t;
    m_vShaderDesc.vFogColor = LerpFloat3(m_ZonDesc[0].vFogColor, m_ZonDesc[2].vFogColor, _t);
    m_vShaderDesc.fFogStartDist = m_ZonDesc[0].fFogStartDist * (1.f - _t) + m_ZonDesc[2].fFogStartDist * _t;
    m_vShaderDesc.vShadowTintColor = LerpFloat3(m_ZonDesc[0].vShadowTint, m_ZonDesc[2].vShadowTint, _t);
    m_vShaderDesc.vMidtoneTintColor = LerpFloat3(m_ZonDesc[0].vMidtoneTint, m_ZonDesc[2].vMidtoneTint, _t);
    m_vShaderDesc.vHighlightTintColor = LerpFloat3(m_ZonDesc[0].vHighlightTint, m_ZonDesc[2].vHighlightTint, _t);
    m_vShaderDesc.vSpecBoostRange.x = m_ZonDesc[0].vSpecBoostRange.x * (1.f - _t) + m_ZonDesc[2].vSpecBoostRange.x * _t;
    m_vShaderDesc.vSpecBoostRange.y = m_ZonDesc[0].vSpecBoostRange.y * (1.f - _t) + m_ZonDesc[2].vSpecBoostRange.y * _t;
    m_vShaderDesc.fogType = m_ZonDesc[0].vFogType * (1.f - _t) + m_ZonDesc[2].vFogType * _t;
    m_vShaderDesc.fBambooEmission = m_ZonDesc[0].fBambooEmissionIntensity * (1.f - _t) + m_ZonDesc[2].fBambooEmissionIntensity * _t;
    m_vShaderDesc.fBackFillStrength = m_ZonDesc[0].fBackFillStr * (1.f - _t) + m_ZonDesc[2].fBackFillStr * _t;
    
    m_vShaderDesc.vSpecMonsterBoostRange.x = m_ZonDesc[1].vSpecMapBoostRange.x * (1.f - _t) + m_ZonDesc[2].vSpecMonsterBoostRange.x * _t;
    m_vShaderDesc.vSpecMonsterBoostRange.y = m_ZonDesc[1].vSpecMapBoostRange.y * (1.f - _t) + m_ZonDesc[2].vSpecMonsterBoostRange.y * _t;

    m_vShaderDesc.fUnderMapZeroDirSpecular = m_ZonDesc[0].vfUnderMapZeroDirSpecular * (1.f - _t) + m_ZonDesc[2].vfUnderMapZeroDirSpecular * _t;
    return;
}

void Engine::Renderer::ChangeBaseMap()
{
    m_vShaderDesc.fBambooEmission = 20.f;
    m_vShaderDesc.vSpecBosstMapRange = _float2(2.f, 2.f); //5, 1 -> 2 2
    m_vShaderDesc.fUnderMapZeroDirSpecular = 0.5f;
    m_vShaderDesc.vGodRayPosition = _float4{ -185.89, 95.86, -43.45, 1.f };
}

void Engine::Renderer::ChangeCaveMap(_float _t)
{
    m_vShaderDesc.fCaveMapWet = m_ZonDesc[0].fCaveWet * (1.f - _t) + m_ZonDesc[1].fCaveWet * _t;
    m_vShaderDesc.fMtrlRoughnessMin = m_ZonDesc[0].fMinMtrlRough * (1.f - _t) + m_ZonDesc[1].fMinMtrlRough * _t;
    m_vShaderDesc.vSpecBosstMapRange.x = m_ZonDesc[0].vSpecMapBoostRange.x * (1.f - _t) + m_ZonDesc[1].vSpecMapBoostRange.x * _t;
    m_vShaderDesc.vSpecBosstMapRange.y = m_ZonDesc[0].vSpecMapBoostRange.y * (1.f - _t) + m_ZonDesc[1].vSpecMapBoostRange.y * _t;
    m_vShaderDesc.vSpecMonsterBoostRange.x = m_ZonDesc[0].vSpecMapBoostRange.x * (1.f - _t) + m_ZonDesc[1].vSpecMonsterBoostRange.x * _t;
    m_vShaderDesc.vSpecMonsterBoostRange.y = m_ZonDesc[0].vSpecMapBoostRange.y * (1.f - _t) + m_ZonDesc[1].vSpecMonsterBoostRange.y * _t;
}

void Engine::Renderer::ChangeCustomize()
{
    m_vShaderDesc.vBackFaceShaowRange.y = 1.f;
    m_vShaderDesc.fShadowBackBright = 0.9f;
    m_vShaderDesc.fShadowBright = 0.9f;

    //지상세팅을 사용한다
    m_vShaderDesc.fLightMultiplier = 2.f; //전체조명세기
    m_vShaderDesc.fAmbientStrength = 0.3f; //앰비언트세기

    m_vShaderDesc.vFogColor = _float3(0.55f, 0.48f, 0.38f);
    m_vShaderDesc.fFogStartDist = 50.f; //포그 시작거리
    m_vShaderDesc.fogType = 0.5f; //포그타입 0.5로 나뉨
    m_vShaderDesc.vShadowTintColor = XMFLOAT3(1.10f, 0.95f, 0.80f);
    m_vShaderDesc.vMidtoneTintColor = XMFLOAT3(1.15f, 1.05f, 0.85f);
    m_vShaderDesc.vHighlightTintColor = XMFLOAT3(1.15f, 1.07f, 0.78f);

    m_vShaderDesc.vSpecBoostRange = XMFLOAT2(4.f, 1.f); //x축 매끈한면 부스트 y축 거친면 부스트 //1.2 0.3
    m_vShaderDesc.fSpecularIntensity = 1.f; //전체 스펙 세기 //0.7
    m_vShaderDesc.vSpecBosstMapRange = XMFLOAT2(2.f, 2.f); //x축 매끈한면 부스트 y축 거친면 부스트 //1.2 0.3
    m_vShaderDesc.fSpecularMapIntensity = 1.f; //전체 스펙 세기 //0.7
    m_vShaderDesc.fUnderMapZeroDirSpecular = 1.0f;

    m_vShaderDesc.fCaveMapWet = 0.1f;
}

void Engine::Renderer::ChangeMainMap()
{
    //지하세팅
    m_vShaderDesc.vBackFaceShaowRange.y = 0.3f;
    m_vShaderDesc.fShadowBright = 0.9f; //피부그림자 밝기
    m_vShaderDesc.fShadowBackBright = 0.3f; //뒷면 그림자 밝기

    m_vShaderDesc.fLightMultiplier = 1.f; //전체조명세기  
    m_vShaderDesc.fAmbientStrength = 0.15f; //앰비언트세기

    m_vShaderDesc.vFogColor = _float3(0.05f, 0.1f, 0.25f);
    m_vShaderDesc.fFogStartDist = 20.f; //포그 시작거리
    m_vShaderDesc.fogType = 0.f; //포그타입 0.5로 나뉨
    m_vShaderDesc.vShadowTintColor = _float3(0.85f, 0.85f, 1.15f);
    m_vShaderDesc.vMidtoneTintColor = _float3(0.92f, 0.90f, 1.05f);
    m_vShaderDesc.vHighlightTintColor = _float3(0.95f, 0.93f, 1.08f);

    m_vShaderDesc.vSpecBoostRange = XMFLOAT2(4.f, 1.f); //x축 매끈한면 부스트 y축 거친면 부스트 //1.2 0.3
    m_vShaderDesc.fSpecularIntensity = 1.f; //전체 스펙 세기 //0.7
    m_vShaderDesc.vSpecBosstMapRange = XMFLOAT2(2.f, 2.f); //x축 매끈한면 부스트 y축 거친면 부스트 //1.2 0.3
    m_vShaderDesc.fSpecularMapIntensity = 1.f; //전체 스펙 세기 //0.7
    m_vShaderDesc.fUnderMapZeroDirSpecular = 0.005f;

    m_vShaderDesc.fBambooEmission = 30.f;
    m_vShaderDesc.fCaveMapWet = 0.8f;
}

void Engine::Renderer::ChangeChurch()
{
    //테스트용으로 이동하고 바로값
    // ---- HDR ----
    m_vShaderDesc.fBambooEmission = 20.f;
    m_vShaderDesc.fUnderBambooEmission = 20.f;
    m_vHDROption.x = 0.1f;   // BloomIntensity
    m_vHDROption.y = 0.7f;   // HDR_Exposure
    m_vHDROption.z = 2.2f;   // Gamma
    m_vHDROption.w = 0.8f;   // BrightExtract

    // ---- SSAO ----
    m_vSSAOOption.x = 1.f;    // ON/OFF
    m_vSSAOOption.y = 0.5f;   // SSAORadius
    m_vSSAOOption.z = 0.01f;  // SSAOBias

    // ---- Color Grading ----
    m_vShaderDesc.g_fEnableColorGrading = 1.f;
    m_vShaderDesc.vShadowTintColor = _float3(1.1f, 0.95f, 0.8f);
    m_vShaderDesc.fShadowTintWeight = 0.6f;
    m_vShaderDesc.vMidtoneTintColor = _float3(1.15f, 1.05f, 0.85f);
    m_vShaderDesc.fMidtoneTintWeight = 0.5f;
    m_vShaderDesc.vHighlightTintColor = _float3(1.15f, 1.07f, 0.78f);
    m_vShaderDesc.fHighlightTintWeight = 0.7f;
    m_vShaderDesc.fShadowRange = 0.3f;
    m_vShaderDesc.fHighLightRange = 0.5f;

    m_vShaderDesc.vFogColor = _float3(0.6f, 0.6f, 0.6f);
    m_vShaderDesc.fFogEnable = 1.f;

    // Distance Fog
    m_vShaderDesc.fFogStartDist = 50.f;
    m_vShaderDesc.fFogEndDist = 300.f;
    m_vShaderDesc.fFogDensity = 0.00015f;
    m_vShaderDesc.fogType = 1.f;  // Exponential

    // Height Fog
    m_vShaderDesc.fFogBaseHeight = -64.f;
    m_vShaderDesc.fFogFadeHeight = 50.2f;
    m_vShaderDesc.fHeightFogIntensity = 7.f;
    m_vShaderDesc.fFogIntensity = 1.f;

    // ---- Lighting ----
    m_vShaderDesc.fLightMultiplier = 2.f;
    m_vShaderDesc.fAmbientStrength = 0.3f;
    m_vShaderDesc.vToonShadowRange = _float2(0.05f, 0.25f);
    m_vShaderDesc.vToonBrightnessRange = _float2(0.05f, 1.f);
    m_vShaderDesc.vAmbientFloorMin = _float2(0.08f, 0.02f);
    m_vShaderDesc.fBackFillStrength = 0.7f;

    // ---- SSS / Skin ----
    m_vShaderDesc.vSSSColor = _float3(0.9f, 0.82f, 0.72f);
    m_vShaderDesc.fSSSPower = 4.f;
    m_vShaderDesc.fSSSIntensity = 0.1f;
    m_vShaderDesc.vSkinTint = XMFLOAT3(0.873f, 0.79f, 0.736f);
    m_vShaderDesc.fSkinRoughnessMin = 0.5f;

    // ---- Rim Light ----
    m_vShaderDesc.fRimPower = 5.f; //색상강도
    m_vShaderDesc.fRimIntensity = 5.f; //림 강도(올리면 윤곽선 강해짐)
    m_vShaderDesc.vRimMaskRange = _float2(0.1f, 0.5f); //림 마스크 범위(안씀)

    // ---- Specular ----
    m_vShaderDesc.vSpecBoostRange = _float2(4.f, 1.f);
    m_vShaderDesc.fSpecularIntensity = 1.f;
    m_vShaderDesc.vSpecBosstMapRange = _float2(3.f, 1.f);
    m_vShaderDesc.fSpecularMapIntensity = 1.f;
    m_vShaderDesc.fUnderMapZeroDirSpecular = 1.f;
    m_vShaderDesc.fMinDiffuse = 0.05f;
    m_vShaderDesc.fSpecSoftClamp = 0.2f;
    m_vShaderDesc.fCaveMapWet = 0.1f;

    // ---- PBR Correction ----
    m_vShaderDesc.fKValue = 8.f;
    m_vShaderDesc.fMtrlRoughnessMin = 0.15f;

    // ---- Shadow ----
    m_vShaderDesc.vBackFaceShaowRange = _float2(-0.2f, 0.3f);
    m_vShaderDesc.fShadowBright = 0.9f;
    m_vShaderDesc.fShadowBackBright = 0.3f;

    // ---- GodRay ----
    m_vShaderDesc.fGodRayDensity = 1.f;
    m_vShaderDesc.fGodRayDecay = 0.98f;
    m_vShaderDesc.fGodRayWeight = 0.1f;
    m_vShaderDesc.fGodRayExposure = 0.5f; //다단계 블러먹이면서 1.5->0.5
    m_vShaderDesc.vGodRayColor = _float4{ 1.f, 1.f, 1.f, 1.f };
    m_vShaderDesc.vGodRayPosition = _float4{ -296.82, 102.69f, -69.81f, 1.f };

    // ---- GodRay Mesh ----
    m_vShaderDesc.fGodRayMeshScrollSpeed = 0.02f;
    m_vShaderDesc.fGodRayMeshSwayFreq = 1.f;
    m_vShaderDesc.fGodRayMeshSwayPhase = 1.f;
    m_vShaderDesc.fGodRayMeshSwayAmp = 0.14f;
    m_vShaderDesc.fGodRayMeshContrast = 1.5f;
    m_vShaderDesc.fGodRayMeshIntensity = 1.1f;
    m_vShaderDesc.fGodRayMeshEdgeFade = 0.79f;
    m_vShaderDesc.vGodRayMeshColor = _float3{ 1.f, 0.9f, 0.7f };
    m_vShaderDesc.fSoftParticleRange = 0.1f;
    m_vShaderDesc.fGodRayMeshAngleFadePower = 2.0f;
    m_vShaderDesc.fGodRayMeshFadeNear = 3.6f;
    m_vShaderDesc.fGodRayMeshFadeFar = 13.4f;

    //캠모션블러
#ifdef _DEBUG
    m_vShaderDesc.fCamMotionBlurIntensity = 1.0f; //0.3f
#else
    m_vShaderDesc.fCamMotionBlurIntensity = 1.f; //0.3f
#endif
    m_vShaderDesc.fCamMotionBlurMaxVelocity = 0.05f;
    m_vShaderDesc.fCamMotionBlurSamples = 12.f;

    // ---- Monster Specular ----
    m_vShaderDesc.fMonsterSpecularIntensity = 1.f;
    m_vShaderDesc.vSpecMonsterBoostRange = _float2(0.5f, 1.f);

}

void Engine::Renderer::ChangeChurchBoss()
{
    //갓레이색변경 + 위치변경
    m_vShaderDesc.vGodRayColor = _float4{ 1.f, 0.9f, 0.7f, 1.f };
    m_vShaderDesc.vGodRayPosition = _float4{ -56.23, 27.98f, -16.22f, 1.f };
    m_vShaderDesc.fSoftParticleRange = 7.f;
  /*  m_vShaderDesc.vShadowTintColor = _float3(0.06f, 0.05f, 0.03f);
    m_vShaderDesc.vMidtoneTintColor = _float3(1.15f, 1.05f, 0.8f);
    m_vShaderDesc.vHighlightTintColor = _float3(1.f, 1.f, 0.75f);*/
}

void Engine::Renderer::ChangeEnd()
{
    // ---- GodRay Mesh ----
    m_vShaderDesc.fGodRayMeshScrollSpeed = 0.02f;
    m_vShaderDesc.fGodRayMeshSwayFreq = 1.f;
    m_vShaderDesc.fGodRayMeshSwayPhase = 1.f;
    m_vShaderDesc.fGodRayMeshSwayAmp = 0.14f;
    m_vShaderDesc.fGodRayMeshContrast = 1.5f;
    m_vShaderDesc.fGodRayMeshIntensity = 4.1f;
    m_vShaderDesc.fGodRayMeshEdgeFade = 0.8f;
    m_vShaderDesc.vGodRayMeshColor = _float3{ 1.f, 0.9f, 0.7f };
    m_vShaderDesc.fSoftParticleRange = 0.1f;
    m_vShaderDesc.fGodRayMeshAngleFadePower = 2.0f;
    m_vShaderDesc.fGodRayMeshFadeNear = 3.6f;
    m_vShaderDesc.fGodRayMeshFadeFar = 13.4f;
}

_float3 Engine::Renderer::LerpFloat3(const _float3& _a, const _float3& _b, float _t) {
    XMVECTOR va = XMLoadFloat3(&_a);
    XMVECTOR vb = XMLoadFloat3(&_b);
    XMFLOAT3 result = {};
    XMStoreFloat3(&result, XMVectorLerp(va, vb, _t));
    return result;
}
/******************************************************* 맵변환 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 비우기 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Renderer::Clear_Renderer()
{
    for (auto& group : m_vecObjects)
    {
        for (auto& object : group)
        {
            Safe_Release_PerFrame(object);
        }
        group.clear();
    }

    return S_OK;
}
/******************************************************* 컨테이너 비우기 함수 *******************************************************/



//////////////////////////////////////////////////////// 옥트리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Renderer::Build_OcTree(const _float3& _worldCenter, _float _worldHalfSize, _uint _maxDepth)
{
    /* 기존 Octree가 있으면 정리 */
    Clear_OcTree();

    m_pOctree = OcTree::Create(_worldCenter, _worldHalfSize, _maxDepth);
    MSG_NULL(m_pOctree, L"Octree 생성 실패", L"Caution!!!", E_FAIL);

    return S_OK;
}

HRESULT Engine::Renderer::Add_StaticObject(RENDER_GROUP _group, GameObject* _staticobject, const BoundingBox& _worldAABB)
{
    MSG_NULL(m_pOctree, L"Octree가 아직 생성되지 않았습니다.", L"Caution!!!", E_FAIL);
    MSG_NULL(_staticobject, L"GameObject가 nullptr입니다.", L"Caution!!!", E_FAIL);

    /* UMAP에 데이터를 저장 */
    STATIC_OBJ_INFO info;
    info.pObject = _staticobject;
    info.pModel = _staticobject->Get_Model();
    info.iModelTypeID = (info.pModel) ? info.pModel->Get_ModelID() : 0;
    info.eRenderGroup = _group;
    info.tAABB = _worldAABB;
    //info.bIsOccluder = Occlusion::IsGoodOccluder(_worldAABB, m_fOccluderAreaThreshold); // 이 Model이 Occluder인지 판별

    _float ex = _worldAABB.Extents.x;
    _float ey = _worldAABB.Extents.y;
    _float ez = _worldAABB.Extents.z;
    info.fMaxFaceArea = max(ex * ey, max(ey * ez, ex * ez));

    // UMAP에 값 저장 (Key: GameObject*, Value: Info)
    m_umapStaticObjectInfo[_staticobject] = info;

    if (info.pModel == nullptr)
    {
        COUT("InstanceBuffer에 Model이 들어가지 않았습니다: " << wstringToString(info.pObject->Get_Name()));
    }

    /* 맵에 저장된 실제 메모리 주소를 가져옴 */
    // UMAP은 rehashing이 일어나도 요소의 메모리 주소는 유지됨 (Iterator만 무효화됨)
    const STATIC_OBJ_INFO* pInfoPtr = &m_umapStaticObjectInfo[_staticobject];

    /* Octree에는 GameObject*가 아니라 '정보 구조체의 포인터'를 삽입 */
    if (!m_pOctree->Insert(pInfoPtr))
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::Renderer::Remove_StaticObject(GameObject* _staticObject)
{

    MSG_NULL(m_pOctree, L"Octree가 아직 생성되지 않았습니다.", L"Caution!!!", E_FAIL);
    MSG_NULL(_staticObject, L"GameObject가 nullptr입니다.", L"Caution!!!", E_FAIL);

    auto iter = m_umapStaticObjectInfo.find(_staticObject);
    if (iter == m_umapStaticObjectInfo.end())
        return E_FAIL;

    const STATIC_OBJ_INFO* pInfoPtr = &iter->second;

    // 옥트리에서 해당 포인터를 찾아 제거
    if (!m_pOctree->Remove(pInfoPtr))
        return E_FAIL;

    // 정보맵에서도 제거
    m_umapStaticObjectInfo.erase(iter);

    return S_OK;
}

void Engine::Renderer::Query_AABB(const BoundingBox& _aabb, vector<const STATIC_OBJ_INFO*>& _outInfos) const
{
    if (m_pOctree)
        return m_pOctree->Query_AABB(_aabb, _outInfos);
}
void Engine::Renderer::Query_StaticObjects_ForInstancing()
{
    // 그룹에 있는 오브젝트들을 비워줌(다음 렌더링을 위해서 지난 프레임 정보 제외)
    for (auto& group : m_vecInstanceGroup)
        group.vecObjects.clear();

    // 옥트리가 없다면 분류하지 않음(이럼 버그임)
    if (m_pOctree == nullptr)
        return;

    // 이 전 프레임의 쿼리 개수를 기억해서 절반정도 공간 미리 확보해두기
    _uint previousCount = (_uint)m_vecQueryInfos.size();
    m_vecQueryInfos.clear();
    m_vecQueryInfos.reserve(previousCount + (previousCount >> 1));

    FrustumPlanes cachedFrustum;
    m_pGameInstance->Get_Frustum_Planes(cachedFrustum.planes);

    //CpuTimer timerQuery;
    //timerQuery.Begin();
    m_pOctree->Query_Frustum_Optimized(m_vecQueryInfos, cachedFrustum);
    //_double queryMs = timerQuery.End();

    ///* 출력 매 프레임 출력하면 느려지니 60프레임마다 */
    //static _uint frameCount = 0;
    //if (++frameCount % 60 == 0)
    //{
    //    char buf[128];
    //    sprintf_s(buf, "[Profile] CPU queryMs: %.3f ms | Query Results: %zu\n",
    //        queryMs, m_vecQueryInfos.size());
    //    OutputDebugStringA(buf);
    //}


#pragma region Occlusion
    ///////////////// Occlusion /////////////////
//_float4x4 viewMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTS_VIEW);
//_float4x4 projMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTS_PROJ);
//_matrix matView = XMLoadFloat4x4(&viewMatrix);
//_matrix matProj = XMLoadFloat4x4(&projMatrix);
//_matrix matVP = matView * matProj;

//CpuTimer timerOccluder;
//timerOccluder.Begin();

//m_pOcclusion->BeginFrame();

//_uint occluderCount = 0;
//for (const auto* pInfo : m_vecQueryInfos)
//{
//    if (pInfo->bIsOccluder)
//    {
//        m_pOcclusion->RenderOccluder_AABB(pInfo->tAABB, matVP);
//        ++occluderCount;
//    }
//}
//double occluderMs = timerOccluder.End();

//CpuTimer timerTest;
//timerTest.Begin();

//_uint visibleCount = 0;
//_uint culledCount = 0;
//_uint skippedCount = 0;
//_uint testedCount = 0;

//// Occluder 10개 미만이면 가릴 게 없으니 전부 visible 처리
//_bool bDoOcclusionTest = (occluderCount >= 10);

//// Occludee 최소 면적 (이 이하는 테스트 비용 > 이득)
//static constexpr _float OCCLUDEE_MIN_AREA = 2.f;
//static constexpr _uint MAX_OCCLUSION_TESTS = 3000;
#pragma endregion Occlusion

    _float4 camPos = m_pGameInstance->Get_CameraPosition();
    

    for (const auto* pInfo : m_vecQueryInfos)
    {
        if (pInfo->pObject && !pInfo->pObject->Is_Visible())
            continue;
#pragma region Occlusion
        //_bool isVisible = true;

        //if (bDoOcclusionTest && !pInfo->bIsOccluder)
        //{
        //    // 캐싱된 면적으로 빠르게 판단
        //    if (pInfo->fMaxFaceArea > OCCLUDEE_MIN_AREA && testedCount < MAX_OCCLUSION_TESTS)
        //    {
        //        isVisible = m_pOcclusion->IsBoxVisible(pInfo->tAABB, matVP);
        //        ++testedCount;
        //    }
        //    else
        //    {
        //        ++skippedCount; // 상한 초과 또는 면적 부족 → 테스트 안 하고 그냥 보임 처리
        //    }
        //}
        //else if (pInfo->bIsOccluder)
        //{
        //    pInfo->pModel->Add_DebugRender();
        //}

        //if (!isVisible)
        //{
        //    ++culledCount;
        //    continue;
        //}

        //++visibleCount;
#pragma endregion Occlusion

        // 거리 기반 소형 오브젝트 컬링
        _float3 objCenter = pInfo->tAABB.Center;
        _float dx = objCenter.x - camPos.x;
        _float dy = objCenter.y - camPos.y;
        _float dz = objCenter.z - camPos.z;
        _float distSq = dx * dx + dy * dy + dz * dz;

        // AABB의 가장 긴 extent로 크기 판단
        _float maxExtent = max(pInfo->tAABB.Extents.x,
            max(pInfo->tAABB.Extents.y, pInfo->tAABB.Extents.z));

        // screen-space 크기 근사: extent / distance
        // 비율이 임계값 이하면 화면에서 너무 작으므로 스킵
        static _float CULL_THRESHOLD_SQ = 0.0004f;
        if(m_bCapture)
            CULL_THRESHOLD_SQ = 0.0000000004f; // 크기 조절 //원래이거고 나중에 구울때 더 작게 주기
        else
            CULL_THRESHOLD_SQ = 0.0001f; // 크기 조절 //원래이거고 나중에 구울때 더 작게 주기

        if (maxExtent * maxExtent < CULL_THRESHOLD_SQ * distSq)
            continue;

        if (pInfo->eRenderGroup == RENDER_GROUP::NONBLEND && pInfo->pModel != nullptr)
        {
            _uint modelID = pInfo->iModelTypeID;
            if (modelID >= m_vecInstanceGroup.size())
                m_vecInstanceGroup.resize(modelID + 10);

            auto& group = m_vecInstanceGroup[modelID];
            if (group.pModel == nullptr)
                group.pModel = pInfo->pModel;

            group.vecObjects.push_back(pInfo->pObject);
        }
        else
        {
            m_vecObjects[UINT(pInfo->eRenderGroup)].push_back(pInfo->pObject);
            Safe_AddRef_PerFrame(pInfo->pObject);
        }
    }
}

void Engine::Renderer::Render_NonBlend_Instanced(const _float fTimeDelta)
{
    m_fAccTime += fTimeDelta;
    if (m_vecInstanceGroup.empty())
        return;
    
    // 인스턴싱으로 그리는 맵 오브젝트들은 스텐실 1 기록
    m_pGameInstance->Set_DepthStencilState(DSSTATE::STENCIL_WRITE, 1);

#pragma region CPUTest
    //static _uint frameCount3 = 0;
    //_uint totalDrawCalls = 0;  // [추가] 실제 GPU Draw 호출 수
    //_uint totalStateChanges = 0;  // [추가] 텍스처 바인딩 횟수
    //_uint drawCallCount = 0;
    //_uint totalTriangles = 0;
#pragma endregion CPUTest

    // 모델별로 나눠져있는 그룹 순회
    for (size_t modelID = 0; modelID < m_vecInstanceGroup.size(); ++modelID)
    {
        InstanceGroup& group = m_vecInstanceGroup[modelID];

        // Query에서 한 번 걸렀지만 혹시 모르니까 한번 더 거름
        if (group.vecObjects.empty() || group.pModel == nullptr)
            continue;


        // 개수가 적으면 일반 렌더링으로 처리
        if (group.vecObjects.size() < INSTANCE_THRESHOLD)
        {
            // 인스턴싱 버퍼 업데이트 없이, 개별 객체의 Render 함수 호출
            for (auto& pObject : group.vecObjects)
            {
                if (pObject)
                {
                    pObject->Render(fTimeDelta);
                }
            }
            //totalDrawCalls += (_uint)group.vecObjects.size();

            continue; // 다음 그룹으로
        }

        //++drawCallCount;

        // WorldMatrix 추출
        m_vecMatrices.clear();
        m_vecMatrices.reserve(group.vecObjects.size());

        // 
        for (auto& object : group.vecObjects)
        {
            _float4x4 worldMatrix;
            XMStoreFloat4x4(&worldMatrix, object->Get_WorldMatrix());
            // 피킹을 위해 worldmatrix의 Look.w에 OBJID를 넣어둠(쉐이더에서 사용)
            worldMatrix._14 = _float(object->Get_ObjectID());
            m_vecMatrices.push_back(worldMatrix);
        }

#pragma region MaxInstanceNum을 넘어간 모든 객체 그리기
        //// m_iMaxInstanceNum을 넘어가도 모든 객체 다 그리기 위해
        //_uint totalCount = (_uint)m_vecMatrices.size(); // 전체 인스턴스 개수
        //_uint drawnCount = 0;                           // 지금까지 그려진 객체수

        //// 그룹에서 Model 가져옴
        //Model* pModel = group.pModel;

        //while (drawnCount < totalCount)
        //{
        //    // 이번 턴에 그릴 개수 (남은 것 vs 5000개 중 작은 값)
        //    _uint drawCount = min(totalCount - drawnCount, m_iMaxElementCount);

        //    // 포인터 연산으로 해당 위치의 데이터만 전송
        //    // &m_vecMatrices[drawnCount] : 현재 그릴 데이터의 시작 주소
        //    m_pInstanceBuffer->Update_RawData(&m_vecMatrices[drawnCount], drawCount);

        //    // 쉐이더 설정 및 그리기
        //    m_pInstanceShader->Begin(0);
        //    for (_uint i = 0; i < pModel->Get_NumMeshes(); ++i)
        //    {
        //        pModel->Bind_Material(m_pInstanceShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
        //        //model->Bind_Material(m_pInstanceShader, "g_NormalTexture", i, aiTextureType_Normal);

        //        m_pInstanceShader->Commit(0);

        //        Mesh* mesh = pModel->Get_Meshes()[i];
        //        mesh->Bind_Resource();

        //        m_pInstanceBuffer->Bind_Buffer(sizeof(VTXMESH)); // IA 설정
        //        m_pInstanceBuffer->Render_Instanced(mesh->Get_NumIndices()); // DrawCall
        //    }

        //    // 다음 배치를 위해 오프셋 증가
        //    drawnCount += drawCount;
        //}
#pragma endregion MaxInstanceNum을 넘어간 모든 객체 그리기


#pragma region 기존 방식(m_iMaxInstanceNum만큼만 그림)
        m_pInstanceBuffer->Update_InstanceData(m_vecMatrices);

        //_uint instanceCount = (_uint)group.vecObjects.size();
        //_uint meshCount = model->Get_NumMeshes();
        //totalDrawCalls += meshCount;        // 메시당 1회 Draw
        //totalStateChanges += meshCount;     // 메시당 1회 텍스처 바인딩
        Model* model = group.pModel;
        
        m_pInstanceShader->Begin(0); //
        for (_uint i = 0; i < model->Get_NumMeshes(); ++i)
        {
            _uint test = 0;
            model->Bind_Material_FullSlot(m_pInstanceShader, 0, i, aiTextureType_DIFFUSE, 0, &test);
            model->Bind_Material_FullSlot(m_pInstanceShader, 2, i, aiTextureType_NORMALS, 0, &test);

            model->Bind_Material_FullSlot(m_pInstanceShader, 4, i, aiTextureType_METALNESS, 0, &test);
            model->Bind_Material_FullSlot(m_pInstanceShader, 10, i, aiTextureType_EMISSIVE, 0, &test);
            model->Bind_Material_FullSlot(m_pInstanceShader, 3, i, aiTextureType_OPACITY, 0, &test);
            model->Bind_Material_FullSlot(m_pInstanceShader, 6, i, aiTextureType_SHININESS, 0, &test);
            model->Bind_Material_FullSlot(m_pInstanceShader, 5, i, aiTextureType_SPECULAR, 0, &test);
            // 1 g_bitFlag
            m_pInstanceShader->Bind_RawValue_ByHandle(g_bitFlag, &test, sizeof(_uint));
            m_pInstanceShader->Bind_RawValue_ByHandle(g_fTime, &m_fAccTime, sizeof(_float));

            // m_pInstanceShader->Bind_RawValue_FullSlot(BUFFER_OBJECT, "g_ObjectID", &objID, sizeof(_uint));

            m_pInstanceShader->Commit(0);
            Mesh* mesh = model->Get_Meshes()[i];
            mesh->Bind_Resource();

            //totalTriangles += (model->Get_Meshes()[i]->Get_NumIndices() / 3) * instanceCount;

            m_pInstanceBuffer->Bind_Buffer(sizeof(_float4x4));
            m_pInstanceBuffer->Render_Instanced(mesh->Get_NumIndices());
        }
#pragma endregion 기존 방식(m_iMaxInstanceNum만큼만 그림)
    }
    //if (++frameCount3 % 60 == 0)
    //{
    //    char buf[256];
    //    sprintf_s(buf, "[Profile] TotalTriangles: %u | Groups: %u | Draws: %u | Visible: %zu\n",
    //        totalTriangles, drawCallCount, totalDrawCalls, m_vecQueryInfos.size());
    //    OutputDebugStringA(buf);
    //}

    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);       // 마지막에 DSS 복원
}

void Engine::Renderer::Clear_OcTree()
{
    for (auto& [object, info] : m_umapStaticObjectInfo)
    {
        m_pOctree->Remove(&info);
    }
    m_umapStaticObjectInfo.clear();

    for (auto& group : m_vecInstanceGroup)
    {
        group.vecObjects.clear();
        group.pModel = nullptr;
    }

    Safe_Release(m_pOctree);
}
/******************************************************* 옥트리 함수 *******************************************************/




void Engine::Renderer::Add_InstanceBatch(const wstring& strProtoTag, Model* pModel, _float4x4 WorldMatrix)
{
    if (nullptr == pModel)
        return;

    INSTANCE_BATCH_DATA& batchData = m_InstanceBatches[strProtoTag];

    if (nullptr == batchData.pMainModel)
        batchData.pMainModel = pModel;

    batchData.vecInstanceMatrices.push_back(WorldMatrix);
}

void Engine::Renderer::Render_Instancing(const _float fTimeDelta)
{
    if (m_InstanceBatches.empty())
        return;

    //m_pGameInstance->Set_RasterizerState(RSTATE::DEFAULT);
    //m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    //m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);

    //CHECK_FAILED(m_pGameInstance->Begin_MRT(MRT_GAMEOBJECT), );

    //Shader* pInstancingShader = m_pGameInstance->Get_Shader_Prototype(L"Prototype_Component_Shader_VTXMeshInstance");
    //if (nullptr == pInstancingShader)
    //{
    //    m_pGameInstance->End_MRT();
    //    return;
    //}

    //_float4x4 ViewMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_VIEW);
    //_float4x4 ProjMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_PROJ);

    //for (auto& Pair : m_InstanceBatches)
    //{
    //    Model* pModel = Pair.second.pMainModel;
    //    auto& vecMatrices = Pair.second.vecInstanceMatrices;
    //    if (!pModel || vecMatrices.empty())
    //        continue;

    //    pInstancingShader->Bind_Matrix("g_ViewMatrix", ViewMatrix);
    //    pInstancingShader->Bind_Matrix("g_ProjMatrix", ProjMatrix);

    //    CHECK_FAILED(pInstancingShader->Begin(0), );
    //    CHECK_FAILED(pInstancingShader->Bind_Resources(0), );

    //    pModel->Render_Instancing(pInstancingShader, vecMatrices);

    //    vecMatrices.clear();
    //}
    //m_InstanceBatches.clear();

    //CHECK_FAILED(m_pGameInstance->End_MRT(), );
}

void Engine::Renderer::Render_WorldUI(const _float fTimeDelta)
{
    if (m_vecObjects[UINT(RENDER_GROUP::WORLD_UI)].empty())
        return;

    sort(m_vecObjects[UINT(RENDER_GROUP::WORLD_UI)].begin(),
        m_vecObjects[UINT(RENDER_GROUP::WORLD_UI)].end(),
        [](GameObject* a, GameObject* b) {
            return a->Get_CombinedZ() < b->Get_CombinedZ(); // 작은 값이 뒤라고 가정
        });
    /*루프를 돌면서 blurui가있는지 확인*/
    bool bNeedBlur = false;
    for (auto& UI : m_vecObjects[UINT(RENDER_GROUP::WORLD_UI)]) {
        if (UI && UI->Is_Active() && UI->IsBlurUI()) {
            bNeedBlur = true;
            Captured_Blur_At_OneFrame();
            break;
        }
    }



    m_pGameInstance->Bind_PipeLine_All(m_pOnlyBindingShader);
    m_pOnlyBindingShader->Bind_Resources(0);

    m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);
    m_pGameInstance->Set_DepthStencilState(DSSTATE::DEFAULT);
    m_pGameInstance->Set_BlendState(BSTATE::BLEND);

    m_pGameInstance->Reset_MRT();

    CHECK_FAILED(m_pGameInstance->Begin_MRT_HDR(MRT::HDR, m_pDSV, false), );

    for (auto& UI : m_vecObjects[UINT(RENDER_GROUP::WORLD_UI)])
    {
        if(UI != nullptr)
        {
            UI->Render(fTimeDelta);
        }
    }

    if (m_iMaxGroupElementCount[_UINT(RENDER_GROUP::WORLD_UI)] < (_uint)m_vecObjects[UINT(RENDER_GROUP::WORLD_UI)].size())
        m_iMaxGroupElementCount[_UINT(RENDER_GROUP::WORLD_UI)] = (_uint)m_vecObjects[UINT(RENDER_GROUP::WORLD_UI)].size();

 

    CHECK_FAILED(m_pGameInstance->End_MRT(), );
}



//////////////////////////////////////////////////////// 인스턴싱 데이터 삭제 함수 ////////////////////////////////////////////////////////
void Engine::Renderer::Remove_RenderObject(GameObject* pGameObject)
{
}
/******************************************************* 인스턴싱 데이터 삭제 함수 *******************************************************/


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Renderer* Engine::Renderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    Renderer* pInstance = new Renderer(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(), L"Renderer 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Renderer::Free()
{
    __super::Free();

    Clear_OcTree();

    Clear_Renderer();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pShader);
    Safe_Release(m_pInstanceShader);
    Safe_Release(m_pOnlyBindingShader);
    Safe_Release(m_pInstanceBuffer);
    Safe_Release(m_pVIBuffer);
    Safe_Release(m_pDSV);
    Safe_Release(m_pOcclusion);
    //Safe_Release(m_pHiZBuffer);

#ifdef _DEBUG
    //m_gpuProfiler.Release();
#endif // _DEBUG



    for (auto& it : m_pDSVCascade)
    {
        Safe_Release(it);
    }

    Safe_Release(m_pSceneTexture);
    Safe_Release(m_pSceneSRV);

    Safe_Release(m_pSceneSRV_ForUI);
    Safe_Release(m_pSceneTexture_ForUI); // 임시 생성했으므로 해제


#ifdef _DEBUG
    for (auto& component : m_DebugComponents)
    {
        Safe_Release(component);
    }
    m_DebugComponents.clear();

    Safe_Delete(m_pDebugBatch);
    Safe_Delete(m_pDebugEffect);
    Safe_Release(m_pDebugInputLayout);
#endif


#if defined(DEBUG) || defined(_DEBUG)
    ID3D11Debug* d3dDebug;
    HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
    if (SUCCEEDED(hr))
    {
        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
        OutputDebugStringW(L"                                                                    Renderer Elements Max Count \r ");
        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

        wstring debugMsg = {};
        for (size_t i = 0; i < UINT(RENDER_GROUP::END); i++)
        {
            debugMsg = L"Group [" + to_wstring(i) + L"] m_iMaxGroupElementCount : " + to_wstring(m_iMaxGroupElementCount[i]) + L"\r\n";
            OutputDebugStringW(debugMsg.c_str());
        }
        debugMsg = L"m_iMaxElementCount : " + to_wstring(m_iMaxElementCount) + L"\r\n";
        OutputDebugStringW(debugMsg.c_str());

        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
        OutputDebugStringW(L"                                                                    Renderer Elements Max Count END \r ");
        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
    }
    if (d3dDebug != nullptr)            d3dDebug->Release();
#endif

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);


}
/******************************************************* 객체 반환 함수 *******************************************************/


