#include "MT_Defines.h"
#include "Monster.h"

#include "GameInstance.h"
#include "Shader.h"
#include "Model.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Character(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& original)
	: Character(original)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* arg)
{
	MONSTER_DESC* pArg = (MONSTER_DESC*)arg;
	MONSTER_DESC Desc = {};

	if (nullptr != pArg && !pArg->jExtraData.empty())
	{
		if (pArg->jExtraData.contains("EnemyType"))
			m_eEnemyType = (ENEMY_TYPE)pArg->jExtraData["EnemyType"].get<int>();

		if (pArg->jExtraData.contains("WeaponType"))
			m_eWeaponType = (WEAPON_TYPE)pArg->jExtraData["WeaponType"].get<int>();
	}
	if (nullptr == pArg)
	{
		// 기본 물리 세팅
		Desc.fSpeed = 5.3f; // MaxWalkSpeed 530.0 반영
		Desc.fRotationSpeed = XMConvertToRadians(90.f); // RotateSpeed 90.0 반영
		pArg = &Desc;
	}

	m_pController = m_pGameInstance->PhysX_Create_Controller(pArg->tControllerDesc);

	static _uint iCount = 0;
	m_wstrName = L"Player_" + std::to_wstring(iCount++);
	pArg->wstrName = m_wstrName;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}

HRESULT CMonster::Ready_Components()
{
	CHECK_FAILED(Add_Shader(L"Prototype_Component_Shader_VTXAnimMesh"), E_FAIL);
	CHECK_FAILED(Add_Model(Proto_Model(L"Oliver_Phase1")), E_FAIL);

	m_pModelCom->Set_Animation_CS(0, true);
	m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());

	m_vecObjPass.clear();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
		m_vecObjPass.push_back(0);

	return S_OK;
}

_int CMonster::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int CMonster::Update_Parallel(const _float fTimeDelta)
{
	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int CMonster::Update(const _float fTimeDelta)
{
	m_pModelCom->Play_Animation_CS(fTimeDelta);

	Movement_Test(fTimeDelta);

	__super::Update(fTimeDelta);

	return 0;
}

_int CMonster::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	Add_RenderGroup(RENDER_GROUP::NONBLEND);

	return 0;
}

HRESULT CMonster::Render(const _float fTimeDelta)
{
	if (FAILED(__super::Render(fTimeDelta))) return E_FAIL;

	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT CMonster::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pShaderCom->Begin(1, true), E_FAIL);
		m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);
		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_Resources(1), E_FAIL);
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

void CMonster::Movement_Test(const _float fTimeDelta)
{
	_long MouseMoveX = 0;
	if ((MouseMoveX = m_pGameInstance->MouseMove(MOUSEMOVESTATE::X)))
	{
		_float fSensitivity = 0.1f;
		_float fTurnRadian = MouseMoveX * fSensitivity * fTimeDelta;

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_matrix RotationMatrix = XMMatrixRotationAxis(vUp, fTurnRadian);

		_vector vRight = m_pTransformCom->Get_State(DIRECTION::RIGHT);
		_vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);

		m_pTransformCom->Set_State(DIRECTION::RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
		m_pTransformCom->Set_State(DIRECTION::LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
	}

	_vector vMoveDir = XMVectorZero();
	_vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
	_vector vRight = m_pTransformCom->Get_State(DIRECTION::RIGHT);

	vLook = XMVector3Normalize(vLook);
	vRight = XMVector3Normalize(vRight);

	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80) vMoveDir += vLook;
	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80) vMoveDir -= vLook;
	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80) vMoveDir -= vRight;
	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80) vMoveDir += vRight;

	if (XMVectorGetX(XMVector3Length(vMoveDir)) > 0.f)
		vMoveDir = XMVector3Normalize(vMoveDir);

	_float fSpeed = 5.3f;
	_vector vDisplacement = vMoveDir * fSpeed * fTimeDelta;

	m_fVelocityY += -9.81f * fTimeDelta;
	vDisplacement = XMVectorSetY(vDisplacement, XMVectorGetY(vDisplacement) + (m_fVelocityY * fTimeDelta));

	if (m_pController)
	{
		PxVec3 pxDisp(XMVectorGetX(vDisplacement), XMVectorGetY(vDisplacement), XMVectorGetZ(vDisplacement));
		PxControllerFilters filters;

		PxControllerCollisionFlags flags = m_pController->move(pxDisp, 0.01f, fTimeDelta, filters);

		if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
			m_fVelocityY = 0.f;

		PxExtendedVec3 pxPos = m_pController->getPosition();
		m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet((_float)pxPos.x, (_float)pxPos.y - 0.4f , (_float)pxPos.z, 1.f));
	}
}

ordered_json CMonster::Get_ExtraData()
{
	ordered_json j;
	//j["LadderLength"] = m_iTotalLength;
	return j;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"CMonster 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"CMonster 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();
	if (m_pController)
		m_pController = nullptr;
}
