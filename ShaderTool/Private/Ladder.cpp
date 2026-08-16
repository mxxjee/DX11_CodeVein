#include "ShaderTool_Define.h"
#include "Ladder.h"
#include "GameInstance.h"
#include "Level_Logo.h"
#include "Player.h"


CLadder::CLadder(ID3D11Device* pD, ID3D11DeviceContext* pC)
	: MapObject(pD, pC)
{
}

CLadder::CLadder(const CLadder& original)
	: MapObject(original)
{
}

HRESULT CLadder::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT CLadder::Initialize(void* arg)
{
	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	if (nullptr != arg)
	{
		MAPOBJECT_DESC* pDesc = (MAPOBJECT_DESC*)arg;

		if (pDesc->jExtraData.contains("LadderLength"))
		{
			m_iTotalLength = pDesc->jExtraData["LadderLength"].get<_float>();
		}
		else
		{
			m_iTotalLength = 5.0f;
		}

		m_iLength = (_uint)ceil(m_iTotalLength / 2.0f);
		if (m_iLength < 2)
			m_iLength = 2;
	}
	else
	{
		m_iLength = 5;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

	static _uint namenum = 0;
	m_wstrName = L"Ladder_" + to_wstring(namenum++);

	m_eObjType = OBJTYPE::TYPE_MAP;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_vecObjPass.push_back(0);
	}

	PHYSX_ACTOR_DESC actordesc;
	actordesc.matWorld = m_pTransformCom->Get_WorldFloat4x4();
	actordesc.pModel = m_pModelTopCom;
	m_pGameInstance->PhysX_Load_Static_Actor_Auto(actordesc);

	_matrix matOriginalWorld = m_pTransformCom->Get_WorldMatrix();

	for (_uint i = 0; i < m_iLength; ++i)
	{
		Model* pCurrentModel = (i == m_iLength - 1) ? m_pModelBottomCom : m_pModelMiddleCom;
		if (nullptr == pCurrentModel) continue;

		_matrix matOffset = XMMatrixTranslation(0.f, -2.0f * (_float)i, 0.f);
		_matrix matFinalWorld = matOffset * matOriginalWorld;

		_float4x4 matFinalFloat4x4;
		XMStoreFloat4x4(&matFinalFloat4x4, matFinalWorld);

		PHYSX_ACTOR_DESC bodyDesc;
		bodyDesc.matWorld = matFinalFloat4x4;
		bodyDesc.pModel = pCurrentModel;
		m_pGameInstance->PhysX_Load_Static_Actor_Auto(bodyDesc);
	}

	_vector vTop = m_pTransformCom->Get_State(DIRECTION::POSITION);
	XMStoreFloat3(&m_vTopPosition, vTop);

	_matrix matBottomOffset = XMMatrixTranslation(0.f, -2.0f * (_float)(m_iLength - 1), 0.f);
	_matrix matBottomWorld = matBottomOffset * matOriginalWorld;
	_vector vBottom = matBottomWorld.r[3];
	XMStoreFloat3(&m_vBottomPosition, vBottom);

	return S_OK;
}

HRESULT CLadder::Ready_Components()
{
	CHECK_FAILED(Add_Shader(L"Prototype_Component_Shader_VTXMesh"), E_FAIL);

	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_LadderTop", L"Com_Model_Top", RCAST(Component**)(&m_pModelTopCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_LadderMiddle", L"Com_Model_Middle", RCAST(Component**)(&m_pModelMiddleCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_LadderBottom", L"Com_Model_Bottom", RCAST(Component**)(&m_pModelBottomCom)), E_FAIL);

	m_pModelCom = m_pModelTopCom;

	if (m_pModelCom != nullptr)
	{
		m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());
	}

	return S_OK;
}

_int CLadder::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int CLadder::Update_Parallel(const _float fTimeDelta)
{
	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int CLadder::Update(const _float fDT)
{
	return __super::Update(fDT);
}

_int CLadder::Update_Late(const _float fDT)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);
	Add_RenderGroup(RENDER_GROUP::SHADOW_BAKE);

	__super::Update_Late(fDT);

	return 0;
}

HRESULT CLadder::Render(const _float fDT)
{
	if (false == m_bIsVisible)
		return S_OK;

	// CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	_matrix matOriginalWorld = m_pTransformCom->Get_WorldMatrix();

	// Top
	if (m_pModelTopCom != nullptr)
	{
		_float4x4 matFinalFloat4x4;

		XMStoreFloat4x4(&matFinalFloat4x4, matOriginalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < m_pModelTopCom->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 7;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Render(fDT, j), E_FAIL);
		}
	}

	// 사다리 본체
	for (_uint i = 0; i < m_iLength; ++i)
	{
		Model* pCurrentModel = nullptr;

		if (i == m_iLength - 1)
			pCurrentModel = m_pModelBottomCom;
		else
			pCurrentModel = m_pModelMiddleCom;

		if (nullptr == pCurrentModel) continue;

		_matrix matOffset = XMMatrixTranslation(0.f, -2.0f * (_float)i, 0.f);
		_matrix matFinalWorld = matOffset * matOriginalWorld;

		_float4x4 matFinalFloat4x4;
		XMStoreFloat4x4(&matFinalFloat4x4, matFinalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < pCurrentModel->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE), E_FAIL);
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 7;//_uint iPass = (m_vecObjPass.size() > j) ? m_vecObjPass[j] : 0;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(pCurrentModel->Render(fDT, j), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CLadder::Render_Shadow(const _float fDT, _int iCascadeNum)
{
	if (false == m_bIsVisible)
		return S_OK;

	// CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	_matrix matOriginalWorld = m_pTransformCom->Get_WorldMatrix();

	// Top
	if (m_pModelTopCom != nullptr)
	{
		_float4x4 matFinalFloat4x4;

		XMStoreFloat4x4(&matFinalFloat4x4, matOriginalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < m_pModelTopCom->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 5;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Render(fDT, j), E_FAIL);
		}
	}

	// 사다리 본체
	for (_uint i = 0; i < m_iLength; ++i)
	{
		Model* pCurrentModel = nullptr;

		if (i == m_iLength - 1)
			pCurrentModel = m_pModelBottomCom;
		else
			pCurrentModel = m_pModelMiddleCom;

		if (nullptr == pCurrentModel) continue;

		_matrix matOffset = XMMatrixTranslation(0.f, -2.0f * (_float)i, 0.f);
		_matrix matFinalWorld = matOffset * matOriginalWorld;

		_float4x4 matFinalFloat4x4;
		XMStoreFloat4x4(&matFinalFloat4x4, matFinalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < pCurrentModel->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE), E_FAIL);
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 5;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(pCurrentModel->Render(fDT, j), E_FAIL);
		}
	}

	return S_OK;
	return S_OK;
}

ordered_json CLadder::Get_ExtraData()
{
	ordered_json j = __super::Get_ExtraData();
	j["LadderLength"] = m_iTotalLength;
	return j;
}

CLadder* CLadder::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, LEVEL _level)
{
	CLadder* pInstance = new CLadder(pD, pC);
	if (FAILED(pInstance->Initialize_Prototype(_level)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

GameObject* CLadder::Clone(void* arg)
{
	CLadder* pInstance = new CLadder(*this);
	if (FAILED(pInstance->Initialize(arg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

#pragma endregion

void CLadder::Free()
{
	__super::Free();
}
