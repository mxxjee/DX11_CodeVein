#include "Client_Define.h"
#include "Projectile_Homing.h"

#include "PoolingManager.h"
#include "PhysX_Function.h"
#include "Player.h"
#include "ParticleSystem.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Projectile_Homing::Projectile_Homing()
{
}

Client::Projectile_Homing::Projectile_Homing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Projectile(pDevice, pContext)
{
}

Client::Projectile_Homing::Projectile_Homing(const Projectile_Homing& original)
	: Projectile(original)
{
}

Client::Projectile_Homing::~Projectile_Homing()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Homing::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Projectile_Homing::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Projectile_Homing_" + to_wstring(namenum++);

	CHECK_FAILED(Projectile::Initialize(arg), E_FAIL); // 수정됨 : Projectile::Initialize 호출로 변경

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Homing::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 //////////////////////////////////////////////////////*/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Homing::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

	return 0;
}

_int Client::Projectile_Homing::Update_Parallel(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));

	m_fElapsedTime += fTimeDelta;

	// 유도 활성화 판정 (딜레이 이후 ~ 지속시간 이내)
	m_bHomingActive = (m_fElapsedTime >= m_fHomingDelay)
		&& (m_fElapsedTime <= m_fHomingDelay + m_fHomingDuration);

	// 가속도 적용
	Update_Speed_Homing(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Homing::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	// 유도 회전
	if (m_bHomingActive && !m_bArrived)
		Update_Homing(fTimeDelta);

	// 공격 및 벽 충돌 판정
	Move_With_Sweep_Monster(fTimeDelta);

#ifdef _DEBUG
	BoundingSphere debugSphere;
	debugSphere.Center = m_vCurrentProjectilePos;
	debugSphere.Radius = m_fAttackRadius;
	Add_Debug_Sphere(debugSphere, m_vDebugColor);
#endif // _DEBUG

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Projectile_Homing::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Homing::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Projectile_Homing::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 스폰/디스폰 함수 ////////////////////////////////////////////////////////
void Client::Projectile_Homing::OnSpawn(void* _arg)
{
	__super::OnSpawn(_arg);

	// 유도 파라미터 초기화
	m_fElapsedTime = 0.f;
	m_bHomingActive = false;
	m_bArrived = false;

	if (nullptr != _arg)
	{
		HOMING_DESC* pHomingDesc = CAST(HOMING_DESC*)(_arg);

		m_fHomingTurnSpeed = pHomingDesc->fHomingTurnSpeed;
		m_fHomingDelay = pHomingDesc->fHomingDelay;
		m_fHomingDuration = pHomingDesc->fHomingDuration;
		m_fAcceleration = pHomingDesc->fAcceleration;
		m_fMaxSpeed = pHomingDesc->fMaxSpeed;
		m_fMinSpeed = pHomingDesc->fMinSpeed;
		m_fHomingLosAngle = pHomingDesc->fHomingLosAngle;
		m_fHomingLosCos = cosf(XMConvertToRadians(m_fHomingLosAngle * 0.5f));
		m_fArrivalRadius = pHomingDesc->fArrivalRadius;
		m_fArrivalRadiusSq = m_fArrivalRadius * m_fArrivalRadius;
		m_bPredictTarget = pHomingDesc->bPredictTarget;
		m_vTargetOffset = pHomingDesc->vTargetOffset;

		m_eSpawnEffectID = pHomingDesc->eSpanwEffectID;
		m_eDeSpawnEffectID = pHomingDesc->eDeSpanwEffectID;
		m_fDefaultLifeTime = pHomingDesc->fLifeTime;

		_vector vLookDir = XMLoadFloat3(&pHomingDesc->vDir);
		m_pTransformCom->LookDir(vLookDir);

		m_strSpawnSound = pHomingDesc->strSpawnSound;
		m_strDespawnSound = pHomingDesc->strDespawnSound;

		if (!m_strSpawnSound.empty()) m_pGameInstance->Play_Sound_Unique(m_strSpawnSound, 0.2f, true);
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

			pEffect->Set_FollowParent(true);

			this->Set_FollowEffect(pEffect);
		}
	}
}

void Client::Projectile_Homing::OnDespawn()
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
/******************************************************* 스폰/디스폰 함수 *******************************************************/



//////////////////////////////////////////////////////// 유도 로직 ////////////////////////////////////////////////////////
void Client::Projectile_Homing::Update_Homing(const _float fTimeDelta)
{
	// 타겟 위치 계산
	_float3 vTargetPos = Calculate_TargetPosition();

	// 타겟까지의 방향 벡터
	_vector vTarget = XMLoadFloat3(&vTargetPos);
	_vector vMyPos = XMLoadFloat3(&m_vCurrentProjectilePos);
	_vector vToTarget = XMVectorSubtract(vTarget, vMyPos);

	_float fDistSq = XMVectorGetX(XMVector3LengthSq(vToTarget));

	// 도달 판정
	if (fDistSq <= m_fArrivalRadiusSq)
	{
		m_bArrived = true;
		return;
	}

	_vector vToTargetNorm = XMVector3Normalize(vToTarget);
	_vector vCurrentDir = XMVector3Normalize(XMLoadFloat3(&m_vDir));

	// 시야각 체크 (LOS angle 밖이면 직진 유지)
	_float fDot = XMVectorGetX(XMVector3Dot(vCurrentDir, vToTargetNorm));
	if (fDot < m_fHomingLosCos)
		return;

	// 현재 방향에서 타겟 방향으로 회전 (Monster::Update_HomingRotation 참고)
	_float fAngleRad = XMVectorGetX(XMVector3AngleBetweenVectors(vCurrentDir, vToTargetNorm));
	if (fAngleRad < 0.001f)
		return;

	_float fMaxTurnRad = XMConvertToRadians(m_fHomingTurnSpeed) * fTimeDelta;
	_float fTurnRad = min(fAngleRad, fMaxTurnRad);

	// 좌/우 회전 방향 결정
	_vector vCross = XMVector3Cross(vCurrentDir, vToTargetNorm);
	if (XMVectorGetY(vCross) < 0.f)
		fTurnRad = -fTurnRad;

	// Y축 회전 행렬 적용
	_matrix matRot = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTurnRad);
	_vector vNewDir = XMVector3TransformNormal(vCurrentDir, matRot);

	// 수직 보간 (Y축 유도)
	_float fTargetY = XMVectorGetY(vToTargetNorm);
	_float fCurrentY = XMVectorGetY(vNewDir);
	_float fVerticalTurnMax = XMConvertToRadians(m_fHomingTurnSpeed) * fTimeDelta;
	_float fYDiff = fTargetY - fCurrentY;
	_float fYStep = clamp(fYDiff, -fVerticalTurnMax, fVerticalTurnMax);

	vNewDir = XMVectorSetY(vNewDir, fCurrentY + fYStep);
	vNewDir = XMVector3Normalize(vNewDir);

	XMStoreFloat3(&m_vDir, vNewDir);
}

void Client::Projectile_Homing::Update_Speed_Homing(const _float fTimeDelta)
{
	// 가속도가 0이면 등속
	if (m_fAcceleration == 0.f)
		return;

	m_fSpeed += m_fAcceleration * fTimeDelta;

	// 최대/최소 속도 클램프
	if (m_fMaxSpeed > 0.f)
		m_fSpeed = min(m_fSpeed, m_fMaxSpeed);

	if (m_fMinSpeed > 0.f)
		m_fSpeed = max(m_fSpeed, m_fMinSpeed);
}

_float3 Client::Projectile_Homing::Calculate_TargetPosition()
{
	// 플레이어 캐싱
	if (nullptr == m_pPlayer)
		m_pPlayer = m_pGameInstance->Get_Player();

	_float3 vTargetPos = {};

	if (m_pPlayer != nullptr)
	{
		XMStoreFloat3(&vTargetPos, m_pPlayer->Get_Position());

		// 오프셋 적용 (높이 보정 등)
		vTargetPos.x += m_vTargetOffset.x;
		vTargetPos.y += m_vTargetOffset.y;
		vTargetPos.z += m_vTargetOffset.z;

		// bPredictTarget이 true면 플레이어 속도 기반 예측 위치 계산도 필요하면 만들듯
		// 현재는 플레이어 현재 위치 사용
	}

	return vTargetPos;
}
/******************************************************* 유도 로직 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Projectile_Homing* Client::Projectile_Homing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Projectile_Homing* pInstance = new Projectile_Homing(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Projectile_Homing 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Projectile_Homing::Clone(void* pArg)
{
	Projectile_Homing* pInstance = new Projectile_Homing(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Projectile_Homing 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Projectile_Homing::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/