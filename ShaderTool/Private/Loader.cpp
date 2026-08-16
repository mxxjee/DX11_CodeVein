#include "ShaderTool_Define.h"
#include "Loader.h"
#include "GameInstance.h"

//여기서 패스를 만든다
#pragma region GameObject
#include "Monster.h"
#include "Monster_Body.h"
#include "Terrain.h"
#include "LightArrow.h"
#include "BossMap.h"
#include "LightComponent.h"
#include "StaticObject.h"
#include "SavePoint.h"
#include "VIBuffer_SkySphere.h"
#include "Sky_Sphere.h"
#include "Ladder.h"
#pragma endregion


#pragma region UIObject
#include "UI_Test.h"

#pragma endregion


#pragma region Camera
#include "CameraFree.h"

#pragma endregion

#pragma region Player
#include "Player.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
//Weapon
#include "PWeapon_Bayonet.h"
#include "GodRayObject.h"
#include "GodRayMesh.h"
#include "GodRay_Sun.h"
#include "Monster_Body.h"
#pragma endregion

_bool Loader::m_bStaticComplete = false;
_bool Loader::m_bLevelCompleteStatic[_UINT(LEVEL::END)] = { false };

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::Loader::Loader()
{
}

ShaderTool::Loader::Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

ShaderTool::Loader::~Loader()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
_uint APIENTRY ThreadMain(void* pArg)
{
    Loader* ploader = static_cast<Loader*>(pArg);

    CHECK_FAILED(ploader->Loading(), 1);

    return 0;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Loader::Initialize(LEVEL _eLevelName)
{
    m_eCreateLevel = _eLevelName;

    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

    m_pGameInstance->Clear_UIManager();

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



HRESULT ShaderTool::Loader::Loading()
{
    InitializeCriticalSection(&m_CriticalSection);

    HRESULT hr = CoInitializeEx(nullptr, 0);

    switch (m_eCreateLevel)
    {
    case LEVEL::STATIC:
        if (!m_bStaticComplete)
            hr = Load_Static();
        break;
    case LEVEL::LOGO:
        hr = Load_Logo();
        break;

    case LEVEL::MAIN:
        hr = Load_Main();
        break;
    }

    m_bIsComplete = true;

    LeaveCriticalSection(&m_CriticalSection);

    if (FAILED(hr))
    {
        MSG_ON(L"레벨 로딩 실패!!!", L"Caution");
        return E_FAIL;
    }


    return S_OK;
}



//////////////////////////////////////////////////////// 전역 프로토타입 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Loader::Load_Static()
{
    LEVEL level = LEVEL::STATIC;
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Collider_Sphere, Collider::Create(m_pDevice, m_pContext, COLLIDER::SPHERE)), E_FAIL);

    COUT("전역 컴포넌트 로딩중");
    /*Main으로 옮김*/
    ///* For.Prototype_Component_VIBuffer_Rect */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
    //    VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"Sky_Sphere"),
        Texture::Create(m_pDevice, m_pContext, L"../../Resources/Textures/Sky/Sky_Sphere_%02d.png", 2)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"),
        Texture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/Rim/T_FX_ExternalNoise08.png", 1)), E_FAIL);

    /* For.Prototype_Component_VIBuffer_Rect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
        VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

   /* For.Prototype_Component_VICube */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VICube,
        VIBuffer_Cube::Create(m_pDevice, m_pContext)), E_FAIL);

    /* For.Prototype_Component_VISkyDome */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VISkyDome,
        VIBuffer_Skydome::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VISkySphere,
        VIBuffer_SkySphere::Create(m_pDevice, m_pContext, 32, 16, 1.f)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VISphere,
        VIBuffer_SkySphere::Create(m_pDevice, m_pContext, 32, 16, 10.f, true)), E_FAIL);

    /* For.Prototype_Component_Texture_PlayerFaceBrow */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceBrow"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/Brows")), E_FAIL);

    /* For.Prototype_Component_Texture_PlayerFaceEyelash */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyelash"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/Eyelash")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyewhite"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/EyeWhite")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyeDetail"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/EyeDetail")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFaceEyeHighlight"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/EyeHighlight")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"PlayerFacePaints"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/Player/FacePaints")), E_FAIL);

    LIGHT_DESC Desc = {};
    Desc.eType = LIGHT::POINT;
    Desc.fRange = 10.f;
    Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.vDiffuse = _float4(1.f, 1.f, 7.f, 1.f);
    Desc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Proto_Component_Point_Light",
        LightComponent::Create(m_pDevice, m_pContext, Desc)), E_FAIL);

    Desc = {};
    Desc.eType = LIGHT::SPOTLIGHT;
    Desc.fRange = 10.f;
    Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
    Desc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
    Desc.vDirection = _float4(0.18f, -1.f, 0.06f, 1.f);
    Desc.fInnerCone = cos(XMConvertToRadians(5.5f));
    Desc.fOuterCone = cos(XMConvertToRadians(25.5f));
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Proto_Component_Spot_Light",
        LightComponent::Create(m_pDevice, m_pContext, Desc)), E_FAIL);

    COUT("전역 텍스쳐 로딩중");


    COUT("전역 쉐이더 로딩중");
    SHADERENTRY entry[1] = { "VS_MAIN", "PS_MAIN" };
    SHADERENTRIES entries;
    entries.pEntries = entry;
    entries.iNumpass = 1;
    /* For.Prototype_Component_Shader_VTXCube */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXCube,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxCube.hlsl", VTXCUBE::Elements, VTXCUBE::iNumElements, entries)), E_FAIL);

    /* For.Prototype_Component_Shader_VTXBlendMesh */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Shader(L"VtxBlendMesh"),
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxBlendMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements, entries)), E_FAIL);

    /* For.Prototype_Component_Shader_Particle_Rect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Particle_Rect,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex_Particle.hlsl", VTXPOSTEX_PARTICLE::Elements, VTXPOSTEX_PARTICLE::iNumElements, entries)), E_FAIL);

    /* For.Prototype_Component_Shader_SkyDome */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Shader(L"SkyDome"),
    //    Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_SkyDome.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries)), E_FAIL);

    //SHADERENTRY entryToon[4] = {
    //    {"VS_OUTLINE", "PS_OUTLINE"},   // Pass 0: 아웃라인
    //    {"VS_MAIN", "PS_MAIN"},         // Pass 1: 기본 툰 셰이딩
    //    {"VS_MAIN", "PS_HAIR"},         // Pass 2: 머리카락
    //    {"VS_MAIN", "PS_MAIN_FALLBACK"} // Pass 3: 폴백
    //};
    //entries.pEntries = entryToon;
    //entries.iNumpass = 4;

    ///* For.Prototype_Component_Shader_Toon */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Shader(L"Toon"),
    //    Shader::Create(m_pDevice, m_pContext, L"../Shader/Shader_Toon.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryAnim[2] = {
        {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
        {"VS_MAIN", "PS_MAIN_SHADOW"}   // Pass 1 : 그림자 그리기
    };
    entries.pEntries = entryAnim;
    entries.iNumpass = 2;

    /* For.Prototype_Component_Shader_VTXNorTex */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXNorTex,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryPlayerAnim[9] = {
        {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
        {"VS_MAIN_SHADOW", "PS_MAIN_SHADOW"},   // Pass 1 : 그림자 그리기
        {"VS_MAIN_NORMAL","PS_MAIN_NORMAL_PBR"},//Pass 2 : PBR기본 패스
        {"VS_MAIN_NORMAL", "PS_CLOTH"}, //Pass 3: 옷 전용 패스
        {"VS_MAIN_NORMAL","PS_HAIR"},//Pass 4: 헤어 전용 패스
        {"VS_MAIN_NORMAL","PS_FACE"}, // PAss 5 : 얼굴 전용 패스 : 그지같다진짜로
        {"VS_MAIN_NORMAL","PS_EYE"}, // Pass 6 : 눈 전용 패스 
        { "VS_MAIN_NORMAL","PS_FACE_BACKUP" }, // Pass 7 : 임시패스
        { "VS_MAIN_NORMAL","PS_MAIN_NORMAL_PBR_RIM" },//Pass 8 : 림들어가는애들 패스(일단 따로파기)
    };
    entries.pEntries = entryPlayerAnim;
    entries.iNumpass = 9;

    CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Shader(L"VTXPlayerAnimMesh"), L"../../Shader/Shader_PlayerAnimMesh.hlsl", VTXPLAYERANIMMESH::Elements, VTXPLAYERANIMMESH::iNumElements, entries), E_FAIL);

    /* 모델 애니메이션용 컴쉐 */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Bone",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_BONECOMBINED")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_LocalMatrix",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_EVALUATELOCAL")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Readback",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_READBACK")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_DrivenBones", //런타임에 SRT를 원하는 뼈에 적용
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_APPLYDRIVENBONES")), E_FAIL);

    SHADERENTRY entryAnim1[12] = {
        {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
        {"VS_MAIN", "PS_MAIN_SHADOW"},   // Pass 1 : 그림자 그리기
        {"VS_MAIN", "PS_MAIN" },         // Pass 2 : 쉐이더 파싱 테스트용
        {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL" },  // pass 3 : 노말 그리기
        {"VS_MAIN_SHADOW_CASCADE", "PS_MAIN_CASCADE_SHADOW" }, // pass 4 : 케스케이드 그림자 그리기
        {"VS_MAIN_NORMAL", "PS_MAIN_SAVEPOINT" }, // pass 5 : 세이브포인트용(emission, opacity 야매로 구성)
        { "VS_MAIN_NORMAL", "PS_CHARACTER_EYE" },  // Pass 6 : 눈동자 전용 패스
        { "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR" },  // Pass 7 : 몬스터 PBR 패스전용
        { "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE" },  // Pass 8 : 몬스터 PBR 디졸브 패스전용
        { "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_CV" },       // Pass 9 : GhostWolf용
        { "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE_MONSTER" },  // Pass 10 : 몬스터 눈깔테스트용
        { "VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_DISSOLVE_GIANTVAMPIRE" },  // Pass 11 : 자이언트뱀파이어용
    };
    entries.pEntries = entryAnim1;
    entries.iNumpass = 12;

    /* For.Prototype_Component_Shader_VTXAnimMesh */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXAnimMesh,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_AnimMesh.hlsl", VTXANIMMESH::Elements, VTXANIMMESH::iNumElements, entries)), E_FAIL);
        

    SHADERENTRY entryAlpha[10] = {
        {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
        {"VS_MAIN", "PS_MAIN_SHADOW"},  // Pass 1 : 그림자 그리기
        {"VS_MAIN", "PS_MAIN_ALPHA"},    // Pass 2 : 알파 들어간거 그리기
        {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL"}, // pass 3 : 노말 들어간거 그리기
        {"VS_MAIN", "PS_MAIN_OPACITY"}, // pass 4 : Opacity 들어간거 그리기
        {"VS_MAIN_SHADOW_CASCADE", "PS_MAIN_SHADOW_CASCADE"}, // pass 5 : Cascade 그림자 그리기
        {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR"}, // pass 6 : 노말들어간 PBR
        {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_MAP"}, // pass 7 : 노말들어간 PBR 맵전용 
        {"VS_MAIN_NORMAL", "PS_MAIN_NORMAL_PBR_GODRAY"},  // pass 8 : 갓레이용
        {"VS_MAIN", "PS_MAIN_GODRAYMESH"},  // pass 9 : 갓레이메쉬용

    };
    entries.pEntries = entryAlpha;
    entries.iNumpass = 10;

    /* For.Prototype_Component_Shader_VTXMesh */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXMesh,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxMesh.hlsl", VTXMESH::Elements, VTXMESH::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryGeo[1] = { "VS_MAIN", "PS_MAIN", "GS_MAIN" };
    entries.pEntries = entryGeo;
    entries.iNumpass = 1;

    /* For.Prototype_Component_Shader_Particle_Rect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Particle_Point,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPos_Particle.hlsl", VTXPOS_PARTICLE::Elements, VTXPOS_PARTICLE::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryNormal[1] = { "VS_MAIN", "PS_NORMAL_SHADERTOOL_TEST" };
    entries.pEntries = entryNormal;
    entries.iNumpass = 1;
    /* For.Prototype_Component_Shader_SkyDome */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Shader(L"LightArrow"),
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries)), E_FAIL);
    
    COUT("전역 쉐이더 로딩중");
    SHADERENTRY entrySky[2] = {
        {"VS_MAIN", "PS_MAIN"},
        {"VS_MAIN_SUN", "PS_MAIN_SUN"},
    };
    entries.pEntries = entrySky;
    entries.iNumpass = 2;

    CHECK_FAILED(m_pGameInstance->Add_Shader(Proto_Com_Shader_Sky, L"../../Shader/Shader_SkySphere.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries), E_FAIL);
    
    
    COUT("전역 폰트 로딩중");

    // CHECK_FAILED(m_pGameInstance->LoadSound("Click", L"../Resources/Sounds/Click.wav"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_GameObject_SkySphere,
        Sky_Sphere::Create(m_pDevice, m_pContext, LEVEL::STATIC)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_GameObject_SphereSun,
        GodRay_Sun::Create(m_pDevice, m_pContext, LEVEL::STATIC)), E_FAIL);

    COUT("전역 로딩 완료");

    

    m_bIsComplete = true;
    m_bStaticComplete = true;

    return S_OK;
}
/******************************************************* 전역 프로토타입 *******************************************************/



//////////////////////////////////////////////////////// 로고 프로토타입 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Loader::Load_Logo()
{
    LEVEL level = LEVEL::LOGO;

    COUT("버퍼 로딩중");
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VITerrain,
    //    VIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../../Resources/ModelTest/Height.bmp")), E_FAIL);

    COUT("버퍼 완료");

    COUT("로고 컴포넌트 로딩중");


    COUT("로고 텍스쳐 로딩중");
    /* For.Prototype_Component_Texture_Logo */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture(L"White"),
        Texture::Create(m_pDevice, m_pContext, L"../../Resources/UI/White.png", 1)), E_FAIL);

    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture(L"Terrain"),
    //    Texture::Create(m_pDevice, m_pContext, L"../../Resources/ModelTest/Tile0.jpg", 1)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture(L"LightArrow"),
        Texture::Create(m_pDevice, m_pContext, L"../../Resources/ModelTest/Arrow-Photoroom.png", 1)), E_FAIL);

    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture(L"Terrain_Height"),
    //    Texture::Create(m_pDevice, m_pContext, L"../../Resources/ModelTest/Height1.bmp", 1)), E_FAIL);

    COUT("로고 동영상 로딩중");
    /* For.Prototype_Component_Video_Title */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::LOGO), Proto_Video_Title,
    //    Video::Create(m_pDevice, m_pContext, L"../Resources/Video/Opening.mp4")), E_FAIL);


    COUT("로고 사운드 로딩중");
    //CHECK_FAILED(m_pGameInstance->LoadSound("Title_Theme", L"../Resources/Sounds/Title/Title_Theme.mp3"), E_FAIL);


    COUT("로고 오브젝트 로딩중");
    /* For.Prototype_UI_Test */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::LOGO), Proto_UIObject(L"Test"),
    //    UI_Test::Create(m_pDevice, m_pContext, level)), E_FAIL);

    COUT("로고 UI오브젝트 로딩중");
    /* For.Prototype_UIObject_VideoPlayer */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject(L"Title"),
    //    VideoPlayer::Create(m_pDevice, m_pContext, level)), E_FAIL);

    COUT("모델 생성중");
    _matrix prematrix = XMMatrixIdentity();
    prematrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SavePoint"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Map_Static/SavePoint/SavePoint.siho", prematrix, MODELROLE::STANDALONE)), E_FAIL);

    prematrix = XMMatrixIdentity();
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GodRay1"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/ModelTest/GodRay/GodRay1.siho", prematrix)), E_FAIL);

    prematrix = XMMatrixIdentity();
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"GodRay2"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/ModelTest/GodRay/GodRay2.siho", prematrix)), E_FAIL);

    //SlaveDevil
   /* prematrix =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(90.f));*/
    //SlimeDevil
    //prematrix = XMMatrixIdentity() * XMMatrixRotationX(XMConvertToRadians(90.f));
    //SlaveVampire
  /*  prematrix=  XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(-90.f)) *
        XMMatrixRotationY(XMConvertToRadians(180.f));*/
     
    //SlaveDevil_Only.fbx
    //GiantVampire
    prematrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)); //* XMMatrixRotationY(XMConvertToRadians(180.f));
    //WolfGhost
    //prematrix =
    //    XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
    //    XMMatrixRotationX(XMConvertToRadians(90.f)) *
    //    XMMatrixRotationY(XMConvertToRadians(180.f));
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"SlaveDevil"),
        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../../Resources/Model/Monster/GiantWhiteDevil/GiantWhiteDevil_Mesh1.fbx", prematrix, MODELROLE::STANDALONE)), E_FAIL);

    prematrix = XMMatrixIdentity();
    //첫번째맵
    //_wstring folderPath = L"../../Resources/Model/Map/test4/"; //처음맵
    //_wstring folderPath = L"../../Resources/Model/Map/Base/"; //베이스
    _wstring folderPath = L"../../Resources/Model/Map/Duomo/"; //성당
    //_uint i = 0;
    for (const auto& entry : fs::recursive_directory_iterator(folderPath))
    {
        // .siho 파일만 처리
        if (entry.is_regular_file() && entry.path().extension() == L".siho")
        {
            //++i;
            // 현재 경로 저장
            fs::path currentPath = entry.path();

            // 파일 이름 추출 (확장자 제외)
            wstring fileName = currentPath.stem().wstring();

            // Prototype 이름 생성: "Prototype_Component_Model_Wonder_Acute"
            wstring prototypeName = L"Prototype_Component_Model_" + fileName;

            // 전체 파일 경로
            wstring filePath = entry.path().wstring();

            // Prototype 등록
            MSG_FAIL(m_pGameInstance->Add_Prototype(_UINT(level), prototypeName, Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, filePath, prematrix))
                , L"Model 로딩에 실패했습니다!", L"뭔가 잘못 불러옴", E_FAIL);
        }
    }
    prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderTop"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Top.siho", prematrix)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderMiddle"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Middle.siho", prematrix)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"LadderBottom"),
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Model/Map_Static/Ladder/Ladder_Bottom.siho", prematrix)), E_FAIL);

#pragma region Player

    prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));

    const _wstring Proto_Com_Model_PlayerMasterBone = L"Prototype_Component_Model_PlayerMasterBone";
    const _wstring Proto_Com_Model_PlayerBody_Base0 = L"Prototype_Component_Model_PlayerBody_Base0";
    const _wstring Proto_Com_Model_PlayerBody_Base1 = L"Prototype_Component_Model_PlayerBody_Base1";
    const _wstring Proto_Com_Model_PlayerBody_Base2 = L"Prototype_Component_Model_PlayerBody_Base2";
    const _wstring Proto_Com_Model_PlayerBody_Base3 = L"Prototype_Component_Model_PlayerBody_Base3";
    const _wstring Proto_Com_Model_PlayerBody_Base4 = L"Prototype_Component_Model_PlayerBody_Base4";
    const _wstring Proto_Com_Model_PlayerBody_Base5 = L"Prototype_Component_Model_PlayerBody_Base5";
    const _wstring Proto_Com_Model_PlayerBody_Base6 = L"Prototype_Component_Model_PlayerBody_Base6";

    const _wstring Proto_Com_Model_PlayerHair_Base0 = L"Prototype_Component_Model_PlayerHair_Base0";
    const _wstring Proto_Com_Model_PlayerHair_Base1 = L"Prototype_Component_Model_PlayerHair_Base1";
    const _wstring Proto_Com_Model_PlayerHair_Base2 = L"Prototype_Component_Model_PlayerHair_Base2";
    const _wstring Proto_Com_Model_PlayerHair_Base3 = L"Prototype_Component_Model_PlayerHair_Base3";
    const _wstring Proto_Com_Model_PlayerHair_Base4 = L"Prototype_Component_Model_PlayerHair_Base4";
    const _wstring Proto_Com_Model_PlayerHair_Base5 = L"Prototype_Component_Model_PlayerHair_Base5";
    const _wstring Proto_Com_Model_PlayerHead_Base = L"Prototype_Component_Model_PlayerHead_Base";
    const _wstring Proto_Com_Model_PlayerWeapon_BlackBayonet = L"Prototype_Component_Model_PlayerWeapon_BlackBayonet";
    const _wstring Proto_Com_Model_PlayerWeapon_BlackGreatSword = L"Prototype_Component_Model_PlayerWeapon_BlackGreatSword";
    const _wstring Proto_Com_Model_PlayerWeapon_BlackSword = L"Prototype_Component_Model_PlayerWeapon_BlackSword";
    const _wstring Proto_Com_Model_PlayerWeapon_BlackHalberd = L"Prototype_Component_Model_PlayerWeapon_BlackHalberd";
    const _wstring Proto_Com_Model_PlayerWeapon_WhiteHammer = L"Prototype_Component_Model_PlayerWeapon_WhiteHammer";


    /* For.Prototype_Component_Model_PlayerMasterBone */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerMasterBone,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/MasterBone.siho", prematrix, MODELROLE::MASTER)), E_FAIL);

    /* For.Prototype_Component_Model_PlayerBody_Base */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base0,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base1,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female2.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base2,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female3.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base3,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female4.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base4,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female5.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base5,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female7.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerBody_Base6,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female8.siho", prematrix, MODELROLE::PART)), E_FAIL);

    /* For.Prototype_Component_Model_PlayerHair_Base */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base0,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base1,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female2.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base2,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female3.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base3,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female4.siho", prematrix, MODELROLE::PART)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHair_Base4,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female10.siho", prematrix, MODELROLE::PART)), E_FAIL);

    /* For.Prototype_Component_Model_PlayerHead_Base */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerHead_Base,
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Face/Face_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

    prematrix = XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

    /* For.Prototype_Component_Model_PlayerWeapon_BlackBayonet */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_PlayerWeapon_BlackBayonet,
        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../../Resources/Models/Player/Weapons/Bayonet/BlackBayonet.siho", prematrix, MODELROLE::STANDALONE, true)), E_FAIL);

    m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player"),
        Player::Create(m_pDevice, m_pContext, level));

    m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_MasterRig"),
        Player_MasterRig::Create(m_pDevice, m_pContext, level));

    m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Body"),
        Player_Body::Create(m_pDevice, m_pContext, level));

    m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Hair"),
        Player_Hair::Create(m_pDevice, m_pContext, level));

    m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Head"),
        Player_Head::Create(m_pDevice, m_pContext, level));

    m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Weapon_Bayonet"),
        PWeapon_Bayonet::Create(m_pDevice, m_pContext, level));

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Slave_Devil",
        Monster_Body::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_Component_StateMachine",
        StateMachine::Create(m_pDevice, m_pContext)), E_FAIL);


#pragma endregion
    m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_SavePoint",
        SavePoint::Create(m_pDevice, m_pContext, level));

    COUT("모델 생성완료");

    COUT("로고 로딩 완료");   

    COUT("게임오브젝트 생성중")
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Free, 
        CameraFree::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_GameObject_Body_Monster"),
        Monster_Body::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_GameObject_Monster"),
        Monster::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_GameObject_Terrain"),
        Terrain::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_GameObject_LightArrow"),
        LightArrow::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_GameObject_BossMap"),
        BossMap::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Static", 
        StaticObject::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_GodRay",
        GodRayObject::Create(m_pDevice, m_pContext, level)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_GodRayMesh",
        GodRayMesh::Create(m_pDevice, m_pContext, level)), E_FAIL);

    m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Ladder",
        CLadder::Create(m_pDevice, m_pContext, level));
 /*   m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Ladder",
        StaticObject::Create(m_pDevice, m_pContext, level));*/

//#pragma region Player
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player"),
//        Player::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_MasterRig"),
//        Player_MasterRig::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Body"),
//        Player_Body::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Hair"),
//        Player_Hair::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Hand"),
//        Player_Hand::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Head"),
//        Player_Head::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Player_Pants"),
//        Player_Pants::Create(m_pDevice, m_pContext, level)), E_FAIL);
//#pragma endregion

    COUT("게임오브젝트 생성완료")
    m_bIsComplete = true;

    return S_OK;
}
/******************************************************* 로고 프로토타입 *******************************************************/



//////////////////////////////////////////////////////// 메인 프로토타입 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Loader::Load_Main()
{



    return S_OK;
}
/******************************************************* 메인 프로토타입 *******************************************************/



//////////////////////////////////////////////////////// 레이스 프로토타입 ////////////////////////////////////////////////////////
//HRESULT Client::Loader::Load_Race()
//{
//    const LEVEL level = LEVEL::RACE;
//
//    m_bLevelCompleteStatic[_UINT(LEVEL::RACE)] = true;
//
//    COUT("레이스 텍스쳐 로딩중");
//    /* For.Prototype_Component_Texture_SkyBox */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture_Sky,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/Texture2D/Sky_%d.dds", 4)), E_FAIL);
//
//    /* For.Prototype_Component_Texture_SkyDome */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture(L"SkyDome"),
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/Texture2D/SkyDome.png", 1)), E_FAIL);
//
//    /* For.Prototype_Component_Texture_Grass */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture_Grass,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/Texture2D/Effect/Race_Grass.png", 1)), E_FAIL);
//
//    /* For.Prototype_Component_Texture_Effect */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture_Effect,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/Texture2D/Effect/Effect_Flash.png")), E_FAIL);
//
//    /* For.Prototype_Component_Texture_Aura */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture_Aura,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/Texture2D/Effect/Effect_Aura.png")), E_FAIL);
//
//    /* For.Prototype_Component_Texture_Cursor */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Texture_Cursor,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/Texture2D/Effect/Player_Cursor.png")), E_FAIL);
//
//    COUT("레이스 컴포넌트 로딩중");
//    /* For.Prototype_Component_Track */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Track,
//        Track::Create(m_pDevice, m_pContext, "../DataFiles/Track01_Long.track")), E_FAIL);
//
//
//
//    COUT("레이스 캐릭터 모델 로딩중");
//    _matrix prematrix = XMMatrixIdentity();
//    prematrix = XMMatrixRotationY(XMConvertToRadians(180.f));
//
//#pragma region gltf방지
//    ///* For.Prototype_Component_Model_ForkLift */
//    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_Com_Model_ForkLift,
//    //    Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../Resources/ForkLift/ForkLift.fbx", prematrix)), E_FAIL);
//#pragma endregion
//
//
//    Character_Models(level);
//
//
//    COUT("레이스 트랙 모델 로딩중");
//    /* For.Prototype_Component_Model_Track_01 */
//    prematrix = XMMatrixIdentity();
//    //prematrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_Model_Track_01,
//        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../Resources/Models/Track/Track01/Track01.gltf", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Gate */
//    prematrix = XMMatrixIdentity();
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Gate"),
//        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../Resources/Models/Track/Gate/Gate.fbx", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Gate */
//    prematrix = XMMatrixIdentity();
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Model(L"Goal"),
//        Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, L"../Resources/Models/Track/Goal/Goal.fbx", prematrix)), E_FAIL);
//
//    //COUT("필드 셰이더 로딩중");
//
//
//    COUT("레이스 카메라 로딩중");
//    /* For.Prototype_GameObject_Camera_Free */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Free,
//        Camera_Free::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Focus */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Focus,
//        Camera_Player_Focus::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_BroadCast01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_BroadCast01,
//        Camera_BroadCast01::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_BroadCast02 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_BroadCast02,
//        Camera_BroadCast02::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_BroadCast03 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_BroadCast03,
//        Camera_BroadCast03::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_BroadCast04 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_BroadCast04,
//        Camera_BroadCast04::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_BroadCast05 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_BroadCast05,
//        Camera_BroadCast05::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Corner01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Corner01,
//        Camera_Corner01::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Corner02 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Corner02,
//        Camera_Corner02::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Corner03 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Corner03,
//        Camera_Corner03::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Start01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Start01,
//        Camera_Start01::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_FaceFocus01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_FaceFocus01,
//        Camera_FaceFocus01::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_FaceFocus02 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_FaceFocus02,
//        Camera_FaceFocus02::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Focus01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Focus01,
//        Camera_Focus01::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_First_Focus01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("First_Focus01"),
//        Camera_First_Focus01::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_First_Focus01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject("First_Focus02"),
//        Camera_First_Focus02::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Goal */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject_Camera_Goal,
//        Camera_Goal::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Camera_Anim */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Camera_Anim"),
//        AnimCamera::Create(m_pDevice, m_pContext, level, L"../Resources/Camera/Wonder.fbx")), E_FAIL);
//
//
//
//    COUT("레이스 오브젝트 로딩중");
//    Character_Objects(level);
//
//    /* For.Prototype_GameObject_Track_01 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_GameObject_Track_01,
//        Track_01::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Sky_Box */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_GameObject_SkyBox,
//        Sky_Box::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Sky_Dome */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_GameObject(L"SkyDome"),
//        Sky_Dome::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Gate */
//    prematrix = XMMatrixIdentity();
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Gate"),
//        Gate::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_GameObject_Goal */
//    prematrix = XMMatrixIdentity();
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_GameObject(L"Goal"),
//        Goal::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//
//    COUT("레이스 이펙트 로딩중");
//    VIBuffer_Particle_Point::PARTICLE_POINT_DESC grassDesc{};
//    grassDesc.iNumInstance = 20;
//    grassDesc.IsLoop = true;
//    grassDesc.vCenter = _float3(0.f, 0.f, 0.f);
//    grassDesc.vLifeTime = _float2(0.6f, 1.0f);
//    grassDesc.vPivot = _float3(0.f, 1.f, 0.f);
//    grassDesc.vRange = _float3(0.3f, 0.1f, 0.3f);
//    grassDesc.vSize = _float2(0.08f, 0.15f);
//    grassDesc.vSpeed = _float2(1.f, 3.f);
//
//    /* For.Prototype_Component_Particle_Grass */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VIParticle_Grass,
//        VIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &grassDesc)), E_FAIL);
//
//    /* For.Prototype_GameObject_Effect_Effect_GrassTrail */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_GameObject_Effect_Grass,
//        Effect_GrassTrail::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);
//
//    VIBuffer_Particle_Point::PARTICLE_POINT_DESC skillEffectDesc{};
//    skillEffectDesc.iNumInstance = 20;
//    skillEffectDesc.IsLoop = false;
//    skillEffectDesc.vCenter = _float3(0.f, 0.f, 0.f);
//    skillEffectDesc.vLifeTime = _float2(2.f, 2.f);
//    skillEffectDesc.vPivot = _float3(0.f, 0.f, 0.f);
//    skillEffectDesc.vRange = _float3(0.1f, 0.1f, 0.1f);
//    skillEffectDesc.vSize = _float2(0.2f, 0.5f);
//    skillEffectDesc.vSpeed = _float2(0.f, 0.f);
//    /* For.Prototype_Component_Particle_Aura */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VIEffect_Flash_Aura,
//        VIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &skillEffectDesc)), E_FAIL);
//
//    VIBuffer_Particle_Point::PARTICLE_POINT_DESC sparkleDesc{};
//    sparkleDesc.iNumInstance = 25;
//    sparkleDesc.IsLoop = true;
//    sparkleDesc.vCenter = _float3(0.f, 0.f, 0.f);
//    sparkleDesc.vLifeTime = _float2(0.3f, 0.6f);
//    sparkleDesc.vPivot = _float3(0.f, 0.f, 0.f);
//    sparkleDesc.vRange = _float3(0.5f, 1.0f, 0.5f);
//    sparkleDesc.vSize = _float2(0.05f, 0.15f);
//    sparkleDesc.vSpeed = _float2(0.f, 0.f);
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VIEffect_Sparkle,
//        VIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &sparkleDesc)), E_FAIL);
//
//    VIBuffer_Particle_Point::PARTICLE_POINT_DESC speedLineDesc{};
//    speedLineDesc.iNumInstance = 10;
//    speedLineDesc.IsLoop = true;
//    speedLineDesc.vCenter = _float3(0.f, 0.f, 0.f);
//    speedLineDesc.vLifeTime = _float2(0.2f, 0.4f);
//    speedLineDesc.vPivot = _float3(0.f, 0.f, 0.f);
//    speedLineDesc.vRange = _float3(1.0f, 1.0f, 1.0f);
//    speedLineDesc.vSize = _float2(0.3f, 0.6f);
//    speedLineDesc.vSpeed = _float2(8.f, 15.f);
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VIEffect_SpeedLine,
//        VIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &speedLineDesc)), E_FAIL);
//
//    VIBuffer_Particle_Point::PARTICLE_POINT_DESC healSpiralDesc{};
//    healSpiralDesc.iNumInstance = 8;
//    healSpiralDesc.IsLoop = true;
//    healSpiralDesc.vCenter = _float3(0.f, 0.f, 0.f);
//    healSpiralDesc.vLifeTime = _float2(1.0f, 1.5f);
//    healSpiralDesc.vPivot = _float3(0.f, 0.f, 0.f);
//    healSpiralDesc.vRange = _float3(0.5f, 2.0f, 0.5f);
//    healSpiralDesc.vSize = _float2(0.05f, 0.1f);
//    healSpiralDesc.vSpeed = _float2(1.f, 2.f);
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_Com_VIEffect_HealSpiral,
//        VIBuffer_Particle_Point::Create(m_pDevice, m_pContext, &healSpiralDesc)), E_FAIL);
//
//    /* For.Prototype_GameObject_Effect_Speed */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_GameObject_Effect_Speed,
//        Effect_Speed::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);
//
//    /* For.Prototype_GameObject_Effect_Heal */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_GameObject_Effect_Heal,
//        Effect_Heal::Create(m_pDevice, m_pContext, _UINT(level))), E_FAIL);
//
//    /* For.Prototype_GameObject_PlayerCursor */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::RACE), Proto_GameObject_Effect_Cursor,
//        PlayerCursor::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//
//
//    COUT("레이스 UI 로딩중");
//    /* For.Prototype_Component_UITexture_Race_Comment_Frame */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_Frame,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Race_Comment_Frame.png", 1)), E_FAIL);
//    /* For.Prototype_UIObject_Race_Frame */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_RaceFrame,
//        UI_Race_Frame::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_Component_UITexture_Race_ProgressBar */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_ProgressBar,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Race_Progress_Bar.png", 1)), E_FAIL);
//    /* For.Prototype_Component_UITexture_Race_ProgressSlash */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_ProgressBarSlash,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Race_Progress_Bar_Slash.png", 1)), E_FAIL);
//    /* For.Prototype_UIObject_Race_ProgressBar */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_ProgressBar,
//        UI_Race_ProgressBar::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_Component_UITexture_Race_ProgressLine */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_ProgressLine,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Race_Progress_Line_%d.png", 2)), E_FAIL);
//    /* For.Prototype_UIObject_Race_ProgressLine */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_ProgressLine,
//        UI_Race_ProgressLine::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_Component_UITexture_Race_Start_End */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_Start_End,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Race_Start_Goal_%d.png", 2)), E_FAIL);
//    /* For.Prototype_UIObject_Race_Start_End */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_Start_End,
//        UI_Race_Start_End::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    ///* For.Prototype_Component_UITexture_Race_Comment */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_Comment,
//        UI_Race_Commentary::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_Component_UITexture_Race_MyRank */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_MyRank,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/MyRank_%02d.png", 17)), E_FAIL);
//    /* For.Prototype_Component_UITexture_Race_Rank */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_Rank,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Rank_%02d.png", 18)), E_FAIL);
//    /* For.Prototype_UIObject_Race_Start_End */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_Rank,
//        UI_Race_Rank::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_Component_UITexture_Race_PlayerIcon */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_PlayerIcon,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Char_Icon.png")), E_FAIL);
//    /* For.Prototype_UIObject_Race_PlayerIcon */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_PlayerIcon,
//        UI_Race_PlayerIcon::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_Component_UITexture_Tracker */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_Tracker,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Tracker.png")), E_FAIL);
//    /* For.Prototype_Component_UITexture_Tracker_Effect */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_Tracker_Effect,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Tracker_Effect.png")), E_FAIL);
//    /* For.Prototype_UIObject_Race_Tracker */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_Tracker,
//        UI_Race_Tracker::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_Component_UITexture_Skill_Background */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_Skill_Background,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Skill_Frame_%d.png", 3)), E_FAIL);
//    /* For.Prototype_Component_UITexture_Skill_Icon */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture_Race_Skill_Icon,
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Skill_Icon_%d.png", 2)), E_FAIL);
//    /* For.Prototype_UIObject_Race_Skill */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_Skill,
//        UI_Race_SkillSlot::Create(m_pDevice, m_pContext, level)), E_FAIL);
//    /* For.Prototype_UIObject_Race_SkillManager */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject_Race_SkillManager,
//        UI_Race_SkillDisplay::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//    /* For.Prototype_UIObject_Masking */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject(L"Masking"),
//        WhiteMasking::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//
//    /* For.Prototype_Component_Texture_Cursor */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UITexture(L"Rank1"),
//        Texture::Create(m_pDevice, m_pContext, L"../Resources/UIs/Race/Rank1.png")), E_FAIL);
//    /* For.Prototype_UIObject_Race_Rank1 */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), Proto_UIObject(L"Rank1"),
//        UI_Race_Rank1::Create(m_pDevice, m_pContext, level)), E_FAIL);
//
//
//    COUT("레이스 사운드 로딩중");
//    CHECK_FAILED(m_pGameInstance->LoadSound("Skill", L"../Resources/Sounds/Race/Skill.mp3"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("Gate_Open", L"../Resources/Sounds/Race/Race_Gate_Open.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("Winbgm", L"../Resources/Sounds/Race/Winbgm.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("함성0", L"../Resources/Sounds/Race/sfx/함성0.mp3"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("함성1", L"../Resources/Sounds/Race/sfx/함성1.mp3"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("함성2", L"../Resources/Sounds/Race/sfx/함성2.mp3"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("발소리0", L"../Resources/Sounds/Race/sfx/발소리0.mp3"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("발소리1", L"../Resources/Sounds/Race/sfx/발소리1.mp3"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("발소리2", L"../Resources/Sounds/Race/sfx/발소리2.mp3"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("레이스기본", L"../Resources/Sounds/Race/레이스기본.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("레이스라스트스퍼트", L"../Resources/Sounds/Race/레이스라스트스퍼트.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("1번인기", L"../Resources/Sounds/Race/Anounce/1번인기.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("선두_01", L"../Resources/Sounds/Race/Anounce/선두_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("스타트_01", L"../Resources/Sounds/Race/Anounce/스타트_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("스타트_02", L"../Resources/Sounds/Race/Anounce/스타트_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("호쾌하게", L"../Resources/Sounds/Race/Anounce/호쾌하게.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("제3코너", L"../Resources/Sounds/Race/Anounce/제3코너.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("선두는", L"../Resources/Sounds/Race/Anounce/선두는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("빠르게치고", L"../Resources/Sounds/Race/Anounce/빠르게.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("선두로부터약간떨어진위치", L"../Resources/Sounds/Race/Anounce/선두로부터약간떨어진위치.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("각우마무스메", L"../Resources/Sounds/Race/Anounce/각우마무스메.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("중간을달리는", L"../Resources/Sounds/Race/Anounce/중간을달리는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("그녀의각질에는", L"../Resources/Sounds/Race/Anounce/그녀의각질에는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("순서를되짚어", L"../Resources/Sounds/Race/Anounce/순서를되짚어.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("그뒤에서", L"../Resources/Sounds/Race/Anounce/그뒤에서.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("거리를두고", L"../Resources/Sounds/Race/Anounce/거리를두고.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("나란히달리는", L"../Resources/Sounds/Race/Anounce/나란히달리는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("조금뒤에", L"../Resources/Sounds/Race/Anounce/조금뒤에.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("우승을걸고", L"../Resources/Sounds/Race/Anounce/우승을걸고.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("삿포로배", L"../Resources/Sounds/Race/Anounce/삿포로배.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("최선을다하고있으니", L"../Resources/Sounds/Race/Anounce/최선을다하고있으니.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("1번코너진입", L"../Resources/Sounds/Race/Anounce/1번코너진입.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("경기의절반", L"../Resources/Sounds/Race/Anounce/경기의절반.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("순위를되짚어", L"../Resources/Sounds/Race/Anounce/순위를되짚어.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("현재1순위는", L"../Resources/Sounds/Race/Anounce/현재1순위는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("약간뒤에서", L"../Resources/Sounds/Race/Anounce/약간뒤에서.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("그뒤를달리는", L"../Resources/Sounds/Race/Anounce/그뒤를달리는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("바깥에서", L"../Resources/Sounds/Race/Anounce/바깥에서.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("근처에서", L"../Resources/Sounds/Race/Anounce/근처에서.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("안쪽에서", L"../Resources/Sounds/Race/Anounce/안쪽에서.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("작전준비", L"../Resources/Sounds/Race/Anounce/작전준비.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("추월준비", L"../Resources/Sounds/Race/Anounce/추월준비.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("뒤따라붙는", L"../Resources/Sounds/Race/Anounce/뒤따라붙는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("맞춰달리는", L"../Resources/Sounds/Race/Anounce/맞춰달리는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("뒤이어달리는", L"../Resources/Sounds/Race/Anounce/뒤이어달리는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("가까이에서", L"../Resources/Sounds/Race/Anounce/가까이에서.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("진지한표정", L"../Resources/Sounds/Race/Anounce/진지한표정.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("다리에힘이", L"../Resources/Sounds/Race/Anounce/다리에힘이.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("다시한번진입", L"../Resources/Sounds/Race/Anounce/다시한번진입.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("경기후반부", L"../Resources/Sounds/Race/Anounce/경기후반부.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("치고나오는", L"../Resources/Sounds/Race/Anounce/치고나오는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("마지막까지", L"../Resources/Sounds/Race/Anounce/마지막까지.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("우승의영광", L"../Resources/Sounds/Race/Anounce/우승의영광.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("순위가바뀌기", L"../Resources/Sounds/Race/Anounce/순위가바뀌기.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("제4코너", L"../Resources/Sounds/Race/Anounce/제4코너.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("현재순위", L"../Resources/Sounds/Race/Anounce/현재순위.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("승부처", L"../Resources/Sounds/Race/Anounce/승부처.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("달려나갑니다", L"../Resources/Sounds/Race/Anounce/달려나갑니다.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("대망의1착", L"../Resources/Sounds/Race/Anounce/대망의1착.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("주저없이달리는", L"../Resources/Sounds/Race/Anounce/주저없이달리는.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("골인", L"../Resources/Sounds/Race/Anounce/골인.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("1번", L"../Resources/Sounds/Race/Character/1번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("2번", L"../Resources/Sounds/Race/Character/2번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("3번", L"../Resources/Sounds/Race/Character/3번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("4번", L"../Resources/Sounds/Race/Character/4번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("5번", L"../Resources/Sounds/Race/Character/5번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("6번", L"../Resources/Sounds/Race/Character/6번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("7번", L"../Resources/Sounds/Race/Character/7번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("8번", L"../Resources/Sounds/Race/Character/8번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("9번", L"../Resources/Sounds/Race/Character/9번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("10번", L"../Resources/Sounds/Race/Character/10번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("11번", L"../Resources/Sounds/Race/Character/11번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("12번", L"../Resources/Sounds/Race/Character/12번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("13번", L"../Resources/Sounds/Race/Character/13번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("14번", L"../Resources/Sounds/Race/Character/14번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("15번", L"../Resources/Sounds/Race/Character/15번.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("16번", L"../Resources/Sounds/Race/Character/16번.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("골드쉽_01_01", L"../Resources/Sounds/Race/Character/골드쉽_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("골드쉽_01_02", L"../Resources/Sounds/Race/Character/골드쉽_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("골드쉽_01_03", L"../Resources/Sounds/Race/Character/골드쉽_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("골드쉽_01_04", L"../Resources/Sounds/Race/Character/골드쉽_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("골드쉽_01_05", L"../Resources/Sounds/Race/Character/골드쉽_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("골드쉽_01_06", L"../Resources/Sounds/Race/Character/골드쉽_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("네오_01_01", L"../Resources/Sounds/Race/Character/네오_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("네오_01_02", L"../Resources/Sounds/Race/Character/네오_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("네오_01_03", L"../Resources/Sounds/Race/Character/네오_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("네오_01_04", L"../Resources/Sounds/Race/Character/네오_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("네오_01_05", L"../Resources/Sounds/Race/Character/네오_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("네오_01_06", L"../Resources/Sounds/Race/Character/네오_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("돈나_01_01", L"../Resources/Sounds/Race/Character/돈나_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("돈나_01_02", L"../Resources/Sounds/Race/Character/돈나_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("돈나_01_03", L"../Resources/Sounds/Race/Character/돈나_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("돈나_01_04", L"../Resources/Sounds/Race/Character/돈나_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("돈나_01_05", L"../Resources/Sounds/Race/Character/돈나_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("돈나_01_06", L"../Resources/Sounds/Race/Character/돈나_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("듀랜달_01_01", L"../Resources/Sounds/Race/Character/듀랜달_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("듀랜달_01_02", L"../Resources/Sounds/Race/Character/듀랜달_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("듀랜달_01_03", L"../Resources/Sounds/Race/Character/듀랜달_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("듀랜달_01_04", L"../Resources/Sounds/Race/Character/듀랜달_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("듀랜달_01_05", L"../Resources/Sounds/Race/Character/듀랜달_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("듀랜달_01_06", L"../Resources/Sounds/Race/Character/듀랜달_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("라샤_01_01", L"../Resources/Sounds/Race/Character/라샤_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("라샤_01_02", L"../Resources/Sounds/Race/Character/라샤_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("라샤_01_03", L"../Resources/Sounds/Race/Character/라샤_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("라샤_01_04", L"../Resources/Sounds/Race/Character/라샤_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("라샤_01_05", L"../Resources/Sounds/Race/Character/라샤_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("라샤_01_06", L"../Resources/Sounds/Race/Character/라샤_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("맥퀸_01_01", L"../Resources/Sounds/Race/Character/맥퀸_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("맥퀸_01_02", L"../Resources/Sounds/Race/Character/맥퀸_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("맥퀸_01_03", L"../Resources/Sounds/Race/Character/맥퀸_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("맥퀸_01_04", L"../Resources/Sounds/Race/Character/맥퀸_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("맥퀸_01_05", L"../Resources/Sounds/Race/Character/맥퀸_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("맥퀸_01_06", L"../Resources/Sounds/Race/Character/맥퀸_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("스즈카_01_01", L"../Resources/Sounds/Race/Character/스즈카_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("스즈카_01_02", L"../Resources/Sounds/Race/Character/스즈카_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("스즈카_01_03", L"../Resources/Sounds/Race/Character/스즈카_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("스즈카_01_04", L"../Resources/Sounds/Race/Character/스즈카_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("스즈카_01_05", L"../Resources/Sounds/Race/Character/스즈카_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("스즈카_01_06", L"../Resources/Sounds/Race/Character/스즈카_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("위크_01_01", L"../Resources/Sounds/Race/Character/스페셜위크_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("위크_01_02", L"../Resources/Sounds/Race/Character/스페셜위크_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("위크_01_03", L"../Resources/Sounds/Race/Character/스페셜위크_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("위크_01_04", L"../Resources/Sounds/Race/Character/스페셜위크_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("위크_01_05", L"../Resources/Sounds/Race/Character/스페셜위크_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("위크_01_06", L"../Resources/Sounds/Race/Character/스페셜위크_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("오구리_01_01", L"../Resources/Sounds/Race/Character/오구리_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("오구리_01_02", L"../Resources/Sounds/Race/Character/오구리_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("오구리_01_03", L"../Resources/Sounds/Race/Character/오구리_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("오구리_01_04", L"../Resources/Sounds/Race/Character/오구리_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("오구리_01_05", L"../Resources/Sounds/Race/Character/오구리_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("오구리_01_06", L"../Resources/Sounds/Race/Character/오구리_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("우라라_01_01", L"../Resources/Sounds/Race/Character/우라라_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("우라라_01_02", L"../Resources/Sounds/Race/Character/우라라_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("우라라_01_03", L"../Resources/Sounds/Race/Character/우라라_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("우라라_01_04", L"../Resources/Sounds/Race/Character/우라라_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("우라라_01_05", L"../Resources/Sounds/Race/Character/우라라_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("우라라_01_06", L"../Resources/Sounds/Race/Character/우라라_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("원더_01_01", L"../Resources/Sounds/Race/Character/원더_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("원더_01_02", L"../Resources/Sounds/Race/Character/원더_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("원더_01_03", L"../Resources/Sounds/Race/Character/원더_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("원더_01_04", L"../Resources/Sounds/Race/Character/원더_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("원더_01_05", L"../Resources/Sounds/Race/Character/원더_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("원더_01_06", L"../Resources/Sounds/Race/Character/원더_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("저니_01_01", L"../Resources/Sounds/Race/Character/저니_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("저니_01_02", L"../Resources/Sounds/Race/Character/저니_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("저니_01_03", L"../Resources/Sounds/Race/Character/저니_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("저니_01_04", L"../Resources/Sounds/Race/Character/저니_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("저니_01_05", L"../Resources/Sounds/Race/Character/저니_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("저니_01_06", L"../Resources/Sounds/Race/Character/저니_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("제퍼_01_01", L"../Resources/Sounds/Race/Character/제퍼_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("제퍼_01_02", L"../Resources/Sounds/Race/Character/제퍼_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("제퍼_01_03", L"../Resources/Sounds/Race/Character/제퍼_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("제퍼_01_04", L"../Resources/Sounds/Race/Character/제퍼_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("제퍼_01_05", L"../Resources/Sounds/Race/Character/제퍼_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("제퍼_01_06", L"../Resources/Sounds/Race/Character/제퍼_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("카렌_01_01", L"../Resources/Sounds/Race/Character/카렌_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("카렌_01_02", L"../Resources/Sounds/Race/Character/카렌_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("카렌_01_03", L"../Resources/Sounds/Race/Character/카렌_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("카렌_01_04", L"../Resources/Sounds/Race/Character/카렌_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("카렌_01_05", L"../Resources/Sounds/Race/Character/카렌_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("카렌_01_06", L"../Resources/Sounds/Race/Character/카렌_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("터보_01_01", L"../Resources/Sounds/Race/Character/터보_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("터보_01_02", L"../Resources/Sounds/Race/Character/터보_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("터보_01_03", L"../Resources/Sounds/Race/Character/터보_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("터보_01_04", L"../Resources/Sounds/Race/Character/터보_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("터보_01_05", L"../Resources/Sounds/Race/Character/터보_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("터보_01_06", L"../Resources/Sounds/Race/Character/터보_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("테이오_01_01", L"../Resources/Sounds/Race/Character/테이오_01_01.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("테이오_01_02", L"../Resources/Sounds/Race/Character/테이오_01_02.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("테이오_01_03", L"../Resources/Sounds/Race/Character/테이오_01_03.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("테이오_01_04", L"../Resources/Sounds/Race/Character/테이오_01_04.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("테이오_01_05", L"../Resources/Sounds/Race/Character/테이오_01_05.wav"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("테이오_01_06", L"../Resources/Sounds/Race/Character/테이오_01_06.wav"), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->LoadSound("Wonder", L"../Resources/Sounds/Race/Wonder.mp3"), E_FAIL);
//    CHECK_FAILED(m_pGameInstance->LoadSound("1등", L"../Resources/Sounds/Race/sfx/1등.wav"), E_FAIL);
//    COUT("레이스 로딩 완료");
//
//
//
//    return S_OK;
//}
/******************************************************* 레이스 프로토타입 *******************************************************/



//////////////////////////////////////////////////////// 스테이지별 공통 ////////////////////////////////////////////////////////
//HRESULT Client::Loader::Character_Models(LEVEL _eLevelName)
//{
//    _matrix prematrix = XMMatrixIdentity();
//    prematrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180));
//
//    /* For.Prototype_Component_Model_Special_Week */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Special_Week,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Special_Week/Special_Week.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Tokai_Teio */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Tokai_Teio,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Tokai_Teio/Tokai_Teio.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Silence_Suzuka */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Silence_Suzuka,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Silence_Suzuka/Silence_Suzuka.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Haru_Urara */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Haru_Urara,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Haru_Urara/Haru_Urara.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Twin_Turbo */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Twin_Turbo,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Twin_Turbo/Twin_Turbo.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Mejiro_McQueen */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Mejiro_McQueen,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Mejiro_McQueen/Mejiro_McQueen.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Oguri_Cap */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Oguri_Cap,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Oguri_Cap/Oguri_Cap.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Gold_Ship */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Gold_Ship,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Gold_Ship/Gold_Ship.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Rice_Shower */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Rice_Shower,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Rice_Shower/Rice_Shower.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Gentildonna */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Gentildonna,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Gentildonna/Gentildonna.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Durandal */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Durandal,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Durandal/Durandal.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Neo_Universe */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Neo_Universe,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Neo_Universe/Neo_Universe.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Curren_Chan */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Curren_Chan,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Curren_Chan/Curren_Chan.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Yamanin_Zephyr */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Yamanin_Zephyr,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Yamanin_Zephyr/Yamanin_Zephyr.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Dream_Journey */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Dream_Journey,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Dream_Journey/Dream_Journey.uma", prematrix)), E_FAIL);
//
//    /* For.Prototype_Component_Model_Wonder_Acute */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_Model_Wonder_Acute,
//        Model::Create(m_pDevice, m_pContext, MODEL::ANIM, L"../Resources/Models/Character/Wonder_Acute/Wonder_Acute.uma", prematrix)), E_FAIL);
//
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_Com_BlobShadow,
//        BlobShadow::Create(m_pDevice, m_pContext)), E_FAIL);
//
//    return S_OK;
//}
//
//HRESULT Client::Loader::Character_Objects(LEVEL _eLevelName)
//{
//    /* For.Prototype_GameObject_Special_Week */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Special_Week,
//        Special_Week::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Tokai_Teio */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Tokai_Teio,
//        Tokai_Teio::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Silence_Suzuka */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Silence_Suzuka,
//        Silence_Suzuka::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Haru_Urara */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Haru_Urara,
//        Haru_Urara::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Twin_Turbo */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Twin_Turbo,
//        Twin_Turbo::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Mejiro_McQueen */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Mejiro_McQueen,
//        Mejiro_McQueen::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Oguri_Cap */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Oguri_Cap,
//        Oguri_Cap::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Gold_Ship */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Gold_Ship,
//        Gold_Ship::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Rice_Shower */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Rice_Shower,
//        Rice_Shower::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Gentildonna */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Gentildonna,
//        Gentildonna::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Durandal */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Durandal,
//        Durandal::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Neo_Universe */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Neo_Universe,
//        Neo_Universe::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Curren_Chan */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Curren_Chan,
//        Curren_Chan::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Yamanin_Zephyr */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Yamanin_Zephyr,
//        Yamanin_Zephyr::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Dream_Journey */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Dream_Journey,
//        Dream_Journey::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    /* For.Prototype_GameObject_Wonder_Acute */
//    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(_eLevelName), Proto_GameObject_Wonder_Acute,
//        Wonder_Acute::Create(m_pDevice, m_pContext, _eLevelName)), E_FAIL);
//
//    return S_OK;
//}

void ShaderTool::Loader::Dumping_Time()
{
    Sleep(3000);
}
/******************************************************* 스테이지별 공통 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Loader* ShaderTool::Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _eLevelName)
{
    Loader* pInstance = new Loader(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_eLevelName), L"Loader 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::Loader::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pGameInstance);

    CoUninitialize();
}
/******************************************************* 객체 반환 함수 *******************************************************/
