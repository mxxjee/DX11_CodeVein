#include "Client_Define.h"
#include "Projectile_Effect.h"

//#include "GameInstance.h"
#include "PoolingManager.h"
#include "ParticleSystem.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Projectile_Effect::Projectile_Effect()
{
}

Client::Projectile_Effect::Projectile_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Projectile(pDevice, pContext)
{
}

Client::Projectile_Effect::Projectile_Effect(const Projectile_Effect& original)
	: Projectile(original)
{
}

Client::Projectile_Effect::~Projectile_Effect()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Effect::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Projectile_Effect::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Projectile_Effect_" + to_wstring(namenum++);

	CHECK_FAILED(Projectile::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Effect::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Effect::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

	return 0;
}

_int Client::Projectile_Effect::Update_Parallel(const _float fTimeDelta)
{
	//CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));
	if (!m_bIsActive)
		return -1;

	m_fLifeTime -= fTimeDelta;
	Move_Up(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Effect::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	// 이펙트용이라 타격판정 X
	//Move_With_Sweep_Monster(fTimeDelta);

#ifdef _DEBUG
	BoundingSphere desc;
	desc.Center = Get_Position_Float3();
	desc.Radius = m_fAttackRadius;

	Add_Debug_Sphere(desc, m_vDebugColor);
#endif // _DEBUG

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Effect::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Effect::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Effect::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 전용 이동 함수 ////////////////////////////////////////////////////////
void Client::Projectile_Effect::Move_Up(const _float fTimeDelta)
{
	m_fCurrentAngle += 250.f * fTimeDelta; // 현재 각도 변경

	// 각도만큼 회전
	_float radian = XMConvertToRadians(m_fCurrentAngle);
	_matrix rotationMatrix = XMMatrixRotationY(radian);
	_vector referenceAngle = XMLoadFloat3(&m_vDir);
	_vector rotatedDirection = XMVector3TransformNormal(referenceAngle, rotationMatrix);

	rotatedDirection = XMVectorSetY(rotatedDirection, 0.2f);

	rotatedDirection = rotatedDirection * fTimeDelta * 30.f;

	// 적용
	m_pTransformCom->Translate(rotatedDirection);
}
/******************************************************* 전용 이동 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
void Client::Projectile_Effect::OnSpawn(void* _arg)
{
	__super::OnSpawn(_arg);

	m_fCurrentAngle = 0.f;

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
			pEffect->Set_FollowParent(false);

			Transform* pTransform = m_pTransformCom;
			pEffect->Set_FollowCallback([pTransform]() -> _float4x4 {
				return pTransform->Get_WorldFloat4x4();
				});

			this->Set_FollowEffect(pEffect);
		}
	}
}

void Client::Projectile_Effect::OnDespawn()
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
Projectile_Effect* Client::Projectile_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Projectile_Effect* pInstance = new Projectile_Effect(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Projectile_Effect 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Projectile_Effect::Clone(void* pArg)
{
	Projectile_Effect* pInstance = new Projectile_Effect(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Projectile_Effect 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Projectile_Effect::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
