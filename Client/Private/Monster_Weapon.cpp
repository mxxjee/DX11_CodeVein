#include "Client_Define.h"
#include "Monster_Weapon.h"

//#include "GameInstance.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Monster_Weapon::Monster_Weapon()
{
}

Client::Monster_Weapon::Monster_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: PartObject(pDevice, pContext)
{
}

Client::Monster_Weapon::Monster_Weapon(const Monster_Weapon& original)
	: PartObject(original)
{
}

Client::Monster_Weapon::~Monster_Weapon()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Weapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Monster_Weapon::Initialize(void* arg)
{
	MONSTERWEAPON_DESC Desc = {};
	MONSTERWEAPON_DESC* pDesc = static_cast<MONSTERWEAPON_DESC*>(arg);

	if (arg == nullptr)
	{
		arg = &Desc;
	}
	else
	{
		m_iSocketIndex = pDesc->iSocketIndex;
		m_pSocketMatrix = pDesc->pSocketMatrix;
		m_eWeaponType = pDesc->eWeaponType;
		m_vWeaponScale = pDesc->vWeaponScale;
		m_bIdentity = pDesc->bIdentity;
		m_pDissolveTime = pDesc->pDissolveTime;
		m_pDissolveMax = pDesc->pDissolveMax;
	}

	CHECK_FAILED(PartObject::Initialize(pDesc), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster_Weapon::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster_Weapon::Update_Parallel(const _float fTimeDelta)
{
	// 소켓 매트릭스에서 부모 행렬 갱신 후 전체 갱신
	const _float4x4* socket = CAST(Monster*)(m_pMasterRig)->Get_SocketMatrix(m_iSocketIndex);
	_matrix SocketMatrix = XMMatrixScaling(m_vWeaponScale.x, m_vWeaponScale.y, m_vWeaponScale.z) * NormalizeMatrix(XMLoadFloat4x4(socket));

	_matrix parentMatrix = SocketMatrix * XMLoadFloat4x4(m_matParentMatrix);
	m_pTransformCom->Set_Matrix(parentMatrix);

	if(m_bIdentity)
	{
		parentMatrix = PreMatrix_Identity * parentMatrix;
	}
	else
	{
		parentMatrix = PreMatrix_X * parentMatrix;
	}

	m_pModelCom->Update_CombinedMatrices_Weapon(parentMatrix);

#ifdef _DEBUG
	for (_uint i = 0; i < m_pModelCom->Get_Bones().size(); ++i)
	{
		m_pModelCom->Render_DebugBones_NoCS(XMMatrixIdentity(), i);
	}
#endif // _DEBUG

	return 0;
}

_int Client::Monster_Weapon::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Monster_Weapon::Update_Late(const _float fTimeDelta)
{;

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Weapon::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	// Noise Texture
	_uint bDissolving = (m_pDissolveTime && *m_pDissolveTime > 0.f);
	_uint bUseNoise = bDissolving ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		_float fTime = *m_pDissolveTime;
		_float fMax = (m_pDissolveMax && *m_pDissolveMax > 0.f) ? *m_pDissolveMax : 1.f;
		_float fNormalized = fTime / fMax;

		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fNormalized, sizeof(_float));
	}
	_float RimNoiseTime = 0;
	m_pShaderCom->Bind_RawValue_ByHandle(g_fRimOn, &RimNoiseTime, sizeof(_float));

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Monster_Weapon::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
