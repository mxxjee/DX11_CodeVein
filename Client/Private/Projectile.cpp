#include "Client_Define.h"
#include "Projectile.h"
#include "PoolingManager.h"
#include "PhysX_Function.h"
#include "Player.h"
#include "ParticleSystem.h"

Client::Projectile::Projectile()
{
}

Client::Projectile::Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:GameObject(pDevice, pContext)
{
}

Client::Projectile::Projectile(const Projectile& original)
	:GameObject(original)
{
}

HRESULT Client::Projectile::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Projectile::Initialize(void* _arg)
{
	PROJECTILE_DESC Desc = {};
	PROJECTILE_DESC* pDesc = {};

	if (_arg == nullptr)
	{
		pDesc = &Desc;
	}
	else
	{
        pDesc =  static_cast<PROJECTILE_DESC*>(_arg);
        m_fDefaultLifeTime = pDesc->fLifeTime;
        m_fLifeTime = m_fDefaultLifeTime;
        m_eProjectileDamagePower = pDesc->eProjectileDamagePower;
        m_bPiercing = pDesc->bPiercing;
        m_iMaxHitCount = pDesc->iMaxHitCount;
        m_fAttackDamage = pDesc->fAttackDamage;

	}

    if (m_fAttackDamage >= 0.f)
        m_fAttackDamage = 200.f;

	CHECK_FAILED(__super::Initialize(pDesc), E_FAIL);

	return S_OK;

}

_int Client::Projectile::Update_Priority(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	return 0;
}

_int Client::Projectile::Update_Parallel(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

	//Parallel 함수만 자식에서 호출하도록 해놓음 (수명계산중)

	m_fLifeTime -= fTimeDelta;
	
	if(m_pBoneMatrix == nullptr)
		Update_ProjectilePosition();
	else
		Update_ProjectileBone();
	


    return 0;
}

_int Client::Projectile::Update(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;

    if (m_fLifeTime <= 0.f)
    {
        PoolingManager::Get_Instance()->Despawn_Object(this);
        return 0;
    }

	return 0;
}

_int Client::Projectile::Update_Late(const _float fTimeDelta)
{
	if (m_bIsActive == false)
		return -1;


	return 0;
}

HRESULT Client::Projectile::Render(const _float fTimeDelta)
{
	return S_OK;
}

void Client::Projectile::Move_With_Sweep(const _float fTimeDelta)
{
	if (!m_bEnablePhysics) return;

	//_float fSpeed = m_pTransformCom->Get_Speed();
	_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vDir));
	_float fMoveDist = m_fSpeed * fTimeDelta;

	PxTransform pose(ToPxVec3(m_vCurrentProjectilePos));
	PxVec3 pxDir(XMVectorGetX(vDir), 0.f, XMVectorGetZ(vDir));

	if (pxDir.magnitudeSquared() < 0.0001f)
		return;

	pxDir.normalize();

	if (m_bPiercing) //관통가능한 투사체 
	{
		PHYSX_MULTI_SWEEP_RESULT MultiSweepReulst = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, pxDir, fMoveDist);

		if (MultiSweepReulst.bHit)
		{
			for (_uint i = 0; i < MultiSweepReulst.vecUserData.size(); ++i)
			{
				PHYSX_USER_DATA* pUserData = MultiSweepReulst.vecUserData[i];

				if (pUserData == nullptr)
					continue;
				if (pUserData->iObjectID == m_iObjectID)
					continue;
				if (m_setHitTargets.count(pUserData->iObjectID) > 0)
					continue;


				if (pUserData->eActorType == PX_ACTOR_TYPE::MONSTER)
				{
					m_setHitTargets.insert(pUserData->iObjectID);

					_float3 vAttackDir = Get_Look_Float3();

					DAMAGE_EVENT damageEvent = {};
					damageEvent.iAttackerID = m_iObjectID;
					damageEvent.iTargetID = pUserData->iObjectID;
					damageEvent.fDamage = m_fAttackDamage;
					damageEvent.vHitPosition = MultiSweepReulst.vecPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
					damageEvent.vHitDirection = vAttackDir;
					damageEvent.fKnockbackForce = m_fKnockbackForce;
					damageEvent.eDamagePower = m_eProjectileDamagePower;
					damageEvent.pDamageOwner = this;

					m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);

					if (m_iMaxHitCount > 0 && m_setHitTargets.size() >= m_iMaxHitCount) //관통할때 최대히트개수 보다 크면 비활성화
					{
						PoolingManager::Get_Instance()->Despawn_Object(this);
						return;
					}
				}
			}
		}

	}
	else //
	{
		PHYSX_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere(
			m_fAttackRadius, pose, pxDir, fMoveDist);

		if (sweepResult.bHit && sweepResult.pUserData)
		{
			PHYSX_USER_DATA* pUserData = sweepResult.pUserData;

			if (pUserData->iObjectID != m_iObjectID)
			{
				_bool isHit = false;

				// 몬스터 충돌 시 데미지 처리
				if (pUserData->eActorType == PX_ACTOR_TYPE::MONSTER)
				{
					isHit = true;

					_float3 vAttackDir = Get_Look_Float3();

					DAMAGE_EVENT damageEvent = {};
					damageEvent.iAttackerID = m_iObjectID;
					damageEvent.iTargetID = pUserData->iObjectID;
					damageEvent.fDamage = m_fAttackDamage;
					damageEvent.vHitPosition = sweepResult.vPosition;
					damageEvent.vHitDirection = vAttackDir;
					damageEvent.fKnockbackForce = m_fKnockbackForce;
					damageEvent.eDamagePower = m_eProjectileDamagePower;
					damageEvent.pDamageOwner = this;

					m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
				}
				else if (pUserData->eActorType == PX_ACTOR_TYPE::STATIC_MAP)
				{
					isHit = true;
				}

				if (isHit)
				{
					// 벽이든 몬스터든 첫 충돌에서 디스폰
					PoolingManager::Get_Instance()->Despawn_Object(this);
					return;
				}

			}
		}
	}

	// 충돌 없으면 트랜스폼 직접 이동
	_vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
	vPos = XMVectorAdd(vPos, XMVectorScale(vDir, fMoveDist));
	m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);

}

void Client::Projectile::Move_With_Sweep_Monster(const _float fTimeDelta)
{
	if (!m_bEnablePhysics) return;

	//_float fSpeed = m_pTransformCom->Get_Speed();
	_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vDir));
	_float fMoveDist = m_fSpeed * fTimeDelta;

	PxTransform pose(ToPxVec3(m_vCurrentProjectilePos));
	PxVec3 pxDir(XMVectorGetX(vDir), XMVectorGetY(vDir), XMVectorGetZ(vDir));

	if (pxDir.magnitudeSquared() < 0.0001f)
		return;

	pxDir.normalize();

	if (m_bPiercing) //관통가능한 투사체 
	{
		PHYSX_MULTI_SWEEP_RESULT MultiSweepReulst = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, pxDir, fMoveDist);

		if (MultiSweepReulst.bHit)
		{
			for (_uint i = 0; i < MultiSweepReulst.vecUserData.size(); ++i)
			{
				PHYSX_USER_DATA* pUserData = MultiSweepReulst.vecUserData[i];

				if (pUserData == nullptr)
					continue;
				if (pUserData->iObjectID == m_iObjectID)
					continue;
				if (m_setHitTargets.count(pUserData->iObjectID) > 0)
					continue;


				if (pUserData->eActorType == PX_ACTOR_TYPE::PLAYER || pUserData->eActorType == PX_ACTOR_TYPE::COMPANION)
				{
					m_setHitTargets.insert(pUserData->iObjectID);

					_float3 vAttackDir = Get_Look_Float3();

					DAMAGE_EVENT damageEvent = {};
					damageEvent.iAttackerID = m_iObjectID;
					damageEvent.iTargetID = pUserData->iObjectID;
					damageEvent.fDamage = m_fAttackDamage;
					damageEvent.vHitPosition = MultiSweepReulst.vecPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
					damageEvent.vHitDirection = vAttackDir;
					damageEvent.fKnockbackForce = m_fKnockbackForce;
					damageEvent.eDamagePower = m_eProjectileDamagePower;
					damageEvent.pDamageOwner = this;

					m_bIsHit = true;

					m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);

					if (m_iMaxHitCount > 0 && m_setHitTargets.size() >= m_iMaxHitCount) //관통할때 최대히트개수 보다 크면 비활성화
					{
						PoolingManager::Get_Instance()->Despawn_Object(this);
						return;
					}
				}
			}
		}

	}
	else //
	{
		PHYSX_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere(
			m_fAttackRadius, pose, pxDir, fMoveDist);

		if (sweepResult.bHit && sweepResult.pUserData)
		{
			PHYSX_USER_DATA* pUserData = sweepResult.pUserData;

			if (pUserData->iObjectID != m_iObjectID)
			{
				_bool isHit = false;

				// 충돌 시 데미지 처리
				if (pUserData->eActorType == PX_ACTOR_TYPE::PLAYER || pUserData->eActorType == PX_ACTOR_TYPE::COMPANION)
				{
					isHit = true;

					_float3 vAttackDir = Get_Look_Float3();

					DAMAGE_EVENT damageEvent = {};
					damageEvent.iAttackerID = m_iObjectID;
					damageEvent.iTargetID = pUserData->iObjectID;
					damageEvent.fDamage = m_fAttackDamage;
					damageEvent.vHitPosition = sweepResult.vPosition;
					damageEvent.vHitDirection = vAttackDir;
					damageEvent.fKnockbackForce = m_fKnockbackForce;
					damageEvent.eDamagePower = m_eProjectileDamagePower;
					damageEvent.pDamageOwner = this;

					m_bIsHit = true;

					m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
				}
				else if (pUserData->eActorType == PX_ACTOR_TYPE::STATIC_MAP)
				{
					isHit = true;
				}

				if (isHit)
				{
					// 벽이든 플레이어든 첫 충돌에서 디스폰
					PoolingManager::Get_Instance()->Despawn_Object(this);
					return;
				}

			}
		}

		// 충돌 없으면 트랜스폼 직접 이동
		_vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
		vPos = XMVectorAdd(vPos, XMVectorScale(vDir, fMoveDist));
		m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);
	}
}

void Client::Projectile::Move(const _float fTimeDelta)
{
	if (nullptr == m_pController) return;

	//중력 필요한 발사체 생기면 추후 제어해주기
	if (m_bEnablePhysics)
	{
		_float fSpeed = m_pTransformCom->Get_Speed();
		physx::PxVec3 vMoveDir(m_vDir.x, 0.f, m_vDir.z);

		if (vMoveDir.magnitudeSquared() > 0.0001f)
			vMoveDir.normalize();

        physx::PxVec3 vFinalVelocity = vMoveDir * m_fSpeed * fTimeDelta;

		physx::PxVec3 vFinalDisplacement = vFinalVelocity;

		physx::PxControllerFilters filters;
		physx::PxControllerCollisionFlags flags = m_pController->move(
			vFinalDisplacement, 0.0001f, fTimeDelta, filters);

		if (flags & physx::PxControllerCollisionFlag::eCOLLISION_SIDES) //오브젝트 충돌했을때
		{
			PoolingManager::Get_Instance()->Despawn_Object(this);
			return;
		}

		physx::PxExtendedVec3 pxPos = m_pController->getPosition();
		_vector vNewPos = XMVectorSet((_float)pxPos.x, (_float)pxPos.y, (_float)pxPos.z, 1.f);
		m_pTransformCom->Set_State(DIRECTION::POSITION, vNewPos);
	}

}

void Client::Projectile::Process_AttackSweep()
{
	if (!m_bAttackSweepActive)
		return;

	if (nullptr == m_pPlayer) m_pPlayer = m_pGameInstance->Get_Player();

	_float3 vSweepDir = {
	m_vCurrentProjectilePos.x - m_vPrevProjectilePos.x,
	m_vCurrentProjectilePos.y - m_vPrevProjectilePos.y,
	m_vCurrentProjectilePos.z - m_vPrevProjectilePos.z
	};

	_float fSweepDist = sqrtf(
		vSweepDir.x * vSweepDir.x +
		vSweepDir.y * vSweepDir.y +
		vSweepDir.z * vSweepDir.z);

	PxTransform pose(PxVec3(
		m_vPrevProjectilePos.x,
		m_vPrevProjectilePos.y,
		m_vPrevProjectilePos.z));


	// 거리에 따라 Overlap 또는 Sweep 선택
	vector<PHYSX_USER_DATA*> vecHitData;
	vector<_float3> vecHitPositions; // 접촉 위치 저장용

	if (fSweepDist < 0.01f)
	{
		// 무기가 거의 안 움직였으면 Overlap
		pose.p = PxVec3(m_vCurrentProjectilePos.x, m_vCurrentProjectilePos.y, m_vCurrentProjectilePos.z);
		PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackRadius, pose);

		if (overlapResult.bHit)
		{
			vecHitData = overlapResult.vecUserData;
			vecHitPositions = overlapResult.vecHitData;
		}
	}
	else
	{
		// 궤적 Sweep
		PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
		PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, dir, fSweepDist);

		if (sweepResult.bHit)
		{
			vecHitData = sweepResult.vecUserData;
			vecHitPositions = sweepResult.vecPositions; // sweep 결과의 실제 접촉 위치 사용
		}
	}

	// 히트 처리 (공통)
	for (_uint i = 0; i < vecHitData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = vecHitData[i];

		if (pUserData == nullptr)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER)
			continue;
		if (pUserData->iObjectID == m_iObjectID)
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0)
			continue;
		m_setHitTargets.insert(pUserData->iObjectID);

		// 데미지 이벤트 발행
		_float3 vAttackDir = Get_Look_Float3();

		DAMAGE_EVENT damageEvent = {};
		damageEvent.iAttackerID = m_iObjectID;
		damageEvent.iTargetID = pUserData->iObjectID;
		damageEvent.fDamage = m_fAttackDamage;
		damageEvent.vHitPosition = vecHitPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_eProjectileDamagePower;
		damageEvent.pDamageOwner = this;

		m_bIsHit = true;

		if (m_bIsHit)
		{
			PoolingManager::Get_Instance()->Despawn_Object(this);
		}

		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
	}
}

void Client::Projectile::Process_AttackOverlap()
{
    if (!m_bAttackSweepActive)
        return;

    PxTransform pose(PxVec3(
        m_vCurrentProjectilePos.x,
        m_vCurrentProjectilePos.y,
        m_vCurrentProjectilePos.z));

    PHYSX_OVERLAP_RESULT overlapResult =
        m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackRadius, pose);

    if (!overlapResult.bHit)
        return;

    for (_uint i = 0; i < overlapResult.vecUserData.size(); ++i)
    {
        PHYSX_USER_DATA* pUserData = overlapResult.vecUserData[i];

        if (pUserData == nullptr) continue;
        if (pUserData->iObjectID == m_iObjectID) continue;
        if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER) continue;
        if (m_setHitTargets.count(pUserData->iObjectID) > 0) continue;

        m_setHitTargets.insert(pUserData->iObjectID);

		_float3 vAttackDir = Get_Look_Float3();

        DAMAGE_EVENT damageEvent = {};
        damageEvent.iAttackerID = m_iObjectID;
        damageEvent.iTargetID = pUserData->iObjectID;
        damageEvent.fDamage = m_fAttackDamage;
        damageEvent.vHitPosition = overlapResult.vecHitData[i];
        damageEvent.vHitDirection = vAttackDir;
        damageEvent.fKnockbackForce = m_fKnockbackForce;
        damageEvent.eDamagePower = m_eProjectileDamagePower;
        damageEvent.pDamageOwner = this;

        m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
    }
}

void Client::Projectile::Process_AttackSweep_Monster()
{
	if (!m_bAttackSweepActive)
		return;

	if (nullptr == m_pPlayer) m_pPlayer = m_pGameInstance->Get_Player();

	_float3 vSweepDir = {
	m_vCurrentProjectilePos.x - m_vPrevProjectilePos.x,
	m_vCurrentProjectilePos.y - m_vPrevProjectilePos.y,
	m_vCurrentProjectilePos.z - m_vPrevProjectilePos.z
	};

	_float fSweepDist = sqrtf(
		vSweepDir.x * vSweepDir.x +
		vSweepDir.y * vSweepDir.y +
		vSweepDir.z * vSweepDir.z);

	PxTransform pose(PxVec3(
		m_vPrevProjectilePos.x,
		m_vPrevProjectilePos.y,
		m_vPrevProjectilePos.z));


	// 거리에 따라 Overlap 또는 Sweep 선택
	vector<PHYSX_USER_DATA*> vecHitData;
	vector<_float3> vecHitPositions; // 접촉 위치 저장용

	if (fSweepDist < 0.01f)
	{
		// 무기가 거의 안 움직였으면 Overlap
		pose.p = PxVec3(m_vCurrentProjectilePos.x, m_vCurrentProjectilePos.y, m_vCurrentProjectilePos.z);
		PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackRadius, pose);

		if (overlapResult.bHit)
		{
			vecHitData = overlapResult.vecUserData;
			vecHitPositions = overlapResult.vecHitData;
		}
	}
	else
	{
		// 궤적 Sweep
		PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
		PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, dir, fSweepDist);

		if (sweepResult.bHit)
		{
			vecHitData = sweepResult.vecUserData;
			vecHitPositions = sweepResult.vecPositions; // sweep 결과의 실제 접촉 위치 사용
		}
	}

	// 히트 처리 (공통)
	for (_uint i = 0; i < vecHitData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = vecHitData[i];

		if (pUserData == nullptr)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER && pUserData->eActorType != PX_ACTOR_TYPE::COMPANION)
			continue;
		if (pUserData->iObjectID == m_iObjectID)
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0)
			continue;
		m_setHitTargets.insert(pUserData->iObjectID);

		// 데미지 이벤트 발행
		_float3 vAttackDir = {};
		_vector playerPos = m_pGameInstance->Get_PlayerPos_Vector();
		_vector vLook = playerPos - XMLoadFloat3(&m_vPrevProjectilePos);

		if (XMVector3Equal(vLook, XMVectorZero()))
		{
			vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		}

		XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

		DAMAGE_EVENT damageEvent = {};
		damageEvent.iAttackerID = m_iObjectID;
		damageEvent.iTargetID = pUserData->iObjectID;
		damageEvent.fDamage = m_fAttackDamage;
		damageEvent.vHitPosition = vecHitPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_eProjectileDamagePower;
		damageEvent.pDamageOwner = this;

		m_bIsHit = true;

		if (m_bIsHit)
		{
			PoolingManager::Get_Instance()->Despawn_Object(this);
		}

		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
	}
}


void Client::Projectile::Process_AttackOverlap_Monster()
{
	// PxTransform규격에 맞춰 넣어주기
	PxTransform pose(PxVec3(m_vCurrentProjectilePos.x, m_vCurrentProjectilePos.y, m_vCurrentProjectilePos.z));

	vector<PHYSX_USER_DATA*> vecHitData;
	vector<_float3> vecHitPositions; // 접촉 위치 저장용

	PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(
		m_fAttackRadius, pose);
	if (overlapResult.bHit)
	{
		vecHitData = overlapResult.vecUserData;
		vecHitPositions = overlapResult.vecHitData;
	}

	// 히트 처리 (공통)
	for (_uint i = 0; i < vecHitData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = vecHitData[i];

		if (pUserData == nullptr)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER && pUserData->eActorType != PX_ACTOR_TYPE::COMPANION)
			continue;
		if (pUserData->iObjectID == m_iObjectID)
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0)
			continue;
		m_setHitTargets.insert(pUserData->iObjectID);

		// 데미지 이벤트 발행
		_float3 vAttackDir = {};
		_vector playerPos = m_pGameInstance->Get_PlayerPos_Vector();
		_vector vLook = playerPos - XMLoadFloat3(&m_vPrevProjectilePos);

		if (XMVector3Equal(vLook, XMVectorZero()))
		{
			vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		}

		XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

		DAMAGE_EVENT damageEvent = {};
		damageEvent.iAttackerID = m_iObjectID;
		damageEvent.iTargetID = pUserData->iObjectID;
		damageEvent.fDamage = m_fAttackDamage;
		damageEvent.vHitPosition = vecHitPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_eProjectileDamagePower;
		damageEvent.pDamageOwner = this;

		m_bIsHit = true;

		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
	}
}

void Client::Projectile::Process_AttackFanOverlap_Monster(_float _currentRadius, _float _halfAngle)
{
	PxTransform pose(PxVec3(m_vCurrentProjectilePos.x, m_vCurrentProjectilePos.y, m_vCurrentProjectilePos.z));

	PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(
		_currentRadius, pose);

	vector<PHYSX_USER_DATA*> vecHitData;
	vector<_float3> vecHitPositions; // 접촉 위치 저장용

	if (overlapResult.bHit == false)
		return;

	vecHitData = overlapResult.vecUserData;

	for (_uint i = 0; i < overlapResult.vecUserData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = overlapResult.vecUserData[i];
		if (pUserData == nullptr)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER && pUserData->eActorType != PX_ACTOR_TYPE::COMPANION)
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0)
			continue;

		// 부채꼴 범위 체크
		_float3 targetPos = overlapResult.vecHitData[i];
		if (!IsInsideFanArea(targetPos, _currentRadius, _halfAngle))
			continue;

		m_setHitTargets.insert(pUserData->iObjectID);

		// 데미지 이벤트 발행
		_float3 vAttackDir = {};
		_vector playerPos = m_pGameInstance->Get_PlayerPos_Vector();
		_vector vLook = playerPos - XMLoadFloat3(&m_vPrevProjectilePos);
		XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

		DAMAGE_EVENT damageEvent = {};
		damageEvent.iAttackerID = m_iObjectID;
		damageEvent.iTargetID = pUserData->iObjectID;
		damageEvent.fDamage = m_fAttackDamage;
		damageEvent.vHitPosition = overlapResult.vecHitData[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_eProjectileDamagePower;
		damageEvent.pDamageOwner = this;

		m_bIsHit = true;

		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
	}
}

void Client::Projectile::Process_AttackLaserSweep_Monster(_float _laserRadius, _float _maxDistance, _float3& _startPosition, _fvector _fireDirection)
{
	_float3 direction = {};
	XMStoreFloat3(&direction, _fireDirection);

	PxVec3 pxDir(direction.x, direction.y, direction.z);
	PxTransform sweepPose(PxVec3(_startPosition.x, _startPosition.y, _startPosition.z));

	// Capsule Multi sweep (벽 + 플레이어 동시 검출)
	PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Capsule_Multi(
		_laserRadius,
		_laserRadius * 0.5f,
		sweepPose,
		pxDir,
		_maxDistance
	);

	// STATIC_MAP 중 최소 거리 (벽 끊김 지점)
	_float fWallDistance = _maxDistance;

	for (_uint i = 0; i < (_uint)sweepResult.vecUserData.size(); ++i)
	{
		if (sweepResult.vecUserData[i] &&
			sweepResult.vecUserData[i]->eActorType == PX_ACTOR_TYPE::STATIC_MAP)
		{
			if (sweepResult.vecDistances[i] < fWallDistance)
				fWallDistance = sweepResult.vecDistances[i];
		}
	}

	// 렌더링 끝점 갱신
	_vector vEndPoint = XMLoadFloat3(&_startPosition) + _fireDirection * fWallDistance;
	XMStoreFloat3(&m_vLaserEndPos, vEndPoint);
	m_fLaserHitDistance = fWallDistance;

	//if (m_bIsHit)
	//	return;

	if (sweepResult.bHit == false)
		return;

	// 벽보다 가까운 플레이어만 데미지
	for (_uint i = 0; i < (_uint)sweepResult.vecUserData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = sweepResult.vecUserData[i];

		if (pUserData == nullptr)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::PLAYER && pUserData->eActorType != PX_ACTOR_TYPE::COMPANION)
			continue;
		if (pUserData->iObjectID == m_iObjectID)
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0)
			continue;
		if (sweepResult.vecDistances[i] >= fWallDistance)
			continue; // 추가됨 : 벽 뒤에 있으면 무시

		m_setHitTargets.insert(pUserData->iObjectID);

		//_float3 vAttackDir = {};
		//_vector playerPos = m_pGameInstance->Get_PlayerPos_Vector();
		//_vector vLook = playerPos - XMLoadFloat3(&m_vPrevProjectilePos);
		//XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

		_float3 vAttackDir = {};
		XMStoreFloat3(&vAttackDir, XMVector3Normalize(_fireDirection));

		DAMAGE_EVENT damageEvent = {};
		damageEvent.iAttackerID = m_iObjectID;
		damageEvent.iTargetID = pUserData->iObjectID;
		damageEvent.fDamage = m_fAttackDamage;
		damageEvent.vHitPosition = sweepResult.vecPositions[i];
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_eProjectileDamagePower;
		damageEvent.pDamageOwner = this;

		m_bIsHit = true;
		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
	}
}

_bool Client::Projectile::IsInsideFanArea(const _float3& targetPos, _float currentRadius, _float _halfAngle)
{
	_float3 myPos = Get_Position_Float3();
	_float3 toTarget = { targetPos.x - myPos.x, 0.f, targetPos.z - myPos.z };

	_float distance = sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);

	// 거리 체크 : 현재 퍼진 반지름 안에 있는가
	if (distance > currentRadius || distance < 0.001f)
		return false;

	// 방향 정규화
	_float3 dir = { toTarget.x / distance, 0.f, toTarget.z / distance };

	// 보스의 정면 방향 (Look 벡터)
	_float3 look = m_pTransformCom->Get_Look_Float3();
	_float lookLength = sqrtf(look.x * look.x + look.z * look.z);
	look = { look.x / lookLength, 0.f, look.z / lookLength };

	// 각도 체크 : 내적으로 사잇각 구하기
	_float dot = dir.x * look.x + dir.z * look.z;
	dot = max(-1.f, min(1.f, dot));    // acos 안전 클램프
	_float angle = acosf(dot) * (180.f / 3.14159265f);

	return angle <= _halfAngle;
}

PxCapsuleController* Client::Projectile::Create_Controller(PHYSX_CONTROLLER_DESC& _controllerDesc)
{
	PxCapsuleController* capsuleController = CAST(PxCapsuleController*)(m_pGameInstance->PhysX_Create_Controller(_controllerDesc));
	return capsuleController;
}

void Client::Projectile::Update_ProjectilePosition()
{
	m_vPrevProjectilePos = m_vCurrentProjectilePos;

	//m_vCurrentProjectilePos.x = XMVectorGetX(m_pTransformCom->Get_State(DIRECTION::POSITION));
	//m_vCurrentProjectilePos.y = XMVectorGetY(m_pTransformCom->Get_State(DIRECTION::POSITION));
	//m_vCurrentProjectilePos.z = XMVectorGetZ(m_pTransformCom->Get_State(DIRECTION::POSITION));

	_float3 pos = m_pTransformCom->Get_Position_Float3();
	m_vCurrentProjectilePos.x = pos.x;
	m_vCurrentProjectilePos.y = pos.y;
	m_vCurrentProjectilePos.z = pos.z;

	//COUT("x:" << m_vCurrentProjectilePos.x);
	//COUT("y:" << m_vCurrentProjectilePos.y);
	//COUT("z:" << m_vCurrentProjectilePos.z);

}

void Client::Projectile::Update_ProjectileBone()
{
	m_vPrevProjectilePos = m_vCurrentProjectilePos;

	_matrix worldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix realboneMatrix = XMLoadFloat4x4(m_pBoneMatrix) * worldMatrix;
	_float4x4 bone = {};
	XMStoreFloat4x4(&bone, realboneMatrix);
	
	m_vCurrentProjectilePos.x = bone._41;
	m_vCurrentProjectilePos.y = bone._42;
	m_vCurrentProjectilePos.z = bone._43;
}

void Client::Projectile::OnSpawn(void* _arg)
{
	m_bIsActive = true;
	m_bEnablePhysics = true;
	m_bIsHit = false;
	m_bAttackSweepActive = true;
	m_setHitTargets.clear();

	if (_arg == nullptr)
	{

	}
	else
	{
		PROJECTILE_DESC* pDesc = CAST(PROJECTILE_DESC*)(_arg);

		m_vDir = pDesc->vDir;
		m_fLifeTime = (pDesc->fLifeTime > 0.f) ? pDesc->fLifeTime : m_fDefaultLifeTime;
		m_fAttackDamage = pDesc->fAttackDamage;
		if (m_fAttackDamage <= 0.f)
			m_fAttackDamage = 200.f;

		m_bPiercing = pDesc->bPiercing;
		m_iMaxHitCount = pDesc->iMaxHitCount;
		m_fSpeed = pDesc->fSpeed;
		m_fAttackRadius = pDesc->fAttackRadius;
		m_eProjectileDamagePower = pDesc->eProjectileDamagePower;

		//_vector vSpawnPos = XMLoadFloat4(&pDesc->vPosition);

		//m_pGameInstance->PhysX_Enable_Controller(m_pController, ToPxExtendedVec3(vSpawnPos), PX_ACTOR_TYPE::PROJECTILE); //컨트롤러 활성화
		if (pDesc->bSetWorldPos == true)
		{
			m_pTransformCom->Set_Matrix_Float4x4(pDesc->matWorldPos);

			m_vCurrentProjectilePos.x = pDesc->matWorldPos._41;
			m_vCurrentProjectilePos.y = pDesc->matWorldPos._42;
			m_vCurrentProjectilePos.z = pDesc->matWorldPos._43;
		}
		else
		{
			_float4 spawnPos = pDesc->vPosition;
			m_pTransformCom->Set_State(DIRECTION::POSITION, spawnPos);

			m_vCurrentProjectilePos.x = spawnPos.x;
			m_vCurrentProjectilePos.y = spawnPos.y;
			m_vCurrentProjectilePos.z = spawnPos.z;
		}

		m_vDebugColor = pDesc->vDebugColor;

		//m_pController->setPosition(ToPxExtendedVec3(m_vCurrentProjectilePos));
	}

}


void Client::Projectile::OnDespawn()
{
	if (m_pFollowEffect)
	{
		POOLING->Despawn_Object(m_pFollowEffect);
	}

	m_bIsActive = false;
	m_bIsHit = true;
	m_setHitTargets.clear();
	m_bAttackSweepActive = false;
	m_bEnablePhysics = false;
	//m_pGameInstance->PhysX_Disable_Controller(m_pController); //컨트롤러 비활성화
}

void Client::Projectile::Free()
{
	if (m_pController)
	{
		m_pController = nullptr;
		//m_pGameInstance->PhysX_Remove_Actor(m_pController->getActor());
		//m_pGameInstance->PhysX_Remove_Controller(m_pController);
	}

	__super::Free();

}
