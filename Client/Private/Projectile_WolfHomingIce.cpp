#include "Client_Define.h"
#include "Projectile_WolfHomingIce.h"

//#include "GameInstance.h"
#include "PoolingManager.h"
#include "ParticleSystem.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Projectile_WolfHomingIce::Projectile_WolfHomingIce()
{
}

Client::Projectile_WolfHomingIce::Projectile_WolfHomingIce(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Projectile(pDevice, pContext)
{
}

Client::Projectile_WolfHomingIce::Projectile_WolfHomingIce(const Projectile_WolfHomingIce& original)
	: Projectile(original)
{
}

Client::Projectile_WolfHomingIce::~Projectile_WolfHomingIce()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_WolfHomingIce::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Projectile_WolfHomingIce::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Projectile_WolfHomingIce_" + to_wstring(namenum++);

	CHECK_FAILED(Projectile::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_WolfHomingIce::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_WolfHomingIce::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

	return 0;
}

_int Client::Projectile_WolfHomingIce::Update_Parallel(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));

	Update_MoveDirection(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_WolfHomingIce::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	Move_With_Sweep_Monster(fTimeDelta);

#ifdef _DEBUG
	BoundingSphere desc;
	desc.Center = m_vCurrentProjectilePos;
	desc.Radius = m_fAttackRadius;

	Add_Debug_Sphere(desc, m_vDebugColor);
#endif // _DEBUG

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_WolfHomingIce::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_WolfHomingIce::Render(const _float fTimeDelta)
{
	//MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_WolfHomingIce::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
void Client::Projectile_WolfHomingIce::OnSpawn(void* _arg)
{
	__super::OnSpawn(_arg);

	m_iHomingCount = 0;
	m_fWaitTime = 0.5f;
	m_fMoveSpeed = m_fSpeed;	// Desc로 받아온 속도 저장
	m_fPhaseElapsedTime = 0.f;
	m_ePhase = HOMING_PHASE::READY;

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

void Client::Projectile_WolfHomingIce::OnDespawn()
{
	// 플레이어한테 맞았으면
	if (m_bIsHit)
	{
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



//////////////////////////////////////////////////////// 이동 방향 업데이트 ////////////////////////////////////////////////////////
void Client::Projectile_WolfHomingIce::Update_MoveDirection(const _float fTimeDelta)
{
	// 이 패턴에는 3가지 단계가 있다
	// 늑대에게서 생성되어서 3쩜 위치에 가는것
	// 플레이어 방향으로 이동하는것
	// 잠시 멈춰있는것
	// switch문으로 구현

	switch (m_ePhase)
	{
		// 3쩜으로 가는 단계
	case HOMING_PHASE::READY:
		// 시간 체크만 하고 지나면 공격 기다리기
		// 이 시간동안 처음 생성된 방향으로 계속 이동할것
		if (m_fPhaseElapsedTime >= 0.4f)
		{
			m_fSpeed = 0.f;	// Wait해야하니까 이동 못 하게 막기
			m_fPhaseElapsedTime = 0.f;
			m_ePhase = HOMING_PHASE::WAIT;
		}
		break;

	case HOMING_PHASE::ATTACK:
		if (m_fPhaseElapsedTime >= 0.4f)
		{
			m_fSpeed = 0.f;	// Wait해야하니까 이동 못 하게 막기
			m_fPhaseElapsedTime = 0.f;
			m_ePhase = HOMING_PHASE::WAIT;
			m_fWaitTime = m_pGameInstance->RandomValue(0.55f, 1.f);	// 얼마나 기다릴지 랜덤
			
			// 2번 공격했으면
			if (++m_iHomingCount >= 2)
			{
				// 넌 끝나야해
				POOLING->Despawn_Object(this);
			}
		}
		break;

	case HOMING_PHASE::WAIT:
		if (m_fPhaseElapsedTime >= m_fWaitTime)
		{
			// 플레이어쪽으로의 방향 구하기
			_vector playerPos = m_pGameInstance->Get_PlayerPos_Vector();
			_vector direction = playerPos - m_pTransformCom->Get_Position_Vector();
			direction = XMVector3Normalize(XMVectorSetY(direction, 0.f));	// Y값 제거한 방향벡터
			XMStoreFloat3(&m_vDir, direction);	// 방향 설정
			m_fSpeed = m_fMoveSpeed;	// 속도 원복
			m_ePhase = HOMING_PHASE::ATTACK;

			m_fPhaseElapsedTime = 0.f;
		}
		break;
	}

	m_fPhaseElapsedTime += fTimeDelta;
}
/******************************************************* 이동 방향 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Projectile_WolfHomingIce* Client::Projectile_WolfHomingIce::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Projectile_WolfHomingIce* pInstance = new Projectile_WolfHomingIce(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Projectile_WolfHomingIce 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Projectile_WolfHomingIce::Clone(void* pArg)
{
	Projectile_WolfHomingIce* pInstance = new Projectile_WolfHomingIce(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Projectile_WolfHomingIce 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Projectile_WolfHomingIce::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
