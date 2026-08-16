#include "Client_Define.h"
#include "Player_Outer.h"
#include "Model.h"
#include "Shader.h"
#include "Bone.h"
#include "Player_MasterRig.h"

Player_Outer::Player_Outer()
{
}

Player_Outer::Player_Outer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice, pContext)
{
}

Player_Outer::Player_Outer(const Player_Outer& original)
	:PartObject(original)
{
}

Player_Outer::~Player_Outer()
{
}

HRESULT Player_Outer::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Player_Outer::Initialize(void* arg)
{
	static _uint namenum = 0;

	OUTER_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(OUTER_DESC*)(arg)->wstrName = L"Player_Outer" + to_wstring(namenum++);
	m_pDissolveTime = CAST(OUTER_DESC*)(arg)->pDissolveTime;

	CHECK_FAILED(PartObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	CHECK_FAILED(__super::RemapBones(m_pModelCom), E_FAIL);

	int failCount = 0;
	for (size_t i = 0; i < m_BoneRemap.size(); ++i)
	{
		if (m_BoneRemap[i] == g_INVALID)
			++failCount;
	}
	COUT("Body Remap Fail Count : " << failCount << " / " << m_BoneRemap.size());

	CHECK_FAILED(Ready_MasterModel(), E_FAIL);

	m_pMasterModelCom->RegisterPartModelCom(m_pModelCom);

	m_pModelCom->Ready_ReampSRV(m_pMasterModelCom, m_BoneRemap);

	CHECK_FAILED(Ready_ClothBoneChain(), E_FAIL);

	Set_AllPass_VecObjPass(8);

	m_pFocus = static_cast<Player*>(m_pGameInstance->Get_Player())->Get_FocuseStatePtr();

	return S_OK;
}

HRESULT Player_Outer::Ready_Components()
{
	CHECK_FAILED(Add_Component(_uint(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPlayerAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Model(L"PlayerOuter_Drape"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

	// Dissolve용 텍스처
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve_Player"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	Set_VecObjPassSize(m_pModelCom->Get_NumMeshes());

	return S_OK;
}

HRESULT Player_Outer::Ready_MasterModel()
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

_int Player_Outer::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int Player_Outer::Update(const _float fTimeDelta)
{
	__super::Setup_CombinedWorldMatrix(XMLoadFloat4x4(m_matParentMatrix));
	return 0;
}

_int Player_Outer::Update_Late(const _float fTimeDelta)
{
	GameObject::Add_RenderGroup(RENDER_GROUP::NONBLEND);
	GameObject::Add_RenderGroup(RENDER_GROUP::SHADOW);

	BoneChain_UpdateAll_Before(m_pPlayer->Get_WorldFloat4x4());

	return 0;
}

HRESULT Player_Outer::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

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

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		/* 쉐이더의 0번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		//바디도 메쉬별로 바인딩 분류 해줘야하긴함.
		_uint bitflag = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL); //Diffuse
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitflag), E_FAIL); //Diffuse
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL); //Diffuse
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitflag), E_FAIL); //Diffuse
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

		CHECK_FAILED(m_pMasterModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		/* 0번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Player_Outer::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);

	if (FAILED(m_pShaderCom->Begin(1, true)))
		return E_FAIL;

	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		CHECK_FAILED(m_pMasterModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT Player_Outer::Bind_ShaderResources()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_matCombinedMatrix), E_FAIL);

	return S_OK;
}

//////////////////////////////////////////////////////// PhysX Cloth 물리 ////////////////////////////////////////////////////////
HRESULT Player_Outer::Ready_ClothBoneChain()
{
	BONE_CHAIN_BUILD_DESC tBuildDesc = {};
	tBuildDesc.vecChainDefines = {
		// 오른쪽 체인
		{ "Spine3", {
			{ "Temp_GCRightDrape",   -1 },  // [0] Kinematic 루트
			{ "Temp_GCRightDrape1",   0 },  // [1] dynamic
			{ "Temp_GCRightDrape2",   1 },  // [2] dynamic
			{ "Temp_GCRightDrape3",   2 },  // [3] dynamic
			{ "Temp_GCRightDrape4",   3 },  // [4] dynamic
			{ "Temp_GCRightDrape5",   4 },  // [5] dynamic
			{ "RightOuterOpen",       0 },  // [6] dynamic (분기)
			{ "RightOuterOpen1",      6 },  // [7] dynamic
			{ "RightOuterUp",         0 },  // [8] dynamic (분기)
		}},
		// 왼쪽 체인
		{ "Spine3", {
			{ "Temp_GCLeftDrape",    -1 },  // [0] Kinematic 루트
			{ "Temp_GCLeftDrape1",    0 },  // [1] dynamic
			{ "Temp_GCLeftDrape2",    1 },  // [2] dynamic
			{ "Temp_GCLeftDrape3",    2 },  // [3] dynamic
			{ "Temp_GCLeftDrape4",    3 },  // [4] dynamic
			{ "Temp_GCLeftDrape5",    4 },  // [5] dynamic
			{ "LeftOuterOpen",        0 },  // [6] dynamic (분기)
			{ "LeftOuterOpen1",       6 },  // [7] dynamic
			{ "LeftOuterUp",          0 },  // [8] dynamic (분기)
		}},
	};
	tBuildDesc.pBoneSourceModel = m_pModelCom;
	tBuildDesc.iChainID_Offset = 0;
	tBuildDesc.tDefaultPhysXDesc.fBoneRadius = 0.01f;
	tBuildDesc.tDefaultPhysXDesc.fSwingLimit = 120.f;   // 부모 본 기준으로 좌우/앞뒤로 흔들릴 수 있는 최대 각도(도 단위)
	tBuildDesc.tDefaultPhysXDesc.fTwistLimit = 120.f;   // 본 자체 축(길이 방향) 기준 비틀림 허용 각도
	tBuildDesc.tDefaultPhysXDesc.fDamping = 0.08f;      // 운동 에너지를 얼마나 빨리 감쇠시킬지
	tBuildDesc.tDefaultPhysXDesc.fStiffness = 30.f;     // 레스트 포즈로 복귀하려는 힘의 세기
	tBuildDesc.tDefaultPhysXDesc.fDensity = 0.1f;       // 본 충돌체의 밀도

	BoneChain_Build(tBuildDesc, m_pMasterModelCom);

	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture(L"RimNoise"), Com_Texture, RCAST(Component**)(&m_pRimNoiseTexture)), E_FAIL);

	return S_OK;
}
/******************************************************* PhysX Cloth 물리 *******************************************************/

Player_Outer* Player_Outer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player_Outer* pInstance = new Player_Outer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player_Outer 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Player_Outer::Clone(void* pArg)
{
	Player_Outer* pInstance = new Player_Outer(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player_Outer 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Player_Outer::Free()
{
	if (m_bIsClone)
	{
		BoneChain_Cleanup();
	}

	__super::Free();
}