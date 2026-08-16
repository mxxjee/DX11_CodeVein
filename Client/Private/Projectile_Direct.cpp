#include "Client_Define.h"
#include "Projectile_Direct.h"

//#include "GameInstance.h"
#include "PoolingManager.h"
#include "ParticleSystem.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Projectile_Direct::Projectile_Direct()
{
}

Client::Projectile_Direct::Projectile_Direct(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Projectile(pDevice, pContext)
{
}

Client::Projectile_Direct::Projectile_Direct(const Projectile_Direct& original)
	: Projectile(original)
{
}

Client::Projectile_Direct::~Projectile_Direct()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Direct::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Projectile_Direct::Initialize(void* arg)
{
	static _uint namenum = 0;

	GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Projectile_Direct_" + to_wstring(namenum++);

	CHECK_FAILED(Projectile::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Direct::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Direct::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

	return 0;
}

_int Client::Projectile_Direct::Update_Parallel(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Direct::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	Move_With_Sweep_Monster(fTimeDelta);

#ifdef _DEBUG
	BoundingSphere desc;
	desc.Center = m_vCurrentProjectilePos;
	desc.Radius	= m_fAttackRadius;

	Add_Debug_Sphere(desc, m_vDebugColor);
#endif // _DEBUG

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Direct::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Direct::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Direct::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
void Client::Projectile_Direct::OnSpawn(void* _arg)
{
	__super::OnSpawn(_arg);

	if (nullptr != _arg)
	{
		PROJECTILE_DESC* pDesc = (PROJECTILE_DESC*)_arg;

		m_eSpawnEffectID = pDesc->eSpanwEffectID;
		m_eDeSpawnEffectID = pDesc->eDeSpanwEffectID;
		m_fAttackRadius = pDesc->fAttackRadius;
		m_vDebugColor = pDesc->vDebugColor;

		m_fAttackDamage = pDesc->fAttackDamage;
		m_fDefaultLifeTime = pDesc->fLifeTime;

		_vector vLookDir = XMLoadFloat3(&pDesc->vDir);
		m_pTransformCom->LookDir(vLookDir);

		m_strSpawnSound = pDesc->strSpawnSound;
		m_strDespawnSound = pDesc->strDespawnSound;

		m_pGameInstance->Play_Sound_Unique(m_strSpawnSound, 0.4f, true);
	}

	if (m_eSpawnEffectID != POOL_ID::EFFECT_END)
	{
		ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(m_eSpawnEffectID));

		if (nullptr != pEffect)
		{
			pEffect->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
			pEffect->Play();

			Transform* pTransform = m_pTransformCom;
			pEffect->Set_FollowCallback([pTransform]() -> _float4x4 {
				return pTransform->Get_WorldFloat4x4();
				});

			pEffect->Set_FollowParent(false);

			this->Set_FollowEffect(pEffect);
		}
	}
}

void Client::Projectile_Direct::OnDespawn()
{
	if (!m_strSpawnSound.empty()) m_pGameInstance->StopSound(m_strSpawnSound);
	// 플레이어한테 맞았으면
	if (m_bIsHit)
	{
		if (!m_strDespawnSound.empty()) m_pGameInstance->Play_Sound(m_strDespawnSound, 0.2f, false);

		if (m_eDeSpawnEffectID != POOL_ID::EFFECT_END)
		{
			ParticleSystem* pEffect = CAST(ParticleSystem*)(
				PoolingManager::Get_Instance()->Acquire_Effect(m_eDeSpawnEffectID));

			if (nullptr != pEffect)
			{
				pEffect->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
				pEffect->Set_FollowParent(false);
				pEffect->Set_FollowCallback(nullptr);
				pEffect->Play();
			}
		}
	}

	__super::OnDespawn();
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Projectile_Direct* Client::Projectile_Direct::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Projectile_Direct* pInstance = new Projectile_Direct(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Projectile_Direct 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Projectile_Direct::Clone(void* pArg)
{
	Projectile_Direct* pInstance = new Projectile_Direct(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Projectile_Direct 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Projectile_Direct::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
