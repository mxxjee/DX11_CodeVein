#include "Client_Define.h"
#include "Bayonet_Bullet.h"

#include "Shader.h"
#include "Model.h"
#include "PhysX_Function.h"
#include "PoolingManager.h"

Client::Bayonet_Bullet::Bayonet_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Projectile(pDevice,pContext)
{
}

Client::Bayonet_Bullet::Bayonet_Bullet(const Bayonet_Bullet& original)
	:Projectile(original)
{
}

HRESULT Client::Bayonet_Bullet::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Bayonet_Bullet::Initialize(void* pArg)
{
	static _uint namenum = 0;

	BAYONETBULLET_DESC Desc;
	BAYONETBULLET_DESC* pDesc = (pArg == nullptr) ? &Desc : static_cast<BAYONETBULLET_DESC*>(pArg);
	if (pArg == nullptr)
	{
		pArg = &Desc;
	}
	else
	{

	}

	//pDesc->tControllerDesc.eActorType = PX_ACTOR_TYPE::PROJECTILE;
	//pDesc->tControllerDesc.iObjectID = m_iObjectID;
	//pDesc->tControllerDesc.fRadius = 0.1f; // CapsuleRadius 55.0 반영
	//pDesc->tControllerDesc.fHeight = 0.1f;  // CapsuleHalfHeight 100.0 반영
	//pDesc->tControllerDesc.pOwner = this;
	pDesc->fSpeed = 15.f;
	pDesc->fLifeTime = 2.0f;
	pDesc->eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;


	(pDesc)->wstrName = L"Bayonet_Bullet" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	//m_pController = Create_Controller(pDesc->tControllerDesc);
	//m_pGameInstance->PhysX_Disable_Controller(m_pController); //컨트롤러 비활성화

	return S_OK;
}

_int Client::Bayonet_Bullet::Update_Priority(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	return 0;
}

_int Client::Bayonet_Bullet::Update_Parallel(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	__super::Update_Parallel(fTimeDelta);
	
	return 0;
}

_int Client::Bayonet_Bullet::Update(const _float fTimeDelta)
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

	//__super::Process_AttackSweep();
	Move_With_Sweep(fTimeDelta);

	_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

	COUT("x:" << XMVectorGetX(vMyPos));
	COUT("y:" << XMVectorGetY(vMyPos));
	COUT("z:" << XMVectorGetZ(vMyPos));
	COUT("==============================");

	return 0;
}

_int Client::Bayonet_Bullet::Update_Late(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	return 0;
}

HRESULT Client::Bayonet_Bullet::Render(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	return S_OK;
}

void Client::Bayonet_Bullet::OnSpawn(void* _arg)
{
	__super::OnSpawn(_arg);
}

void Client::Bayonet_Bullet::OnDespawn()
{
	__super::OnDespawn();
}

Bayonet_Bullet* Client::Bayonet_Bullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Bayonet_Bullet* pInstance = new Bayonet_Bullet(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Bayonet_Bullet 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Bayonet_Bullet::Clone(void* pArg)
{
	Bayonet_Bullet* pInstance = new Bayonet_Bullet(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Bayonet_Bullet 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
void Client::Bayonet_Bullet::Free()
{
	__super::Free();
}
