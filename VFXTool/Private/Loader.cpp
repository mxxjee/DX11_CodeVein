#include "VFXTool_Define.h"
#include "Loader.h"
#include "GameInstance.h"


//여기서 패스를 만든다
#pragma region GameObject
#include "Player.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_MasterRig.h"

#pragma endregion
#include "Terrain.h"
#pragma endregion


#pragma region Camera
#include "Camera_Free.h"

#pragma endregion

#pragma region Effect
#include "ParticleSystem.h"
#include "BasicParticle.h"
#include "SwordTrail.h"
#include "Decal_Blood.h"
#include "BasicMesh.h"
#pragma endregion


_bool Loader::m_bStaticComplete = false;
_bool Loader::m_bLevelCompleteStatic[_UINT(LEVEL::END)] = { false };

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
VFXTool::Loader::Loader()
{
}

VFXTool::Loader::Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

VFXTool::Loader::~Loader()
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
HRESULT VFXTool::Loader::Initialize(LEVEL _eLevelName)
{
    m_eCreateLevel = _eLevelName;

    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);

    m_pGameInstance->Clear_UIManager();

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



HRESULT VFXTool::Loader::Loading()
{
    InitializeCriticalSection(&m_CriticalSection);

    HRESULT hr = CoInitializeEx(nullptr, 0);

    switch (m_eCreateLevel)
    {
    case LEVEL::STATIC:
        if (!m_bStaticComplete)
            hr = Load_Static();
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
HRESULT VFXTool::Loader::Load_Static()
{
    LEVEL level = LEVEL::STATIC;

    COUT("전역 컴포넌트 로딩중");

    /* For.Prototype_Component_VIBuffer_Rect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_VIBuffer_Rect",
        VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

   /* For.Prototype_Component_VICube */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_Component_VIBuffer_Cube",
        VIBuffer_Cube::Create(m_pDevice, m_pContext)), E_FAIL);


    COUT("전역 텍스쳐 로딩중");


    COUT("전역 쉐이더 로딩중");
    SHADERENTRY entry[1] = { "VS_MAIN", "PS_MAIN" };
    SHADERENTRIES entries;
    entries.pEntries = entry;
    entries.iNumpass = 1;
    /* For.Prototype_Component_Shader_VTXCube */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_VTXCube",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxCube.hlsl", VTXCUBE::Elements, VTXCUBE::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryAnim[2] = {
        {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
        {"VS_MAIN", "PS_MAIN_SHADOW"}   // Pass 1 : 그림자 그리기
    };
    entries.pEntries = entryAnim;
    entries.iNumpass = 2;

    /* For.Prototype_Component_Shader_VTXNorTex */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_VTXNorTex",
        Shader::Create(m_pDevice, m_pContext, L"../Shader/Shader_VtxNorTex.hlsl", VTXNORTEX::Elements, VTXNORTEX::iNumElements, entries)), E_FAIL);

    /* For.Prototype_Component_Shader_Compute_Particle */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Particle",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Compute_Particle.hlsl", "CS_SPREAD")), E_FAIL);

    /* For.Prototype_Component_Shader_Compute_Particle_Converge */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Particle_Converge",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Compute_Particle.hlsl", "CS_CONVERGE")), E_FAIL);

    /* For.Prototype_Component_Shader_Compute_Particle_Dust */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Particle_Dust",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Compute_Particle.hlsl", "CS_DUST")), E_FAIL);

    SHADERENTRY entryTrail[4] = {
        {"VS_MAIN", "PS_MAIN"},                 // Pass 0 : 텍스처 + 컬러 틴트 + 알파 페이드
        {"VS_MAIN", "PS_DISTORTION"},           // Pass 1 : 텍스처 + 왜곡 + 컬러 틴트 + 알파 페이드
        {"VS_MAIN", "PS_SCENE_DISTORTION"},     // Pass 2 : 장면 왜곡
        {"VS_MAIN", "PS_SOLID"}                 // Pass 3 : 솔리드 컬러 (텍스처 없이, 디버그)
    };
    entries.pEntries = entryTrail;
    entries.iNumpass = 4;

    /* For.Prototype_Component_Shader_Trail */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Trail",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Trail.hlsl", VTXTRAIL::Elements, VTXTRAIL::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryDecal[1] = {
        {"VS_MAIN", "PS_MAIN"}
    };
    entries.pEntries = entryDecal;
    entries.iNumpass = 1;

    /* For.Prototype_Component_Shader_Decal */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Decal",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Decal.hlsl", VTXTRAIL::Elements, VTXTRAIL::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryParticleEffect[3] = {
        {"VS_MAIN", "PS_MAIN"},
        {"VS_MAIN", "PS_SPRITE"},
        {"VS_MAIN", "PS_SCENE_DISTORTION"}
    };
    entries.pEntries = entryParticleEffect;
    entries.iNumpass = 3;

    /* For.Prototype_Component_Shader_Particle_Rect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Particle_Rect",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex_Particle.hlsl", VTXPOSTEX_PARTICLE::Elements, VTXPOSTEX_PARTICLE::iNumElements, entries)), E_FAIL);

    SHADERENTRY entryMeshEffect[4] = {
    {"VS_MAIN", "PS_MAIN"},
    {"VS_MAIN", "PS_SPRITE"},
    {"VS_MAIN", "PS_SCENE_DISTORTION"},
    {"VS_FIRE", "PS_FIRE"}
    };
    entries.pEntries = entryMeshEffect;
    entries.iNumpass = 4;

    /* For.Prototype_Component_Shader_MeshEffect */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_MeshEffect",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_MeshEffect.hlsl", VTXMESH::Elements, VTXMESH::iNumElements, entries)), E_FAIL);


#pragma region PLAYER
    SHADERENTRY entryPlayerAnim[2] = {
        {"VS_MAIN", "PS_MAIN"},         // Pass 0 : 일반 그리기
        {"VS_MAIN", "PS_MAIN_SHADOW"}   // Pass 1 : 그림자 그리기
    };
    entries.pEntries = entryPlayerAnim;
    entries.iNumpass = 2;

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh",
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_PlayerAnimMesh.hlsl", VTXPLAYERANIMMESH::Elements, VTXPLAYERANIMMESH::iNumElements, entries)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Bone",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_BONECOMBINED")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_LocalMatrix",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_EVALUATELOCAL")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_Readback",
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_READBACK")), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_Compute_DrivenBones", //런타임에 SRT를 원하는 뼈에 적용
        ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl", "CS_APPLYDRIVENBONES")), E_FAIL);
#pragma endregion


    COUT("전역 로딩 완료");

    

    m_bIsComplete = true;
    m_bStaticComplete = true;

    return S_OK;
}
/******************************************************* 전역 프로토타입 *******************************************************/


//////////////////////////////////////////////////////// 메인 프로토타입 ////////////////////////////////////////////////////////
HRESULT VFXTool::Loader::Load_Main()
{
    _uint level = _UINT(LEVEL::MAIN);

    COUT("카메라 로딩중");
    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_GameObject_Camera_Free", Camera_Free::Create(m_pDevice, m_pContext, LEVEL::MAIN)), E_FAIL);

    COUT("로드 완료");

    COUT("텍스쳐 로딩중");
    // For Prototype_Component_Texture_Effects
    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Prototype_Component_Texture_Effects"),
        NewTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/")))))
        return E_FAIL;

    // For Prototype_Component_Texture_Terrain
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_Component_Texture_Terrain"),
        Texture::Create(m_pDevice, m_pContext, L"../../Resources/Effects/grid_64.bmp", 1)), E_FAIL);

    // For Prototype_Component_Texture_Decal
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_Component_Texture_Decal"),
        Texture::Create(m_pDevice, m_pContext, L"../../Resources/Effects/T_FX_BloodEffect_Color.png", 1)), E_FAIL);

    // For Prototype_Component_Texture_DecalMask
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), TEXT("Prototype_Component_Texture_Decal_Mask"),
        Texture::Create(m_pDevice, m_pContext, L"../../Resources/Effects/T_FX_Blood_Floor_01.png", 1)), E_FAIL);

    COUT("로드 완료")

    COUT("버퍼 로딩 중")
    /* For.Prototype_Component_VIBuffer_Particle_Basic */
    VIBuffer_Particle_Rect::PARTICLE_RECT_DESC	BasicDesc{};
    BasicDesc.IsLoop = true;
    BasicDesc.IsBillboard = true;
    BasicDesc.iNumInstance = 1;
    BasicDesc.vCenter = _float3(0.f, 0.f, 0.f);
    BasicDesc.vSize = _float2(1.f, 1.f);
    BasicDesc.vRange = _float3(0.f, 0.f, 0.f);
    BasicDesc.vSpeed = _float2(0.f, 0.f);
    BasicDesc.vRotation = _float2(0.f, 0.f);
    BasicDesc.vLifeTime = _float2(1.f, 1.f);
    BasicDesc.vPivot = _float3(0.f, -0.5f, 0.f);
    BasicDesc.fGravity = 0.f;
    BasicDesc.fAlpha = 1.f;
    if (FAILED(m_pGameInstance->Add_Prototype(level, TEXT("Prototype_Component_VIBuffer_Particle_Basic"),
        VIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, &BasicDesc))))
        return E_FAIL;

    /* For.Prototype_Component_VIBuffer_Trail */
    VIBuffer_Trail::TRAIL_DESC TrailDesc{};
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_Component_VIBuffer_Trail",
        VIBuffer_Trail::Create(m_pDevice, m_pContext, &TrailDesc)), E_FAIL);

    /* For.Prototype_Component_VIBuffer_Terrain */
    if (FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_Component_VIBuffer_Terrain",
        VIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/grid.bmp")))))
        return E_FAIL;

    COUT("오브젝트 로딩중");
    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_GameObject_Terrain", Terrain::Create(m_pDevice, m_pContext, LEVEL::MAIN)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_GameObject_BasicParticle", BasicParticle::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_GameObject_ParticleSystem", ParticleSystem::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_GameObject_TrailEffect", SwordTrail::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_GameObject_DecalBlood", Decal_Blood::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(level, L"Prototype_GameObject_BasicMesh", BasicMesh::Create(m_pDevice, m_pContext)), E_FAIL);





    _matrix prematrix = XMMatrixIdentity();

    prematrix = XMMatrixIdentity();
    prematrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

    _wstring folderPath = L"../../Resources/Model/Effect/";
    m_pGameInstance->Load_Folder(folderPath, _UINT(level), prematrix, L".siho");

    prematrix *= XMMatrixRotationX(XMConvertToRadians(-90.f));
    
    /* For.Prototype_Component_Model_PlayerMasterBone */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::MAIN), L"Prototype_Component_Model_PlayerMasterBone",
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/MasterBone.siho", prematrix, MODELROLE::MASTER)), E_FAIL);

    /* For.Prototype_Component_Model_PlayerBody_Base */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::MAIN), L"Prototype_Component_Model_PlayerBody_Base",
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Body/Body_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

    /* For.Prototype_Component_Model_PlayerHair_Base */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::MAIN), L"Prototype_Component_Model_PlayerHair_Base",
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Hair/Hair_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

    /* For.Prototype_Component_Model_PlayerHead_Base */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::MAIN), L"Prototype_Component_Model_PlayerHead_Base",
        Model::Create(m_pDevice, m_pContext, MODEL::PLAYERANIM, L"../../Resources/Models/Player/Face/Face_Female1.siho", prematrix, MODELROLE::PART)), E_FAIL);

    m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Player",
        Player::Create(m_pDevice, m_pContext, LEVEL::MAIN));

    m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Player_MasterRig",
        Player_MasterRig::Create(m_pDevice, m_pContext, LEVEL::MAIN));

    m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Player_Body",
        Player_Body::Create(m_pDevice, m_pContext, LEVEL::MAIN));

    m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Player_Hair",
        Player_Hair::Create(m_pDevice, m_pContext, LEVEL::MAIN));

    m_pGameInstance->Add_Prototype(_UINT(level), L"Prototype_GameObject_Player_Head",
        Player_Head::Create(m_pDevice, m_pContext, LEVEL::MAIN));



    m_bIsComplete = true;
    COUT("로드 완료");

    return S_OK;
}
/******************************************************* 메인 프로토타입 *******************************************************/



//////////////////////////////////////////////////////// 스테이지별 공통 ////////////////////////////////////////////////////////
void VFXTool::Loader::Dumping_Time()
{
    Sleep(3000);
}
/******************************************************* 스테이지별 공통 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Loader* VFXTool::Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _eLevelName)
{
    Loader* pInstance = new Loader(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_eLevelName), L"Loader 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void VFXTool::Loader::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    Safe_Release(m_pGameInstance);

    CoUninitialize();
}
/******************************************************* 객체 반환 함수 *******************************************************/
