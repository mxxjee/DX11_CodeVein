#include "Client_Define.h"
#include "WolfGhost_Attack.h"
#include "WolfGhost.h"
#include "Projectile_Direct.h"
#include "Projectile_Homing.h"
#include "Projectile_WolfHomingIce.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::WolfGhost_Attack::WolfGhost_Attack()
{
}

Client::WolfGhost_Attack::WolfGhost_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Projectile(pDevice, pContext)
{
}

Client::WolfGhost_Attack::WolfGhost_Attack(const WolfGhost_Attack& original)
	: Projectile(original)
{
}

Client::WolfGhost_Attack::~WolfGhost_Attack()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost_Attack::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::WolfGhost_Attack::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"WolfGhost_Attack_" + to_wstring(namenum++);

	CHECK_FAILED(Projectile::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_fDefaultLifeTime = 5.f;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost_Attack::Ready_Components()
{

	return S_OK;
}

HRESULT Client::WolfGhost_Attack::Ready_Events()
{
	//Subscribe_Event<MONSTER_ANIM_EVENT>([this](const MONSTER_ANIM_EVENT& _event) {
	//	if (_event.iOwnerId != m_iMonsterID)
	//		return;

	//	//// 레이저 후 불 소환 패턴으로 쓸것
	//	//// 있는거 갖다가 야매로 하는거임....
	//	//if (_event.iNextState == 1000)
	//	//{
	//	//	FireBall();
	//	//}

	//	});

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::WolfGhost_Attack::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));
	
	
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::WolfGhost_Attack::Update_Parallel(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));
	
	m_fElapsedTime += fTimeDelta;

	return 0;
}

_int Client::WolfGhost_Attack::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::WolfGhost_Attack::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));

	switch (m_iAttackType)
	{
	case WolfGhost::WG_TELEPORT:
		Teleport();
		break;
	case WolfGhost::WG_BLOODBOOM:
		BloodBoom();
		break;
	case WolfGhost::WG_THUNDER:
		Thunder();
		break;
	case WolfGhost::WG_FIREBLAST:
		FireBlast();
		break;
	case WolfGhost::WG_VOIDLASER:
		VoidLaser();
		break;
	case WolfGhost::WG_SOLARBEAM:
		// 일단 뒤로 미뤄
		break;
	case WolfGhost::WG_ICEBLAST:
		IceBlast();
		break;
	case WolfGhost::WG_SOLARLASER:
		SolarLaser();
		break;
	case WolfGhost::WG_HELLFIREBOLT:
		HellFireBolt();
		break;
	case WolfGhost::WG_VOIDSPHERE:
		VoidSphere();
		break;
	case WolfGhost::WG_ICEHOMING:
		IceHoming();
		break;
	}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost_Attack::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost_Attack::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 패턴별 공격 함수 ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// 텔레포트 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::Teleport()
{
	if(m_bIsHit == false)
		Process_AttackOverlap_Monster();

#ifdef _DEBUG
	_float3 color = { 1.f, 0.1f, 0.1f };
	BoundingSphere sphere;
	sphere.Center = Get_Position_Float3();
	sphere.Radius = m_fAttackRadius;

	Add_Debug_Sphere(sphere, color);
#endif // _DEBUG
}
/******************************************************* 텔레포트 *******************************************************/


//////////////////////////////////////////////////////// 피폭발 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::BloodBoom()
{
	_float wait = 1.2f;	// 이 시간만큼 기다리기
	_float3 color = {0.8f, 0.5f, 0.f};
	_float3 fPos = Get_Position_Float3();

	// 이펙트 소환
	if(m_bEffectCreated[BB_READY] == false)
	{
		static ParticleSystem* pSystem = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_PLATE));
		if (pSystem == nullptr)
			return;

		pSystem->Set_WorldPosition(fPos);
		pSystem->OnSpawn(nullptr);

		m_bEffectCreated[BB_READY] = true;
	}

	// 공격 기다렸다가
	if (m_fElapsedTime >= wait)
	{
		// 이펙트 소환
		if (m_bEffectCreated[BB_FIRE] == false)
		{
			static ParticleSystem* pSystem = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_PILLAR));
			if (pSystem == nullptr)
				return;

			pSystem->Set_WorldPosition(fPos);
			pSystem->OnSpawn(nullptr);
			m_bEffectCreated[BB_FIRE] = true;

			m_pGameInstance->Play_Sound("AURORA_KETSUGI_BLOODBOOM", 0.4f, false);
		}

		// 플레이어 한 번 맞췄으면 또 할 필요 없으니까 맞추면 계산도 멈춤
		if(m_bIsHit == false)
			Process_AttackOverlap_Monster();

		color = _float3{ 1.f, 0.1f, 0.1f };
	}

#ifdef _DEBUG
	BoundingSphere sphere;
	sphere.Center = Get_Position_Float3();
	sphere.Radius = m_fAttackRadius;

	Add_Debug_Sphere(sphere, color);
#endif // _DEBUG
}
/******************************************************* 피폭발 *******************************************************/


//////////////////////////////////////////////////////// 번개3연타 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::Thunder()
{
	// 3번 반복
	if (m_iAttackIndex >= 3)
		return;

	_float wait = 0.8f;
	_float3 color = { 0.8f, 0.5f, 1.f };

	// 공격 기다렸다가
	if (m_iAttackActive == _UINT(ThunderPhase::READY))
	{
		if (m_bEffectCreated[m_iAttackIndex] == false)
		{
			ParticleSystem* pWarningEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_THUNDER_PLATE));
			if (pWarningEffect)
			{
				pWarningEffect->Set_WorldPosition(Get_Position_Float3());
				pWarningEffect->OnSpawn(nullptr);
			}

			m_bEffectCreated[m_iAttackIndex] = true;
		}
		// 시간 지나면 어택상태에 돌입해
		if (m_fElapsedTime >= wait)
		{
			m_fElapsedTime = 0.f;
			m_iAttackActive = _UINT(ThunderPhase::ACTIVE);

			ParticleSystem* pStrikeEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_THUNDER_PILLAR));
			if (pStrikeEffect)
			{
				pStrikeEffect->Set_WorldPosition(Get_Position_Float3());
				pStrikeEffect->OnSpawn(nullptr);
			}

			m_pGameInstance->Play_Sound("AURORA_KETSUGI_THUNDER", 0.3f, false);
		}
	}
	// 공격상태야
	else if (m_iAttackActive == _UINT(ThunderPhase::ACTIVE))
	{
		wait = 1.f;

		// 구 안에 플레이어 들어온다? 죽 여 버 려
		if(m_bIsHit == false)
			Process_AttackOverlap_Monster();

		color = { 1.0f, 0.8f, 1.f };

		// 시간 지나면 힝 없어져
		if (m_fElapsedTime >= wait)
		{
			m_fElapsedTime = 0.f;
			m_bIsHit = false;
			m_setHitTargets.clear();
			m_iAttackActive = _UINT(ThunderPhase::READY);
			_float4 pos = m_pGameInstance->Get_PlayerPos_Float4();
			Set_State(DIRECTION::POSITION, pos);
			++m_iAttackIndex;
		}
	}
	

#ifdef _DEBUG
	BoundingSphere sphere;
	sphere.Center = Get_Position_Float3();
	sphere.Radius = m_fAttackRadius;

	Add_Debug_Sphere(sphere, color);
#endif // _DEBUG
}
/******************************************************* 번개3연타 *******************************************************/


//////////////////////////////////////////////////////// 부채꼴 화염방사 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::FireBlast()
{
	_float3 color = { 0.5f, 0.8f, 1.f };
	_float wait = 0.2f;
	_float expandDuration = 0.4f;
	_float halfAngle = 90.f;
	_float currentRadius = 0.f;
		
	if (m_bEffectCreated[0] == false)
	{
		_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
		_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::RIGHT));
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vCenterPos = XMLoadFloat3(&m_vCurrentProjectilePos);

		_float fHandOffsetX = 1.5f;
		_float fHandOffsetY = 1.0f;

		_vector vRightHandPos = vCenterPos + (vRight * fHandOffsetX) + (vUp * fHandOffsetY);
		_vector vLeftHandPos = vCenterPos - (vRight * fHandOffsetX) + (vUp * fHandOffsetY);

		auto SpawnEffect = [&](_float fAngle, _vector vSpawnPos)
			{
				_matrix matRot = XMMatrixRotationY(XMConvertToRadians(fAngle));
				_vector vRotatedLook = XMVector3TransformNormal(vLook, matRot);
				_vector vActualRight = XMVector3Normalize(XMVector3Cross(vUp, vRotatedLook));
				_vector vActualUp = XMVector3Normalize(XMVector3Cross(vRotatedLook, vActualRight));

				_matrix effectWorld = XMMatrixIdentity();
				effectWorld.r[0] = vActualRight;
				effectWorld.r[1] = vActualUp;
				effectWorld.r[2] = vRotatedLook;
				effectWorld.r[3] = XMVectorSetW(vSpawnPos, 1.f);

				_float4x4 fWorld;
				XMStoreFloat4x4(&fWorld, effectWorld);

				ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_MAIN));
				if (pEffect != nullptr)
				{
					pEffect->Set_WorldMatrix(fWorld);
					pEffect->OnSpawn(nullptr);
				}
			};

		_float rightAngles[4] = { 0.f, 20.f, 40.f, 60.f };
		_float leftAngles[4] = { 0.f, -20.f, -40.f, -60.f };

		for (_uint i = 0; i < 4; ++i)
		{
			SpawnEffect(rightAngles[i], vRightHandPos);
			SpawnEffect(leftAngles[i], vLeftHandPos);
		}

		m_bEffectCreated[0] = true;
		m_pGameInstance->Play_Sound("AURORA_KETSUGI_ATTACK_FIRERANGE_MOVE", 0.5f, false);
	}

	// 시전 대기
	if (m_fElapsedTime >= wait)
	{
		if (m_bEffectCreated[1] == false)
		{
			m_pGameInstance->Play_Sound("AURORA_KETSUGI_ATTACK_FIRERANGE_BOOB", 0.6f, false);
			m_bEffectCreated[1] = true;
		}

		// 대기시간 빼고 실제 확산 시간 계산
		_float expandTime = m_fElapsedTime - wait;
		// 0 ~ 1 비율로 현재 반지름 계산
		_float ratio = min(expandTime / expandDuration, 1.f);
		currentRadius = m_fAttackRadius * ratio;

		// 부채꼴 범위 안이면 공격 판정
		if (m_bIsHit == false)
		{
			Process_AttackFanOverlap_Monster(currentRadius, halfAngle);
		}

		color = { 1.f, 0.3f, 0.3f };
	}

#ifdef _DEBUG
	_float3 look = {};
	XMStoreFloat3(&look, m_pTransformCom->Get_State(DIRECTION::LOOK));
	Add_Debug_Fan(m_vCurrentProjectilePos, look, m_fAttackRadius, halfAngle);
	Add_Debug_Fan(m_vCurrentProjectilePos, look, currentRadius, halfAngle, _float3(1.f, 0.2f, 0.1f));
#endif // _DEBUG
}
/******************************************************* 부채꼴 화염방사 *******************************************************/


//////////////////////////////////////////////////////// 암흑 광선 발싸 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::VoidLaser()
{
	if (m_bEffectCreated[0] == false)
	{
		m_pGameInstance->Play_Sound("AURORA_KETSUGI_THUNDERRASER_MAIN", 0.6f, false);
		m_pGameInstance->Play_Sound("AURORA_KETSUGI_THUNDERRASER_SPARK", 0.4f, false);
		m_bEffectCreated[0] = true;
	}
	_float3 color = { 0.4f, 0.1f, 0.5f };

	// 쏴야 할 레이저 방향을 구한다 (본 위치 -> 보스 정면, Y 제거)
	_vector position = m_pTransformCom->Get_Position_Vector();			// 몬스터의 위치
	_vector bonePosition = XMLoadFloat3(&m_vCurrentProjectilePos);		// 빔이 나오는 본 위치
	_vector fireDirection = XMVectorSetY(bonePosition - position, 0.f);	// 빔 발싸 방향
	fireDirection = XMVector3Normalize(fireDirection);
	
	Process_AttackLaserSweep_Monster(m_fAttackRadius, 50.f, m_vCurrentProjectilePos, fireDirection);

#ifdef _DEBUG
	Add_Debug_Capsule(m_vCurrentProjectilePos, m_vLaserEndPos, m_fAttackRadius, color);
#endif // _DEBUG
}
/******************************************************* 암흑 광선 발싸 *******************************************************/


//////////////////////////////////////////////////////// 얼음 방사 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::IceBlast()
{
	// 5번 발사
	if (m_iAttackIndex >= 5)
		return;

	if (m_fElapsedTime <= 0.3f)
		return;

	_vector bonePosition = XMLoadFloat3(&m_vCurrentProjectilePos);	// 얼음이 나올 위치
	_vector playerpostion = m_pGameInstance->Get_PlayerPos_Vector();	// 플레이어 위치
	_vector fireDirection = playerpostion - bonePosition + XMVectorSet(0.f, 0.5f, 0.f, 0.f);			// 얼음이 발사될 방향
	static Projectile_Direct::PROJECTILE_DESC desc;

	// POOL에서 꺼내올 description 생성
	if(m_bIceBlastCached == false)
	{
		desc.vPosition.w = 1.f;													// 위치 w값
		desc.fAttackRadius = m_fAttackRadius;									// 크기
		desc.fSpeed = 25.f;														// 속도
		desc.fAttackDamage = m_fAttackDamage;									// 데미지
		desc.fLifeTime = 3.f;
		desc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
		desc.vDebugColor = _float3(0.7f, 0.7f, 0.9f);
		desc.eSpanwEffectID = POOL_ID::EFFECT_WOLF_ICE_BULLET;
		desc.eDeSpanwEffectID = POOL_ID::EFFECT_WOLF_ICE_HOMING_HIT;

		desc.strSpawnSound = "AURORA_KETSUGI_ICEBULLET_MOVE";
		desc.strDespawnSound = "AURORA_KETSUGI_ICEBULLET_HITPLAYER";

		m_bIceBlastCached = true;
	}

	XMStoreFloat3(&desc.vDir, fireDirection);								// 방향
	memcpy(&desc.vPosition, &m_vCurrentProjectilePos, sizeof(_float3));		// 위치

	// 0도 (정면) 발사
	POOLING->Acquire(POOL_ID::PROJECTILE_MONSTER_DIRECT, &desc);

	// 나머지 4갈래 발사
	static constexpr _float angles[4] = { -50.f, -25.f, 25.f, 50.f };

	for (_uint i = 0; i < 4; ++i)
	{
		_float radian = XMConvertToRadians(angles[i]);
		_matrix rotationMatrix = XMMatrixRotationY(radian);
		_vector rotatedDirection = XMVector3TransformNormal(fireDirection, rotationMatrix);

		// description에서 방향만 바꿔서 발사
		XMStoreFloat3(&desc.vDir, rotatedDirection);

		POOLING->Acquire(POOL_ID::PROJECTILE_MONSTER_DIRECT, &desc);
	}

	m_fElapsedTime = 0.f;
	++m_iAttackIndex;
}
/******************************************************* 얼음 방사 *******************************************************/


//////////////////////////////////////////////////////// 불꽃빔 + 불꽃히히발싸 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::SolarLaser()
{
	// 레이저 후 불꽃2번(총 3번)
	// 이었는데 불꽃 발싸는 그냥 보스가 직접 하는걸로
	if (m_iAttackIndex >= 1)
		return;
	
	// 레이저 패턴
	else if(m_iAttackIndex == 0)
	{
		if (m_bEffectCreated[0] == false)
		{
			m_pGameInstance->Play_Sound("AURORA_KETSUGI_FIRERASER_MOVE", 0.5f, false);
			m_bEffectCreated[0] = true;
		}
		_float3 color = { 0.9f, 0.5f, 0.15f };

		// 쏴야 할 레이저 방향을 구한다 (본 위치 -> 보스 정면, Y 제거)
		_vector position = m_pTransformCom->Get_Position_Vector();			// 몬스터의 위치
		_vector bonePosition = XMLoadFloat3(&m_vCurrentProjectilePos);		// 빔이 나오는 본 위치
		_vector fireDirection = XMVectorSetY(bonePosition - position, 0.f);	// 빔 발싸 방향
		fireDirection = XMVector3Normalize(fireDirection);

		m_vCurrentProjectilePos.y += 0.8f;
		Process_AttackLaserSweep_Monster(m_fAttackRadius, 50.f, m_vCurrentProjectilePos, fireDirection);

#ifdef _DEBUG
		Add_Debug_Capsule(m_vCurrentProjectilePos, m_vLaserEndPos, m_fAttackRadius, color);
#endif // _DEBUG

		// 2초동안 발싸
		if (m_fElapsedTime >= 2.f)
		{
			m_fElapsedTime = 0.f;
			++m_iAttackIndex;
		}
		return;
	}
}
/******************************************************* 불꽃빔 + 불꽃히히발싸 *******************************************************/


//////////////////////////////////////////////////////// 불덩어리 발싸 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::HellFireBolt()
{
	// 3번 발싸
	if (m_iAttackIndex >= 3)
		return;

	if (m_fElapsedTime <= 0.15f)
		return;

	m_fElapsedTime = 0.f;
	++m_iAttackIndex;

	static _float lookOffset = 9.f;

	_matrix worldmatrix = Get_WorldMatrix();
	_vector startPosition = worldmatrix.r[3] + worldmatrix.r[2] * lookOffset + XMVectorSet(0.f, 1.2f, 0.f, 0.f);		// 불덩어리가 나올 위치
	_vector playerpostion = m_pGameInstance->Get_PlayerPos_Vector();	// 플레이어 위치
	_vector fireDirection = playerpostion - startPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f);			// 불덩어리가 발사될 방향

	ParticleSystem* pShokeEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HAND_SHOKE));
	if (pShokeEffect != nullptr)
	{
		_float3 vEffectPos;
		XMStoreFloat3(&vEffectPos, startPosition);
		pShokeEffect->Set_WorldPosition(vEffectPos);
		pShokeEffect->OnSpawn(nullptr);
	}

	static Projectile_Direct::PROJECTILE_DESC hellFireBoltdesc;

	// POOL에서 꺼내올 description 생성
	if (m_bFireBallCached == false)
	{
		hellFireBoltdesc.vPosition.w = 1.f;													// 위치 w값
		hellFireBoltdesc.fAttackRadius = m_fAttackRadius;									// 크기
		hellFireBoltdesc.fSpeed = 20.f;														// 속도
		hellFireBoltdesc.fAttackDamage = m_fAttackDamage;									// 데미지
		hellFireBoltdesc.fLifeTime = 2.f;
		hellFireBoltdesc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
		hellFireBoltdesc.vDebugColor = _float3(0.9f, 0.3f, 0.2f);
		hellFireBoltdesc.eSpanwEffectID = POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET;
		hellFireBoltdesc.eDeSpanwEffectID = POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HIT;

		hellFireBoltdesc.strSpawnSound = "AURORA_KETSUGI_FIREBULLET_MOVE";
		hellFireBoltdesc.strDespawnSound = "AURORA_KETSUGI_FIREBULLET_HIT";

		m_bFireBallCached = true;
	}

	XMStoreFloat3(&hellFireBoltdesc.vDir, fireDirection);		// 방향
	XMStoreFloat4(&hellFireBoltdesc.vPosition, startPosition);	// 위치

	POOLING->Acquire(POOL_ID::PROJECTILE_MONSTER_DIRECT, &hellFireBoltdesc);
}
/******************************************************* 불덩어리 발싸 *******************************************************/


//////////////////////////////////////////////////////// 플레이어 쫒아오는 유도 구체 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::VoidSphere()
{
	if (m_iAttackActive >= 4)
		return;

	static _float lookOffset = 10.f;
	static _float lookMoveRange = 0.f;//1.5f;
	static _float rightOffset_Small = 1.5f;
	static _float rightOffset_Large = 3.5f;
	static _float upOffset_Small = 1.f;
	static _float upOffset_Large = 2.5f;

	switch (m_iAttackIndex)
	{
		// 구체 발사 준비
	case _UINT(VoidSpherePhase::READY):
	{
		_matrix monsterMatrix = Get_WorldMatrix();
		_vector monsterRight = XMVector3Normalize(monsterMatrix.r[0]);	// 몬스터의 Right
		static _vector monsterUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector monsterLook = XMVector3Normalize(monsterMatrix.r[2]);	// 몬스터의 Look
		_vector monsterPos = monsterMatrix.r[3];	// 몬스터의 transform위치

		{	// (늑대 기준) 왼쪽 위
			_vector targetPos = monsterPos - (monsterRight * rightOffset_Large) + (monsterUp * upOffset_Large) + (monsterLook * lookOffset);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Start[0], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Current[0], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_End[0], targetPos + (monsterLook * lookMoveRange));
		}
		{	// (늑대 기준) 왼쪽 아래
			_vector targetPos = monsterPos - (monsterRight * rightOffset_Small) + (monsterUp * upOffset_Small) + (monsterLook * lookOffset);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Start[1], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Current[1], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_End[1], targetPos + (monsterLook * lookMoveRange));
		}
		{	// (늑대 기준) 오른쪽 아래
			_vector targetPos = monsterPos + (monsterRight * rightOffset_Small) + (monsterUp * upOffset_Small) + (monsterLook * lookOffset);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Start[2], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Current[2], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_End[2], targetPos + (monsterLook * lookMoveRange));
		}
		{	// (늑대 기준) 오른쪽 위
			_vector targetPos = monsterPos + (monsterRight * rightOffset_Large) + (monsterUp * upOffset_Large) + (monsterLook * lookOffset);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Start[3], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Current[3], targetPos);
			XMStoreFloat3(&m_vVoidSphereSpawnPoint_End[3], targetPos + (monsterLook * lookMoveRange));
		}

		for (_uint i = 0; i < 4; ++i)
		{
			m_pGateEffects[i] = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE));
			if (m_pGateEffects[i] != nullptr)
			{
				m_pGateEffects[i]->Set_WorldPosition(m_vVoidSphereSpawnPoint_Start[i]);
				m_pGateEffects[i]->OnSpawn(nullptr);

				m_pGateEffects[i]->Set_Dead(false);
				m_pGateEffects[i]->Set_Active(true);
				m_pGateEffects[i]->Set_Visible(true);

			}
		}

		m_iAttackIndex = _UINT(VoidSpherePhase::DELAY);
		break;
	}

	case _UINT(VoidSpherePhase::DELAY):
	{
		// 목표 시간까지 보간
		_float ratio = m_fElapsedTime / 1.f;

		// 스폰 포인트를 보간시키면서 이동
		for (_uint i = 0; i < 4; ++i)
		{
			_vector start = XMLoadFloat3(&m_vVoidSphereSpawnPoint_Start[i]);
			_vector end = XMLoadFloat3(&m_vVoidSphereSpawnPoint_End[i]);

			XMStoreFloat3(&m_vVoidSphereSpawnPoint_Current[i], XMVectorLerp(start, end, ratio));

			if (m_pGateEffects[i] != nullptr)
			{
				m_pGateEffects[i]->Set_WorldPosition(m_vVoidSphereSpawnPoint_Current[i]);
			}

		}

		// 목표 시간을 넘겼으면
		if (ratio >= 1.f)
		{
			m_fElapsedTime = 1.f;
			m_iAttackIndex = _UINT(VoidSpherePhase::FIRE);
		}
		break;
	}

	case _UINT(VoidSpherePhase::FIRE):
	{
		// 2초마다 발싸
		if (m_fElapsedTime >= 2.f)
		{
			_vector bonePosition = XMLoadFloat3(&m_vVoidSphereSpawnPoint_End[m_iAttackActive]);	// 구체가 나올 위치
			_vector playerPosition = m_pGameInstance->Get_PlayerPos_Vector();	// 초기 발사각용 플레이어 위치
			_vector fireDirection = playerPosition - bonePosition + XMVectorSet(0.f, 1.f, 0.f, 0.f); // 구체가 발사될 방향

			ParticleSystem* pBlink = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE_BLINK));
			if (pBlink != nullptr)
			{
				pBlink->Set_WorldPosition(m_vVoidSphereSpawnPoint_End[m_iAttackActive]);
				pBlink->OnSpawn(nullptr);
			}

			static Projectile_Homing::HOMING_DESC voidSphereDesc;
			if (m_bVoidSphereCached == false)
			{
				// Projectile 기본 필수값
				voidSphereDesc.vPosition.w = 1.f;
				voidSphereDesc.fAttackRadius = 1.1f;
				voidSphereDesc.fSpeed = 3.f;
				voidSphereDesc.fAttackDamage = 100.f;// 243.f;
				voidSphereDesc.fLifeTime = 10.f;
				voidSphereDesc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
				voidSphereDesc.bPiercing = false;
				voidSphereDesc.iMaxHitCount = 1;
				voidSphereDesc.vDebugColor = _float3(0.6f, 0.3f, 0.6f);
				voidSphereDesc.eSpanwEffectID = POOL_ID::EFFECT_WOLF_THUNDER_BULLET;
				voidSphereDesc.eDeSpanwEffectID = POOL_ID::EFFECT_WOLF_THUNDER_BULLET_HIT;
				// 유도 필수값
				voidSphereDesc.fHomingTurnSpeed = 60.f;	// 초당 회전 각도
				voidSphereDesc.fHomingDelay = 0.0f;		// 유도 시작 시간
				voidSphereDesc.fHomingDuration = 999.f;	// 수명 끝까지 유도
				voidSphereDesc.fAcceleration = 0.f;		// 가속도
				voidSphereDesc.fMaxSpeed = 0.f;			// 최고 속도(가속도 기반일때) (0이면 무제한)
				voidSphereDesc.fMinSpeed = 0.f;			// 최저 속도(가속도 기반할때)
				voidSphereDesc.fHomingLosAngle = 360.f;	// 유도 범위(시야각 느낌)
				voidSphereDesc.fArrivalRadius = 0.f;		// 유도를 멈출 범위
				voidSphereDesc.bPredictTarget = false;	// 이동 경로 예측(아직 안 만듬)
				voidSphereDesc.vTargetOffset = { 0.f, 1.13f, 0.f }; // 플레이어 몸통 위치 보정

				voidSphereDesc.strSpawnSound = "AURORA_KETSUGI_THUNDER_SPHERE_BULLETMOVE";
				voidSphereDesc.strDespawnSound = "AURORA_KETSUGI_THUNDER_SPHERE_BULLETHIT";

				m_bVoidSphereCached = true;
			}

			// 매 발사마다 갱신
			XMStoreFloat3(&voidSphereDesc.vDir, fireDirection);
			XMStoreFloat4(&voidSphereDesc.vPosition, XMVectorSetW(bonePosition, 1.f)
			);

			POOLING->Acquire(POOL_ID::PROJECTILE_MONSTER_HOMING, &voidSphereDesc);

			if (m_iAttackActive == 3)
			{
				for (_uint i = 0; i < 4; ++i)
				{
					if (m_pGateEffects[i] != nullptr)
					{
						ParticleSystem* pGateEndEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE_END));
						if (pGateEndEffect != nullptr)
						{
							pGateEndEffect->Set_WorldPosition(m_vVoidSphereSpawnPoint_End[i]);
							pGateEndEffect->OnSpawn(nullptr);
						}

						m_pGateEffects[i]->Stop();
						m_pGateEffects[i] = nullptr;
					}
				}
			}

			++m_iAttackActive;
			m_fElapsedTime = 0.f;
			//m_pGateEffects[m_iAttackActive] = nullptr;
		}

		break;
	}
	}

#ifdef _DEBUG
	_float3 color = { 0.8f, 0.5f, 0.8f };
	for (_uint i = 0; i < 4; ++i)
	{
		BoundingSphere sphere;
		sphere.Center = m_vVoidSphereSpawnPoint_Current[i];
		sphere.Radius = m_fAttackRadius;

		Add_Debug_Sphere(sphere, color);
	}
#endif // _DEBUG
}
/******************************************************* 플레이어 쫒아오는 유도 구체 *******************************************************/


//////////////////////////////////////////////////////// 반 유도 얼음덩어리 3개 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::IceHoming()
{
	// IceHoming패턴은 투사체 3개를 생성하는 중간 다리 역할만 함
	_matrix transform = m_pTransformCom->Get_WorldMatrix();	// 얼음이 나올 위치(WolfGhost가 처음 생성한 위치 = WolfGhost의 Transform 위치)
	_vector look = transform.r[2];
	_vector position = transform.r[3];

	_float4x4 asdf = m_pTransformCom->Get_WorldFloat4x4();
	
	_vector projectileStartPosition = position + XMVectorSet(0.f, 1.2f, 0.f, 0.f) + look * 10.f;			// 얼음이 처음 생성될 위치
	_vector fireDirection = look;			// 얼음이 발사될 방향

	static Projectile_WolfHomingIce::PROJECTILE_DESC desc;

	// POOL에서 꺼내올 description 생성
	if (m_bIceHomingCached == false)
	{
		desc.vPosition.w = 1.f;													// 위치 w값
		desc.fAttackRadius = m_fAttackRadius;									// 크기
		desc.fSpeed = 30.f;														// 속도
		desc.fAttackDamage = m_fAttackDamage;									// 데미지
		desc.fLifeTime = 4.f;
		desc.eProjectileDamagePower = m_eProjectileDamagePower;
		desc.vDebugColor = _float3(0.5f, 0.5f, 0.9f);
		desc.eSpanwEffectID = POOL_ID::EFFECT_WOLF_ICE_HOMING_BULLET;
		desc.eDeSpanwEffectID = POOL_ID::EFFECT_WOLF_ICE_HOMING_HIT;

		desc.strSpawnSound = "AURORA_KETSUGI_ICEBULLET_MOVE";
		desc.strDespawnSound = "AURORA_KETSUGI_ICEBULLET_HITPLAYER";

		m_bIceHomingCached = true;
	}

	XMStoreFloat3(&desc.vDir, fireDirection);								// 방향
	XMStoreFloat4(&desc.vPosition, projectileStartPosition);
	//memcpy(&desc.vPosition, &projectileStartPosition, sizeof(_float3));		// 위치

	// 하나는 0도 (정면) 발사
	POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_ICE, &desc);

	// 나머지 2갈래 발사
	static constexpr _float angles[2] = { -30.f, 30.f };

	for (_uint i = 0; i < 2; ++i)
	{
		_float radian = XMConvertToRadians(angles[i]);
		_matrix rotationMatrix = XMMatrixRotationY(radian);
		_vector rotatedDirection = XMVector3TransformNormal(fireDirection, rotationMatrix);

		// description에서 방향만 바꿔서 발사
		XMStoreFloat3(&desc.vDir, rotatedDirection);

		POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_ICE, &desc);
	}

	POOLING->Despawn_Object(this);
}
/******************************************************* 반 유도 얼음덩어리 3개 *******************************************************/
/******************************************************* 패턴별 공격 함수 *******************************************************/



//////////////////////////////////////////////////////// 풀링 함수 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::OnSpawn(void* _arg)
{
	__super::OnSpawn(_arg);

	WolfGhostAttackDesc* desc = CAST(WolfGhostAttackDesc*)(_arg);

	m_fElapsedTime = 0.f;
	m_iAttackActive = 0;
	m_iAttackIndex = 0;

	m_fElapsedTime += desc->fStartUpElapseTime;

	// 이 어택시스템이 어떤 공격을 할 것인지 여기서 결정
	m_iAttackType = desc->iAttackType;
	m_fAttackRadius = desc->fRadius;	//  공격 범위
	m_iMonsterID = desc->iMonsterID;

	if (desc->pBoneMatrix != nullptr)
	{
		m_pBoneMatrix = desc->pBoneMatrix;
	}
	else
	{
		m_pBoneMatrix = nullptr;
	}

	for (_uint i = 0; i < sizeof(m_bEffectCreated) / sizeof(_bool); ++i)
	{
		m_bEffectCreated[i] = false;
	}
}

void Client::WolfGhost_Attack::OnDespawn()
{
	__super::OnDespawn();

}
/******************************************************* 풀링 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
WolfGhost_Attack* Client::WolfGhost_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	WolfGhost_Attack* pInstance = new WolfGhost_Attack(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"WolfGhost_Attack 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::WolfGhost_Attack::Clone(void* pArg)
{
	WolfGhost_Attack* pInstance = new WolfGhost_Attack(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"WolfGhost_Attack 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::WolfGhost_Attack::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
