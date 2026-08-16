// Player_Hair.cpp

#include "Client_Define.h"
#include "Player_Hair.h"
#include "Model.h"
#include "Shader.h"
#include "Bone.h"
#include "Player_MasterRig.h"
#include "Mesh.h"

Client::Player_Hair::Player_Hair()
{
}

Client::Player_Hair::Player_Hair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :PartObject(pDevice, pContext)
{
}

Client::Player_Hair::Player_Hair(const Player_Hair& original)
    :PartObject(original)
{
}

Client::Player_Hair::~Player_Hair()
{
}

HRESULT Client::Player_Hair::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);
    return S_OK;
}

HRESULT Client::Player_Hair::Initialize(void* arg)
{
    static _uint namenum = 0;
    HAIR_DESC desc;

    if (arg == nullptr)
        arg = &desc;


	CAST(HAIR_DESC*)(arg)->wstrName = L"Player_Hair" + to_wstring(namenum++);
	m_pDissolveTime = CAST(HAIR_DESC*)(arg)->pDissolveTime;

    CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);
    CHECK_FAILED(Ready_Components(), E_FAIL);

    Model* pMasterModelCom;
    for (int i = 0; i < 5; i++)
    {
        CHECK_FAILED(__super::RemapBonesVec(m_pModelComVec[i], i), E_FAIL);

        int failCount = 0;
        for (size_t j = 0; j < m_BoneRemaps[i].size(); ++j)
        {
            if (m_BoneRemaps[i][j] == g_INVALID)
                ++failCount;
        }

        pMasterModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
        pMasterModelCom->RegisterPartModelCom(m_pModelComVec[i]);
        m_pModelComVec[i]->Ready_ReampSRV(pMasterModelCom, m_BoneRemaps[i]);
    }

    CHECK_FAILED(Ready_ShaderValue(), E_FAIL);

    // MasterModel 캐싱 + 헤어 본 체인 초기화
    CHECK_FAILED(Ready_MasterModel(), E_FAIL);

    Ready_HairBoneChain();

    m_pFocus = static_cast<Player*>(m_pGameInstance->Get_Player())->Get_FocuseStatePtr();

    return S_OK;
}

HRESULT Client::Player_Hair::Ready_MasterModel()
{
    if (m_pMasterModelCom == nullptr)
    {
        m_pMasterModelCom = CAST(Model*)(m_pMasterRig->Get_Component_FromName(Com_Model));
        if (m_pMasterModelCom == nullptr) return E_FAIL;
    }
    if (m_pPlayer == nullptr)
    {
        m_pPlayer = CAST(Player*)(m_pGameInstance->Get_Player());
        if (m_pPlayer == nullptr) return E_FAIL;
    }

    return S_OK;
}

_int Client::Player_Hair::Update_Priority(const _float fTimeDelta)
{
    return 0;
}

_int Client::Player_Hair::Update(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));
    m_iRimNoiseTime += fTimeDelta;
	return 0;
}

_int Client::Player_Hair::Update_Late(const _float fTimeDelta)
{
    GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND);
    GameObject::Add_RenderGroup(RENDER_GROUP::SHADOW);

    // PhysX 헤어 물리 (체인이 있을 때만)
    BoneChain_UpdateAll(m_pPlayer->Get_WorldFloat4x4());

    return 0;
}

HRESULT Client::Player_Hair::Render(const _float fTimeDelta)
{
    MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

    Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	// Noise Texture
	_uint bDissolving = (m_pDissolveTime && *m_pDissolveTime > 0.f);
	_uint bUseNoise = bDissolving ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		_float fTime = *m_pDissolveTime;
		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fTime, sizeof(_float));
	}

    //Rim NoiseTexture
    if (*m_pFocus)
    {
        CHECK_FAILED(m_pRimNoiseTexture->Bind_ShaderResource_FullSlot(m_pShaderCom, 18, 0), E_FAIL);
        m_pShaderCom->Bind_RawValue_ByHandle(g_fTime, &m_iRimNoiseTime, sizeof(_float));
    }
   
    CHECK_FAILED(m_pShaderCom->Begin(4), E_FAIL);

    for (_uint i = 0; i < m_pModelComVec[m_iHairMeshNum]->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS), E_FAIL);

        CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);
        CHECK_FAILED(m_pShaderCom->Bind_Resources(4), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Render(fTimeDelta, i), E_FAIL);
    }

    return S_OK;
}

HRESULT Client::Player_Hair::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);

    if (FAILED(m_pShaderCom->Begin(1, true)))
        return E_FAIL;

    Model* pMasterRigModelCom = dynamic_cast<Model*>(m_pMasterRig->Get_Component_FromName(Com_Model));
    _uint iNumMeshes = m_pModelComVec[m_iHairMeshNum]->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        CHECK_FAILED(pMasterRigModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);
        CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);
        CHECK_FAILED(m_pModelComVec[m_iHairMeshNum]->Render(fTimeDelta, i), E_FAIL);
    }
    return S_OK;
}

HRESULT Client::Player_Hair::Ready_ShaderValue()
{
    m_tPlayerHairShader.fPlayerHighlight1Shift = -1.17f;
    m_tPlayerHairShader.fPlayerHighlight1Strength = 0.03f;
    m_tPlayerHairShader.fPlayerHighlight2Shift = -0.82f;
    m_tPlayerHairShader.fPlayerHighlight2Strength = 0.5f;
    m_tPlayerHairShader.fPlayerHighlightPower = 40.0f;
    m_tPlayerHairShader.fPlayerMetallic = 0.1f;
    m_tPlayerHairShader.fPlayerRoughnessMin = 0.5f;
    m_tPlayerHairShader.vPlayerHairColor = { 0.376f, 0.282f, 0.173f, 1.0f };
    m_iHairMeshNum = 0;
    return S_OK;
}

HRESULT Client::Player_Hair::Ready_Components()
{
    m_pModelComVec.resize(10);
    m_BoneRemaps.resize(10);
    CHECK_FAILED(Add_Component(_uint(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);

    CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base"), Com_Model, RCAST(Component**)(&m_pModelComVec[0])), E_FAIL);
    CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base1"), L"Model_Component1", RCAST(Component**)(&m_pModelComVec[1])), E_FAIL);
    CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base2"), L"Model_Component2", RCAST(Component**)(&m_pModelComVec[2])), E_FAIL);
    CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base3"), L"Model_Component3", RCAST(Component**)(&m_pModelComVec[3])), E_FAIL);
    CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerHair_Base4"), L"Model_Component4", RCAST(Component**)(&m_pModelComVec[4])), E_FAIL);
    CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"), Com_Texture, RCAST(Component**)(&m_pRimNoiseTexture)), E_FAIL);

	// Dissolve용 텍스처
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve_Player"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

    return S_OK;
}

HRESULT Client::Player_Hair::Bind_ShaderResources()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);
    //CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);
    return S_OK;
}

//////////////////////////////////////////////////////// PhysX 헤어 물리 ////////////////////////////////////////////////////////
HRESULT Client::Player_Hair::Ready_HairBoneChain()
{
    //BONE_CHAIN_BUILD_DESC tBuildDesc = {};
    //tBuildDesc.vecChainDefines = {
    //    { "Head", {
    //        { "Hair_CenterFront",  -1 },
    //        { "Hair_CenterFront1",  0 },
    //    }},
    //    { "Head", {
    //        { "Hair_CenterTail",  -1 },
    //        { "Hair_CenterTail1",  0 },
    //        { "Hair_CenterTail2",  1 },
    //        { "Hair_CenterTail3",  2 },
    //        { "Hair_CenterTail4",  3 },
    //        { "Hair_CenterTail5",  4 },
    //        { "Hair_CenterTail6",  5 },
    //        { "Hair_CenterTail7",  6 },
    //    }},
    //    { "Head", {
    //        { "Hair_CenterBack",  -1 },
    //        { "Hair_CenterBack1",  0 },
    //        { "Hair_CenterBack2",  1 },
    //        { "Hair_CenterBack3",  2 },
    //        { "Hair_CenterBack4",  3 },
    //        { "Hair_CenterBack5",  4 },
    //        { "Hair_CenterBack6",  5 },
    //        { "Hair_CenterBack7",  6 },
    //    }},
    //    { "Head", {
    //        { "Hair_LeftFront",  -1 },
    //        { "Hair_LeftFront1",  0 },
    //    }},
    //    { "Head", {
    //        { "Hair_LeftSide",      -1 },
    //        { "Hair_LeftSide1",      0 },
    //        { "Hair_LeftSide2",      1 },
    //        //{ "Hair_LeftSide3",      2 },
    //        //{ "Hair_LeftSide4",      3 },
    //        //{ "Hair_LeftSide5",      4 },
    //        //{ "Hair_LeftSide6",      5 },
    //    }},
    //    { "Head", {
    //        { "Hair_LeftTail",  -1 },
    //        { "Hair_LeftTail1",  0 },
    //        { "Hair_LeftTail2",  1 },
    //        { "Hair_LeftTail3",  2 },
    //        { "Hair_LeftTail4",  3 },
    //    }},
    //    { "Head", {
    //        { "Hair_LeftSideTail",  -1 },
    //        { "Hair_LeftSideTail1",  0 },
    //        { "Hair_LeftSideTail2",  1 },
    //        { "Hair_LeftSideTail3",  2 },
    //        { "Hair_LeftSideTail4",  3 },
    //    }},
    //    { "Head", {
    //        { "Hair_LeftBack",      -1 },
    //        { "Hair_LeftBack1",      0 },
    //        { "Hair_LeftBack2",      1 },
    //        { "Hair_LeftBack3",      2 },
    //        { "Hair_LeftBack4",      3 },
    //        { "Hair_LeftBack5",      4 },
    //        { "Hair_LeftBack6",      5 },
    //        { "Hair_LeftBack7",      6 },
    //    }},
    //    { "Head", {
    //        { "Hair_RightFront",  -1 },
    //        { "Hair_RightFront1",  0 },
    //    }},
    //    { "Head", {
    //        { "Hair_RightSide",      -1 },
    //        { "Hair_RightSide1",      0 },
    //        { "Hair_RightSide2",      1 },
    //        //{ "Hair_RightSide3",      2 },
    //        //{ "Hair_RightSide4",      3 },
    //        //{ "Hair_RightSide5",      4 },
    //        //{ "Hair_RightSide6",      5 },
    //    }},
    //    { "Head", {
    //        { "Hair_RightTail",  -1 },
    //        { "Hair_RightTail1",  0 },
    //        { "Hair_RightTail2",  1 },
    //        { "Hair_RightTail3",  2 },
    //        { "Hair_RightTail4",  3 },
    //    }},
    //    { "Head", {
    //        { "Hair_RightSideTail",  -1 },
    //        { "Hair_RightSideTail1",  0 },
    //        { "Hair_RightSideTail2",  1 },
    //        { "Hair_RightSideTail3",  2 },
    //        { "Hair_RightSideTail4",  3 },
    //    }},
    //    { "Head", {
    //        { "Hair_RightBack",      -1 },
    //        { "Hair_RightBack1",      0 },
    //        { "Hair_RightBack2",      1 },
    //        { "Hair_RightBack3",      2 },
    //        { "Hair_RightBack4",      3 },
    //        { "Hair_RightBack5",      4 },
    //        { "Hair_RightBack6",      5 },
    //        { "Hair_RightBack7",      6 },
    //    }},
    //    { "Head", {
    //        { "Hair_Back",      -1 },
    //        { "Hair_Back1",      0 },
    //        { "Hair_Back2",      1 },
    //        { "Hair_Back3",      2 },
    //        { "Hair_Back4",      3 },
    //    }},
    //};

    BONE_CHAIN_BUILD_DESC tBuildDesc = {};

    BoneChain_AutoBuildDefines(
        m_pModelComVec[m_iHairMeshNum],
        "Head",
        tBuildDesc.vecChainDefines
    );

    tBuildDesc.pBoneSourceModel = m_pModelComVec[m_iHairMeshNum];
    tBuildDesc.bUseRestPosePosition = true;
    tBuildDesc.iChainID_Offset = 100;
    tBuildDesc.tDefaultPhysXDesc.fBoneRadius = 0.01f;
    tBuildDesc.tDefaultPhysXDesc.fSwingLimit = 120.f;    // 부모 본 기준으로 좌우/앞뒤로 흔들릴 수 있는 최대 각도(도 단위)
    tBuildDesc.tDefaultPhysXDesc.fTwistLimit = 120.f;    // 본 자체 축(길이 방향) 기준 비틀림 허용 각도
    tBuildDesc.tDefaultPhysXDesc.fDamping = 0.08f;       // 운동 에너지를 얼마나 빨리 감쇠시킬지
    tBuildDesc.tDefaultPhysXDesc.fStiffness = 10.f;     // 레스트 포즈로 복귀하려는 힘의 세기
    tBuildDesc.tDefaultPhysXDesc.fDensity = 0.005f;     // 본 충돌체의 밀도

    BoneChain_Build(tBuildDesc, m_pMasterModelCom);

    COUT("플레이어 머리 바뀜");

    return S_OK;
}

void Client::Player_Hair::Switch_HairMesh(_int _newIndex)
{
    if (_newIndex < 0 || _newIndex >= (_int)m_pModelComVec.size())
        return;

    if (_newIndex == m_iHairMeshNum)
        return;

    // 기존 PhysX 체인 제거
    BoneChain_Cleanup();

    // 모델 교체
    m_iHairMeshNum = _newIndex;

    // 새 모델로 체인 재생성
    Ready_HairBoneChain();
}
/******************************************************* PhysX 헤어 물리 *******************************************************/

Client::Player_Hair* Player_Hair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    Player_Hair* pInstance = new Player_Hair(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_Hair 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::Player_Hair::Clone(void* pArg)
{
    Player_Hair* pInstance = new Player_Hair(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"Player_Hair 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::Player_Hair::Free()
{
    //if(m_bIsClone)
    {
        BoneChain_Cleanup();
    }

    __super::Free();
}