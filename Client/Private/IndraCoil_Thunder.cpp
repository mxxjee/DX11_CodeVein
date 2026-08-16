#include "Client_Define.h"
#include "IndraCoil_Thunder.h"

#include "PhysX_Function.h"
#include "PoolingManager.h"

Client::IndraCoil_Thunder::IndraCoil_Thunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Projectile(pDevice, pContext)
{
}

Client::IndraCoil_Thunder::IndraCoil_Thunder(const IndraCoil_Thunder& original)
	:Projectile(original)
{
}

HRESULT Client::IndraCoil_Thunder::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::IndraCoil_Thunder::Initialize(void* pArg)
{
	static _uint namenum = 0;

	INDRACOILTHUNDER_DESC Desc;
	INDRACOILTHUNDER_DESC* pDesc = (pArg == nullptr) ? &Desc : static_cast<INDRACOILTHUNDER_DESC*>(pArg);
	if (pArg == nullptr)
	{
		pArg = &Desc;
	}
	else
	{

	}

	pDesc->eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;


	(pDesc)->wstrName = L"IndraCoil_Thunder" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);


	return S_OK;
}

_int Client::IndraCoil_Thunder::Update_Priority(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	return 0;
}

_int Client::IndraCoil_Thunder::Update_Parallel(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int Client::IndraCoil_Thunder::Update(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;



	if (m_fLifeTime <= 0.f)
	{
		PoolingManager::Get_Instance()->Despawn_Object(this);
		return 0;
	}


#ifdef _DEBUG
	if (m_bAttackSweepActive)
	{
		// 무기 위치에 디버그 구체 그리기
		BoundingSphere debugSphere;
		debugSphere.Center = m_vCurrentProjectilePos;
		debugSphere.Radius = m_fAttackRadius;

		m_pGameInstance->Add_Debug_Sphere(debugSphere);
	}
#endif // _DEBUG

	__super::Process_AttackOverlap();
	//Move_With_Sweep(fTimeDelta);

	_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

	COUT("x:" << XMVectorGetX(vMyPos));
	COUT("y:" << XMVectorGetY(vMyPos));
	COUT("z:" << XMVectorGetZ(vMyPos));
	COUT("==============================");

	return 0;
}

_int Client::IndraCoil_Thunder::Update_Late(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	return 0;
}

HRESULT Client::IndraCoil_Thunder::Render(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	return S_OK;
}

void Client::IndraCoil_Thunder::OnSpawn(void* _arg)
{
	__super::OnSpawn(_arg);
}

void Client::IndraCoil_Thunder::OnDespawn()
{
	__super::OnDespawn();
}

IndraCoil_Thunder* Client::IndraCoil_Thunder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	IndraCoil_Thunder* pInstance = new IndraCoil_Thunder(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"IndraCoil_Thunder 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::IndraCoil_Thunder::Clone(void* pArg)
{
	IndraCoil_Thunder* pInstance = new IndraCoil_Thunder(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"IndraCoil_Thunder 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
void Client::IndraCoil_Thunder::Free()
{
	__super::Free();
}
