#include "Client_Define.h"
#include "WolfGhost.h"
#include "WolfGhost_Attack.h"

//#include "GameInstance.h"
#include "PhysX_Function.h"
#include "Projectile_Homing.h"
#include "UIObj_BossName.h"
#include "UIObj_HpBar.h"
#include "UIObj_BossDead.h"

#include "ParticleSystem.h"
#include "MeshEffect.h"
#include "MState_TransitionTable.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::WolfGhost::WolfGhost()
{
}

Client::WolfGhost::WolfGhost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: BossBase(pDevice, pContext)
{
}

Client::WolfGhost::WolfGhost(const WolfGhost& original)
	: BossBase(original)
{
}

Client::WolfGhost::~WolfGhost()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::WolfGhost::Initialize(void* arg)
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

	CHECK_FAILED(Ready_Controller(), E_FAIL);

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"WolfGhost_" + to_wstring(namenum++);

	CHECK_FAILED(BossBase::Initialize(arg), E_FAIL);

	//m_pTransformCom;

	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);
	CHECK_FAILED(Ready_Event(), E_FAIL);
	CHECK_FAILED(Ready_States(), E_FAIL);
	CHECK_FAILED(Ready_Value(), E_FAIL);
	CHECK_FAILED(Ready_Sockets(), E_FAIL);

	//POOLING->Register(POOL_ID::PROJECTILE_WOLFGHOST_ATTACK, 3, m_iLevel, );

	// 컨트롤러 위치 맞춰주기
	m_pController->setFootPosition(ToPxExtendedVec3(Get_Position()));
	m_pGameInstance->PhysX_Disable_Controller(m_pController); //컨트롤러 비활성화

	m_bLastBoss = true;
	
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost::Ready_Controller()
{
	__super::Ready_Components();

	// 피직스 세팅
	PHYSX_CONTROLLER_DESC controllerdesc;
	controllerdesc.fRadius = 3.3f;
	controllerdesc.fHeight = 1.f;
	controllerdesc.fSlopeLimit = 50.f;
	controllerdesc.eActorType = PHYSX_ACTOR_TYPE::MONSTER;
	controllerdesc.iObjectID = m_iObjectID;
	controllerdesc.pOwner = this;

	// 피직스 컨트롤러 만들기
	m_pController = Create_Controller(controllerdesc);

	return S_OK;
}

HRESULT Client::WolfGhost::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Com_Shader_VTXAnimMesh), E_FAIL);	// 쉐이더
	CHECK_FAILED(Add_Model(Proto_Model(L"WolfGhost")), E_FAIL);		// 모델
	CHECK_FAILED(Add_StateMachine(), E_FAIL);						// 상태 머신
	CHECK_FAILED(Add_Minimap_Monster(), E_FAIL);					// 미니맵
	
	m_pModelCom->Set_Animation_CS(0, true);
	m_pModelCom->Play_Animation_CS(0.2f);
	m_pModelCom->Set_Animation_CS(1, true);


	
	return S_OK;
}

HRESULT Client::WolfGhost::Ready_PhysXEvent()
{
	// 충돌처리 해야되니까 모델에 오브젝트 ID 등록
	m_pModelCom->Set_OwnerId(m_iObjectID);

	// 공격 이벤트 구독(애니메이션이 쏴줌)
	// 얘는 내가봤을때 좀 다름
	/*m_iColliderEventHandle = GameObject::Subscribe_Event<COLLIDER_EVENT>(
		[this](const COLLIDER_EVENT& _event)
		{
			if (_event.iOwnerId != m_iObjectID)
				return;

			switch (_event.ePhase)
			{
			case ANIM_FRAMEPHASE::START:
				m_bAttackSweepActive = true;
				m_fAttackRadius = _event.fAttackRadius;
				m_fAttackDamage = _event.fAttackDamage;
				m_fAttackHalfHeight = _event.fAttackHalfHeight;
				m_setHitTargets.clear();
				m_vPrevWeaponPos = m_vCurrentWeaponPos;
				break;

			case ANIM_FRAMEPHASE::UPDATE:
				break;

			case ANIM_FRAMEPHASE::END:
				m_bAttackSweepActive = false;
				m_setHitTargets.clear();
				break;
			}
		});*/

	// 데미지 받는 이벤트
	Subscribe_Event<DAMAGE_EVENT>([this](DAMAGE_EVENT _event) {
		if (_event.iTargetID != m_iObjectID)
			return;

		Monster::OnDamaged(_event);
		});

	return S_OK;
}

HRESULT Client::WolfGhost::Ready_Event()
{
	// 몬스터 애니메이션 이벤트 구독
	Subscribe_Event<MONSTER_ANIM_EVENT>([this](const MONSTER_ANIM_EVENT& _event) {
		// 내꺼인지 체크
		if (_event.iOwnerId != m_iObjectID)
			return;

		// 흠 아직 기능이 없네
		switch (_event.ePhase)
		{
		case ANIM_FRAMEPHASE::START:
			if (_event.fMaxMoveDistance >= 1.f)
			{
				m_bFlame = true;
				return;
			}

			Spawn_Attack();
			break;

		case ANIM_FRAMEPHASE::UPDATE:
			break;

		case ANIM_FRAMEPHASE::END:
			break;
		}

		});

	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return E_FAIL;

		ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire_Effect(e.ePoolId));
		if (pSystem == nullptr)
			return E_FAIL;

		// 뼈 행렬 계산 람다
		auto fnCalcBoneMatrix = [this, poolID = e.ePoolId, socketName = e.SocketName, bAttached = e.bAttached, 
								 bOnlyPosition = e.bOnlyPosition, bUseOwnerRotation = e.bOwnerRotation]() -> _float4x4
			{
				_float4x4 matFinalPos = {};
				const _float4x4* pMatBone = { nullptr };

				if (bAttached)
				{
					// Wolf는 무기가 없다
				}
				else
				{
					// 몬스터 뼈
					pMatBone = m_pModelCom->Get_SocketBoneMatrixPtr_Index(socketName);
					if (pMatBone != nullptr)
					{
						_matrix matBone = XMLoadFloat4x4(pMatBone);
						_matrix matWorld = matBone * m_pTransformCom->Get_WorldMatrix();

						// 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
						matWorld.r[0] = XMVector3Normalize(matWorld.r[0]);
						matWorld.r[1] = XMVector3Normalize(matWorld.r[1]);
						matWorld.r[2] = XMVector3Normalize(matWorld.r[2]);

						if (bUseOwnerRotation)
						{
							// 소켓에서 위치만 가져오고, 방향은 Owner거 가ㅇ굦이
							_float3 vBonePos{};
							vBonePos.x = XMVectorGetX(matWorld.r[3]);
							vBonePos.y = XMVectorGetY(matWorld.r[3]);
							vBonePos.z = XMVectorGetZ(matWorld.r[3]);

							const _float4x4* pMatRoot{ nullptr };

							if (poolID == POOL_ID::EFFECT_WOLF_VOIDLASER)
							{
								pMatRoot = m_pModelCom->Get_SocketBoneMatrixPtr_Index("CHARA_OFFSET");
								if (pMatRoot != nullptr)
								{
									_matrix matRoot = XMLoadFloat4x4(pMatRoot) * m_pTransformCom->Get_WorldMatrix();
									matRoot.r[0] = XMVector3Normalize(matRoot.r[0]);
									matRoot.r[1] = XMVector3Normalize(matRoot.r[1]);
									matRoot.r[2] = XMVector3Normalize(matRoot.r[2]);
									matRoot.r[3] = XMVectorSet(vBonePos.x, vBonePos.y, vBonePos.z, 1.f);

									XMStoreFloat4x4(&matFinalPos, matRoot);
								}
							}
							else if (poolID == POOL_ID::EFFECT_WOLF_SOLARLASER ||
									 poolID == POOL_ID::EFFECT_WOLF_SOLARLASER_PARTICLE)
							{
								pMatRoot = m_pModelCom->Get_SocketBoneMatrixPtr_Index("CHARA_OFFSET");
								_matrix matRoot = XMLoadFloat4x4(pMatRoot) * m_pTransformCom->Get_WorldMatrix();

								// CHARA_OFFSET 월드 위치 (y는 0으로)
								_vector vRootPos = matRoot.r[3];
								vRootPos = XMVectorSetY(vRootPos, 0.f);

								// 방향 = 본 위치 - CHARA_OFFSET 위치
								_vector vBonePosVec = XMVectorSet(vBonePos.x, 0.f, vBonePos.z, 1.f);
								_vector vLook = XMVector3Normalize(vRootPos - vBonePosVec);

								// Look 방향으로 부터 Right, Up 구하기
								_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
								_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
								vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

								_matrix matResult = XMMatrixIdentity();
								matResult.r[0] = vRight;
								matResult.r[1] = vUp;
								matResult.r[2] = vLook;
								matResult.r[3] = XMVectorSet(vBonePos.x, vBonePos.y, vBonePos.z, 1.f);

								XMStoreFloat4x4(&matFinalPos, matResult);
							}
						}
						else if (!bOnlyPosition)
							XMStoreFloat4x4(&matFinalPos, matWorld);
						else
						{
							_float3 vPos;
							vPos.x = XMVectorGetX(matWorld.r[3]);
							vPos.y = XMVectorGetY(matWorld.r[3]);
							vPos.z = XMVectorGetZ(matWorld.r[3]);
							XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vPos.x, vPos.y, vPos.z));
						}
					}
				}

				return matFinalPos;
			};

		// 초기 위치 설정
		_float4x4 matInitial = fnCalcBoneMatrix();
		pSystem->Set_WorldMatrix(matInitial);

		pSystem->Play();

		// VoidLaser, SolarLaser 이펙트 줄여보자
		if ((e.ePoolId == POOL_ID::EFFECT_WOLF_VOIDLASER) ||
			(e.ePoolId == POOL_ID::EFFECT_WOLF_SOLARLASER) ||
			(e.ePoolId == POOL_ID::EFFECT_WOLF_SOLARLASER_PARTICLE))
		{
			LASERSYSTEM_CACHE cache;
			cache.pSystem = pSystem;

			for (_uint i = 0; i < pSystem->Get_EffectCount(); i++)
			{
				ParticleEffect* pEffect = pSystem->Find_EffectByIndex(i);
				if (pEffect == nullptr)
					break;

				if (pEffect->Get_IsMeshEffect())		// MeshEffect의 경우
				{
					_float fOriginalScale = CAST(MeshEffect*)(pEffect)->Get_MeshDesc().vScale.x;		// 메쉬 이펙트의 원본 스케일을 구해서
					cache.vecMeshScales.push_back({ i, fOriginalScale });								// 컨테이너에 인덱스와 값을 넣어준다.
				}
			}

			m_vecLaserSystems.push_back(cache);			// 원본 스케일을 담고 있는 정보를 넣어준다.

			if (e.bFollow)
			{
				pSystem->Set_FollowCallback([this, fnCalcBoneMatrix]() -> _float4x4
				{
					_float4x4 matFinalPos = fnCalcBoneMatrix();
					_matrix matResult = XMLoadFloat4x4(&matFinalPos);
					matResult.r[2] *= m_fLaserRatio;
					XMStoreFloat4x4(&matFinalPos, matResult);
					return matFinalPos;
				});
			}
		}		
		// Follow 면 콜백에 등록
		else if (e.bFollow)
			pSystem->Set_FollowCallback(fnCalcBoneMatrix);

		return S_OK;
		});


		// Dissolve 이벤트 구독
		Subscribe_Event<DISSOLVE_EVENT>([this](const DISSOLVE_EVENT& e) {
			if (e.iOwnerId != m_iObjectID)
				return E_FAIL;

			if (e.bIsWeaponTarget)
				return S_OK;

			m_bDissolving = true;

			if (e.bDissolve)
			{
				// 사라지기
				m_fDissolveDir = 1.f;
				m_fTimeElapsed = 0.f;
			}
			else
			{
				// 나타나기
				m_fDissolveDir = -1.f;
				m_fTimeElapsed = m_fDissolveMax;
			}
			});

	return S_OK;
}

HRESULT Client::WolfGhost::Ready_States()
{
	CHECK_FAILED(Add_State(MS_IDLE, WG_IDLE), E_FAIL);
	CHECK_FAILED(Add_State(MS_AWAKE, WG_AWAKE), E_FAIL);
	CHECK_FAILED(Add_State(MS_BATTLE, 0), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKSPECIAL0, WG_TELEPORT), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKMELEE0, WG_VOIDLASER), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKMELEE1, WG_BLOODBOOM), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKMELEE2, WG_SOLARBEAM), E_FAIL);		// 미완성(아마 안 할듯)
	CHECK_FAILED(Add_State(MS_ATTACKMELEE3, WG_ICEBLAST), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKMELEE4, WG_SOLARLASER), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKFAR0, WG_HELLFIREBOLT), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKFAR1, WG_FIREBLAST), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKFAR2, WG_VOIDSPHERE), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKFAR3, WG_THUNDER), E_FAIL);
	CHECK_FAILED(Add_State(MS_ATTACKFAR4, WG_ICEHOMING), E_FAIL);
	CHECK_FAILED(Add_State(MS_PHASECHANGE, WG_PHASECHANGE), E_FAIL);
	CHECK_FAILED(Add_State(MS_DEAD, WG_DEAD), E_FAIL);

	m_pStateMachineCom->Set_State(_UINT(MS_IDLE));

	m_eIdleType = ENEMY_IDLE_TYPE::SLEEP;

	CHECK_FAILED(Load_TransitionTable(L"../../DataFiles/Monster_StateTable/WolfGhost_Phase01.jsonc"), E_FAIL);
	m_pPhase2Table = MState_TransitionTable::Create(L"../../DataFiles/Monster_StateTable/WolfGhost_Phase02.jsonc");


	return S_OK;
}

HRESULT Client::WolfGhost::Ready_Value()
{
	m_bIsSquared = false;
	m_bEnablePhysics = false;
	m_bLifted = true;
	m_bCanAttack = false;
	m_fAttackTimer = 0.f;

	m_fMeshXOffset = 2.f;
	
	/* 스탯 */
#ifdef _DEBUG
	m_fMaxHp = 3000.f;;
#else
	m_fMaxHp = 20000.f;
#endif
	m_fCurrentHp = m_fMaxHp;

	/* 이동 및 회전 속도 */
	m_fWalkSpeed = 1.f;             // 걷기 최대 속도
	m_fWalkAcceleration = 2.f;      // 걷기 가속도
	m_fRunSpeed = 5.f;              // 달리기 최대 속도
	m_fRunAcceleration = 9.f;       // 달리기 가속도
	m_fNormalRotationSpeed = 1.f;  // RotateSpeed 평상시
	m_fChaseRotationSpeed = 3.f;    // RotateSpeed 추격시
	m_fTurnSpeed = 0.f;             // 턴 인터럽트 시 회전 속도
	m_fTurnTimeLimit = 0.f;         // 턴 인터럽트 최대 허용 시간
	m_eTurnType = TURN_TYPE::TURN_END; // 플레이어쪽으로 방향 전환 타입
	/* 얘는 그런거 없어 그냥 복붙해둔거임 */

	/* 거리 및 범위 */
	m_fWalkRange = 15.f;            // 어디쯤부터 간 보면서 걸을지
	m_fDetectRange = 50.0f;         // IDLE상태일때 어디까지 탐지할지
	m_fChaseRange = 44.0f;          // BATTLE상태일 때 어디까지 쫒아갈지
	m_fAttackMeleeRange = 1.5f;      // 근접공격 시작 범위
	m_fAttackFarRange = 100.f;        // 원거리공격 범위(0이면 없음)
	m_fAttackDuration = 15.f;		// 공격 딜레이

	/* 시야각 */
	m_fFOV = XMConvertToRadians(360.f);		// FOV값 설정(시야각)
	m_bIgnoreFOV = true;					// FOV 무시

	/* 정찰관련 */
	m_vPatrolTargetPos;			// 뭐 줘야할지 아직은 모르겠다 OnSpawn때 결정날듯
	m_fPatrolRange = 10.f;      // 정찰반경

	/* 방어 및 상태 (슈아/그로기) */
	m_fMaxGrit = 14.f;              // 강인도 최대치
	m_fCurrentGrit = m_fMaxGrit;
	m_bSuppressDown = true;			// 다운/뒤잡기 면역
	m_bIsSuperArmor = false;        // 기본값 false, 공격 애니메이션 시 true로 제어
	m_bBackAttackMonster = false;	// 백어택 가능한 몬스터

	/* 스탯 컴포넌트 추가 */
	Add_Stat_Monster(m_fMaxHp, (_uint)m_fAttackDamage, m_iDefence, 15000);

	//UI에 띄울 이름
	m_ApperUIName_Kr = L"늑해의 계승자";
	m_ApperUIName_Eng = L"Successor of the Ribcage";

#pragma region deadui알람
	m_DeadUIApearAlarm.Limit = 2.f;
	m_DeadUIApearAlarm.m_AlarmFunc = [this]()
		{
			/*2초뒤에 켜지고, 5초뒤에 꺼져라*/
			UIObj_BossDead::BossDeadUIEvent Event;
			Event.m_fDeadSecond = 8.f;
			m_pGameInstance->Publish(Event);

			m_DeadUIApearAlarm.Off();

		};

	m_DeadUIApearAlarm.Off();


#pragma endregion

	// Dissolve 시간
	m_fDissolveMax = 4.f;

	return S_OK;
}

HRESULT Client::WolfGhost::Ready_Sockets()
{
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandA"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandA"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandB"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandB"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftForeArmA_Attach.001"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftForeArmB_AttachSocket_0"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightForeArmB_AttachSocket_0"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftForeArmB_AttachSocket_1"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightForeArmB_AttachSocket_1"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandMiddleB4"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("CHARA_OFFSET"), E_FAIL);

	m_pRightHandAMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandA");
	m_pRightHandBMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandB");
	m_pLeftHandAMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("LeftHandA");
	m_pLeftHandBMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("LeftHandB");
	m_pLeftForeArmAMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("LeftForeArmA_Attach.001");			// VoidLaser용
	m_pLeftForeArmBMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("LeftForeArmB_AttachSocket_0");		// IceBlast용
	m_pRightForeArmBMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightForeArmB_AttachSocket_0");	// IceBlast용

	return S_OK;
}

void Client::WolfGhost::Spawn_Attack()
{
	// 꼭 해야되는 애들 적어두기
	WolfGhost_Attack::WolfGhostAttackDesc desc;
	desc.iAttackType = _UINT(WG_END);
	desc.vPosition = {};
	desc.fRadius = {};
	desc.fLifeTime = {};

	_uint animationIndex = m_pModelCom->Get_CurrentAnimationIndex();

	// 엄 준 식
	switch (animationIndex)
	{
	case WG_TELEPORT:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKSPECIAL0;
		desc.iAttackType = _UINT(WG_TELEPORT);
		desc.matWorldPos = m_pTransformCom->Get_WorldFloat4x4();
		desc.bSetWorldPos = true;
		desc.fRadius = 12.f;
		desc.fLifeTime = 0.1f;
		desc.fAttackDamage = 100.f;//354.f;
		desc.eProjectileDamagePower = DAMAGEPOWER::HEAVYPOWER;

		// 순간이동 해야되면
		if (m_bTeleported == false)
		{
			m_bIsVisible = false;
			m_bTeleported = true;

			_vector pos = Get_Position();
			_vector look = Get_Look();
			_vector targetPos = pos + look * 42.f;

			// Look 반대 방향으로 축 직접 세팅
			_vector reverseLook = XMVectorNegate(look);
			reverseLook = XMVector3Normalize(reverseLook);
			_vector right = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), reverseLook));
			_vector up = XMVector3Cross(reverseLook, right);

			m_pTransformCom->Set_State(DIRECTION::RIGHT, right);
			m_pTransformCom->Set_State(DIRECTION::UP, up);
			m_pTransformCom->Set_State(DIRECTION::LOOK, reverseLook);

			Teleport(targetPos);
		}
		else
		{
			m_bIsVisible = true;
			m_bTeleported = false;
		}
		break;

	case WG_BLOODBOOM:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKMELEE1;

		desc.iAttackType = _UINT(WG_BLOODBOOM);
		desc.vPosition = m_pGameInstance->Get_PlayerPos_Float4();
		desc.vPosition.y += 0.5f;
		desc.fRadius = 2.f;
		desc.fLifeTime = 2.2f;
		desc.fAttackDamage = 354.f;//354.f;
		desc.eProjectileDamagePower = DAMAGEPOWER::HEAVYPOWER;
		break;
		
	case WG_THUNDER:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKFAR3;

		desc.iAttackType = _UINT(WG_THUNDER);
		desc.vPosition = m_pGameInstance->Get_PlayerPos_Float4();
		desc.vPosition.y += 0.5f;
		desc.fRadius = 1.7f;
		desc.fLifeTime = 7.f;
		desc.fAttackDamage = 287.f;//287.f;
		desc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
		break;

	case WG_FIREBLAST:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKFAR1;

		// 얘는 방향도 필요해서 WorldMatrix로 전달
		_matrix worldmat = m_pTransformCom->Get_WorldMatrix();
		_vector look = worldmat.r[2];
		_vector position = worldmat.r[3];
		position += look * 9.f;
		worldmat.r[3] = position;

		desc.iAttackType = _UINT(WG_FIREBLAST);
		XMStoreFloat4x4(&desc.matWorldPos, worldmat);
		desc.bSetWorldPos = true;
		desc.fRadius = 12.f;
		desc.fLifeTime = 1.f;
		desc.fAttackDamage = 319.f;//1023.f;
		desc.eProjectileDamagePower = DAMAGEPOWER::HEAVYPOWER;
		break;

	case WG_VOIDLASER:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKMELEE0;

		// 얘도 방향 중요한데 socketmatrix 업데이트만 필요해서 위치 계산 없이 전달
		desc.iAttackType = _UINT(WG_VOIDLASER);
		desc.fRadius = 2.f;
		desc.fLifeTime = 3.0f;
		desc.fAttackDamage = 364.f;//816.f;
		desc.matWorldPos = m_pTransformCom->Get_WorldFloat4x4();
		desc.bSetWorldPos = true;
		desc.pBoneMatrix = m_pLeftForeArmAMatrix;
		desc.eProjectileDamagePower = DAMAGEPOWER::HEAVYPOWER;
		break;

		// 일단 뒤로 미뤄
	case WG_SOLARBEAM:
		break;

	case WG_ICEBLAST:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKMELEE3;

		desc.iAttackType = _UINT(WG_ICEBLAST);
		desc.fRadius = 0.3f;
		desc.fLifeTime = 2.5f;
		desc.fAttackDamage = 49.f;//77.f;
		desc.matWorldPos = m_pTransformCom->Get_WorldFloat4x4();
		desc.bSetWorldPos = true;
		desc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
		desc.pBoneMatrix = m_pLeftForeArmBMatrix;	// 왼손
		POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_ATTACK, &desc);	// 2개 쏴야돼서 하나 먼저 생성
		desc.pBoneMatrix = m_pRightForeArmBMatrix;	// 오른손
		break;

	case WG_SOLARLASER:
		// 이 패턴 사용중이면 FireBall
		if (m_eSolarLaserPhase != SR_LASER)
		{
			Laser_After_FireBall();

			return;
		}
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKMELEE4;

		desc.iAttackType = _UINT(WG_SOLARLASER);
		desc.fRadius = 0.8f;
		desc.fLifeTime = 7.f;
		desc.fAttackDamage = 234.f;//924.f;
		desc.matWorldPos = m_pTransformCom->Get_WorldFloat4x4();
		desc.bSetWorldPos = true;
		desc.eProjectileDamagePower = DAMAGEPOWER::STRONGPOWER;
		desc.pBoneMatrix = m_pLeftHandBMatrix;	// 왼손
		desc.iMonsterID = m_iObjectID;
		m_eSolarLaserPhase = SR_RIGHTHAND;		// 이 패턴용 다음에 어떤 공격 할지
		break;

	case WG_HELLFIREBOLT:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKFAR0;

		desc.iAttackType = _UINT(WG_HELLFIREBOLT);
		desc.fRadius = 0.4f;
		desc.fLifeTime = 1.f;
		desc.fAttackDamage = 77.f;//77.f;
		desc.matWorldPos = m_pTransformCom->Get_WorldFloat4x4();
		desc.bSetWorldPos = true;
		desc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
		desc.fStartUpElapseTime = 1.5f;
		break;

	case WG_VOIDSPHERE:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKFAR2;

		desc.iAttackType = _UINT(WG_VOIDSPHERE);
		desc.fRadius = 1.f;
		desc.fLifeTime = 10.f;
		desc.fAttackDamage = 86.f;//77.f;
		desc.matWorldPos = m_pTransformCom->Get_WorldFloat4x4();
		desc.bSetWorldPos = true;
		desc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
		break;

	case WG_ICEHOMING:
		m_iPrevPattern = m_iCurrentPattern;
		m_iCurrentPattern = MS_ATTACKFAR4;

		desc.iAttackType = _UINT(WG_ICEHOMING);
		desc.fRadius = 0.9f;
		desc.fLifeTime = 1.f;	// 얘는 중간 생성 역할만 함
		desc.fAttackDamage = 94.f;// 120.f;
		desc.matWorldPos = m_pTransformCom->Get_WorldFloat4x4();
		desc.bSetWorldPos = true;
		desc.eProjectileDamagePower = DAMAGEPOWER::STRONGPOWER;
		desc.fSpeed = 0.f;


		break;
	}

	GameObject* pAcquired = POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_ATTACK, &desc);

	// VoidLaser 공격 객체 캐싱
	if (desc.iAttackType == _UINT(WG_VOIDLASER) || 
		desc.iAttackType == _UINT(WG_SOLARLASER))
		m_pLaserAttack = DCAST(WolfGhost_Attack*)(pAcquired);
}

void Client::WolfGhost::OnAwarePlayer()
{
	CHECK_JUST_NULL(m_pStateMachineCom);

	_uint	iCurState = m_pStateMachineCom->Get_CurStateType();
	CHECK_FALSE(iCurState == MS_AWAKE);

	if (!m_bFirstAware)
	{
		//////////////////////////////////HP바연동///////////
		UIObject* pBossHpBar = m_pGameInstance->Find_UI_ByName(L"Boss_HPBar");
		if (pBossHpBar)
		{
			UIObj_HpBar* pProgressBar = dynamic_cast<UIObj_HpBar*>(pBossHpBar);
			if (pProgressBar)
			{
				pProgressBar->Set_CurrentFloat(Get_CurrentHp());
				pProgressBar->Set_MaxFloat(Get_MaxHp());

				pProgressBar->Init_Ratio(true, 1.f);
				pProgressBar->Init_Ratio(false, 1.f);


				pBossHpBar->Set_Active(false, false);

				pProgressBar->Set_Owner_ObjectID(Get_ObjectID());
				pProgressBar->Set_NameTex("Textures/Wolf_Name");
			}
		}

		m_pGameInstance->Play_Sound("2St_Battle_Boss_BGM", 0.f, true);

		///////////////////////////////////////////////////
		//이름켜주기!!내부에서 알아서 보스hp바랑연동.
		UIObj_BossName::BossNameUIEVent BossNameEvent;
		BossNameEvent.bActive = true;
		BossNameEvent.BossName_Korean = m_ApperUIName_Kr;
		BossNameEvent.BossName_English = m_ApperUIName_Eng;
		BossNameEvent.English_Spacing = 20.f;
		BossNameEvent.English_LocalPos = _float2(-22.f, 0.f);
		BossNameEvent.m_eBossType = BOSS_TYPE::WOLFGOST;

		m_pGameInstance->Publish(BossNameEvent);


		m_bFirstAware = true;
	}
}
void Client::WolfGhost::Laser_After_FireBall()
{
	_vector bonePosition = {};	// 불꽃이 나올 위치
	// 오른손 먼저
	if (m_eSolarLaserPhase == SR_RIGHTHAND)
	{
		bonePosition = (XMLoadFloat4x4(m_pRightHandAMatrix) * Get_WorldMatrix()).r[3];
		m_eSolarLaserPhase = SR_LEFTHAND;
	}
	else
	{
		bonePosition = (XMLoadFloat4x4(m_pLeftHandAMatrix) * Get_WorldMatrix()).r[3];
		m_eSolarLaserPhase = SR_LASER;
	}

	_vector monsterposition = Get_Position();	// 각도를 위한 몬스터 위치
	_vector fireDirection = bonePosition - monsterposition + XMVectorSet(0.f, 0.5f, 0.f, 0.f); // 불꽃이 발사될 방향

	static Projectile_Homing::HOMING_DESC desc;
	if (m_bFireBallCached == false)
	{
		// Projectile 기본 필수값
		desc.vPosition.w = 1.f;
		desc.fAttackRadius = 0.8f;
		desc.fSpeed = 5.f;
		desc.fAttackDamage = 100.f;// 243.f;
		desc.fLifeTime = 2.f;
		desc.eProjectileDamagePower = DAMAGEPOWER::NORMALPOWER;
		desc.bPiercing = false;
		desc.iMaxHitCount = 1;
		desc.vDebugColor = _float3{ 1.f, 0.5f, 0.1f };

		// 유도 필수값
		desc.fHomingTurnSpeed = 180.f;	// 초당 회전 각도
		desc.fHomingDelay = 0.1f;		// 유도 시작 시간
		desc.fHomingDuration = 999.f;	// 수명 끝까지 유도
		desc.fAcceleration = 30.f;		// 가속도
		desc.fMaxSpeed = 17.f;			// 최고 속도(가속도 기반일때) (0이면 무제한)
		desc.fMinSpeed = 0.f;			// 최저 속도(가속도 기반할때)
		desc.fHomingLosAngle = 270.f;	// 유도 범위(시야각 느낌)
		desc.fArrivalRadius = 1.5f;		// 유도를 멈출 범위
		desc.bPredictTarget = false;	// 이동 경로 예측(아직 안 만듬)
		desc.vTargetOffset = { 0.f, 1.f, 0.f }; // 플레이어 몸통 위치 보정
		desc.eSpanwEffectID = POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET;
		desc.eDeSpanwEffectID = POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HIT;

		desc.strSpawnSound = "AURORA_KETSUGI_FIREBULLET_MOVE";
		desc.strDespawnSound = "AURORA_KETSUGI_FIREBULLET_HIT";

		m_bFireBallCached = true;
	}

	// 매 발사마다 갱신
	XMStoreFloat3(&desc.vDir, fireDirection);
	XMStoreFloat4(&desc.vPosition, bonePosition);

	POOLING->Acquire(POOL_ID::PROJECTILE_MONSTER_HOMING, &desc);
}

void Client::WolfGhost::Spawn_Effect()
{
	_matrix worldMatrix = Get_WorldMatrix();
	_vector right = worldMatrix.r[0];
	_vector look = worldMatrix.r[2];
	_vector position = worldMatrix.r[3];
	
	position += look * 1.f; // 몬스터 중앙부터 시작하기 위해 첫 오프셋 맞추기

	/* 공통 변수 */
	Projectile::PROJECTILE_DESC desc;
	desc.fAttackRadius = 1.f;	// 디버그용 구체 크기
	desc.vDebugColor = _float3(0.f, 0.0f, 0.0f);	// 디버그용 구체 색상
	desc.fLifeTime = 1.4f;
	desc.eSpanwEffectID = POOL_ID::EFFECT_WOLF_TP_FLAME;
	
	// vDir에는 최초 회전 방향이 들어감

	/* 앞에거 하나 */
	XMStoreFloat3(&desc.vDir, right);	// 오른쪽으로 회전(+Z기준)
	XMStoreFloat4(&desc.vPosition, position + look * 5.f);	// 앞에서 생성
	POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_EFFECT, &desc);

	/* 뒤에거 하나 */
	XMStoreFloat3(&desc.vDir, -right);	// 왼쪽으로 회전(+Z기준)
	XMStoreFloat4(&desc.vPosition, position - look * 5.f);	// 뒤에서 생성
	POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_EFFECT, &desc);

	/* 왼쪽 하나 */
	XMStoreFloat3(&desc.vDir, look);	// 앞쪽으로 회전(+Z기준)
	XMStoreFloat4(&desc.vPosition, position - right * 5.f);	// 왼쪽에서 생성
	POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_EFFECT, &desc);

	/* 오른쪽 하나 */
	XMStoreFloat3(&desc.vDir, -look);	// 뒤쪽으로 회전(+Z기준)
	XMStoreFloat4(&desc.vPosition, position + right * 5.f);	// 오른쪽에서 생성
	POOLING->Acquire(POOL_ID::PROJECTILE_WOLFGHOST_EFFECT, &desc);

	m_bFlame = false;
}

void Client::WolfGhost::OnDamaged(const DAMAGE_EVENT& _damageEvent)
{
	if ((m_fCurrentHp -= RandomDamage(_damageEvent.fDamage)) < 0)
	{
		m_pGameInstance->Play_Sound("BOSS_DEAD_COMMON_00_Play", 0.4f, false);
		m_pGameInstance->FadeOutSound("2St_Battle_Boss_BGM", 5.f);


		m_DeadUIApearAlarm.On();
	}
	__super::OnDamaged(_damageEvent);
}

void Client::WolfGhost::Change_Phase()
{
	m_RimAlarm.m_AlarmFunc = [this] {
		MonsterWave = true;
		m_pPhase1Table = m_pTransitionTable;
		m_pTransitionTable = m_pPhase2Table;
		m_pPhase2Table = nullptr;

		m_fCurrentHp = m_fMaxHp;
		m_fAttackTimer = m_fAttackDuration;
		
		m_RimAlarm.Off();
		};

	m_RimAlarm.Elapsed = 0.f;
	m_RimAlarm.Limit = 13.9f;

	m_RimAlarm.On();
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::WolfGhost::Update_Priority(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

	m_pModelCom->Play_Animation_CS(fTimeDelta);

	m_RimAlarm.Update(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::WolfGhost::Update_Parallel(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));

	m_pModelCom->Update_Socket_Simple();

	if (m_bIsVisible == false && m_pStateMachineCom->Get_CurStateType() != MS_ATTACKSPECIAL0)
	{
		m_bIsVisible = true;
	}

	/* 이 보스는 특수하게 거리가 가까우면 텔레포트함 */
	/* 그래서 타이머를 좀 다르게 관리하려고 이런식으로 관리 */

	// 공격 불가능한 상태고(이 보스의 경우 텔레포트 가능한 상태)
	if (!m_bCanAttack)
	{
		// 범위 안에 플레이어가 들어와있지 않으면
		if (m_fDistanceSq >= m_fWalkRangeSq)
		{
			// 어택 타이머 늘린만큼 줄이기(플레이어가 범위 안에 오래 머무르면 텔레포트 시키게끔)
			// 왜냐하면 __super::Update_Parallel에서 관리하고 있으니까
			m_fAttackTimer -= fTimeDelta;
		}
	}
	if (m_pStateMachineCom->Get_CurStateType() == MS_IDLE)
	{
		m_fAttackTimer = 0.f;
	}

	//if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::WHEEL))
	//{
	//	Spawn_Effect();
	//}

//#ifdef _DEBUG
//	BoundingSphere sphere;
//	sphere.Center = Get_Position_Float3();
//	sphere.Radius = m_fWalkRange;
//
//	Add_Debug_Sphere(sphere);
//#endif // _DEBUG


	return 0;
}

_int Client::WolfGhost::Update(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));

	if (m_bIsKilled)
	{
		m_pStateMachineCom->Change_State(MS_DEAD);
	}

	// 상태 전이 업데이트
	Update_Transition();

	if (m_bFlame)
		Spawn_Effect();

	// 상태머신 컴포넌트 업데이트
	if (m_pStateMachineCom)
		m_pStateMachineCom->Update_State(fTimeDelta);

	// 레이저 자르기
	if (m_pLaserAttack && !m_vecLaserSystems.empty())
	{
		// 재생 중인 시스템이 있는가
		_bool bAnyPlaying = false;
		for (auto& cache : m_vecLaserSystems)
		{
			if (cache.pSystem && cache.pSystem->Is_Playing())
			{
				bAnyPlaying = true;
				break;
			}
		}

		if (!bAnyPlaying)
		{
			// 재생 끝나면 스케일 복구
			for (auto& cache : m_vecLaserSystems)
			{
				if (cache.pSystem == nullptr)
					continue;
				
				// MeshEffect 세팅
				for (auto& [idx, fOriginalScale] : cache.vecMeshScales)
				{
					MeshEffect* pMeshEffect = CAST(MeshEffect*)(cache.pSystem->Find_EffectByIndex(idx));
					if (pMeshEffect != nullptr)
						pMeshEffect->Set_ScaleX(fOriginalScale);
				}
			}

			// 전부 끝났다면 정리
			m_pLaserAttack = nullptr;
			m_vecLaserSystems.clear();
			m_fLaserRatio = 1.f;
		}
		else
		{
			// 비율 계산
			_float fHitDist = m_pLaserAttack->Get_LaserHitDistance();

			// 혹시 몰라서 음수 나오면 원래 값으로 설정
			if (fHitDist <= 0.f)
				fHitDist = 50.f;
			m_fLaserRatio = min(fHitDist / 50.f, 1.f);		// 비율 1보다 커지는거 방지

			// 모든 시스템이 가지고 있는 Mesh에 적용(사실상 레이저 밖에 없다)
			for (auto& cache : m_vecLaserSystems)
			{
				if (!cache.pSystem || !cache.pSystem->Is_Playing())
					continue;

				// 구조체 가지고 MeshEffect scale 바꿔주기
				for (auto& [idx, fOriginalScale] : cache.vecMeshScales)
				{
					MeshEffect* pMeshEffect = CAST(MeshEffect*)(cache.pSystem->Find_EffectByIndex(idx));
					if (pMeshEffect != nullptr)
						pMeshEffect->Set_ScaleX(fOriginalScale * m_fLaserRatio);
				}
			}
		}
	}

	OnAwarePlayer();

	Move_Smooth(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::WolfGhost::Update_Late(const _float fTimeDelta)
{
	CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));

	Add_RenderGroup(RENDER_GROUP::NONBLEND);
	Add_RenderGroup(RENDER_GROUP::SHADOW);

#ifdef _DEBUG
	Add_Debug_Controller_Capsule(fTimeDelta);
	Add_Debug_LookLine();
	//BoundingSphere desc;
	//desc.Center = Get_Position_Float3();
	//desc.Radius = 10.f;

	//Add_Debug_Sphere(desc);
#endif // _DEBUG


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost::Render(const _float fTimeDelta)
{
	CHECK_FALSE_RESULT(m_bIsActive, E_FAIL);
	CHECK_FALSE_RESULT(m_bIsVisible, E_FAIL);
	CHECK_FAILED(__super::Render(fTimeDelta), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Begin(9), E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint bitFlag = 0;

		//CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL); // t0: _C
		//CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitFlag), E_FAIL); // t1: _N
		//CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL); // t2: _U
		//CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &bitFlag), E_FAIL); // t3: _AA
		//CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_SHININESS, 0, &bitFlag), E_FAIL); // t4: _SS
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitFlag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &bitFlag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitFlag), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitFlag), E_FAIL);

		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitFlag, sizeof(_uint));
		m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);
		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Commit(9), E_FAIL);
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	return S_OK;
}

HRESULT Client::WolfGhost::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	// Noise Texture
	_uint bUseNoise = (m_fTimeElapsed > 0.f) ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		_float fNormalized = m_fTimeElapsed / m_fDissolveMax;
		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &fNormalized, sizeof(_float));
	}

	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Begin(4, false), E_FAIL);

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

		CHECK_FAILED(m_pShaderCom->Bind_Resources(4), E_FAIL);

		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::WolfGhost::Bind_ShaderResources()
{


	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
WolfGhost* Client::WolfGhost::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	WolfGhost* pInstance = new WolfGhost(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"WolfGhost 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::WolfGhost::Clone(void* pArg)
{
	WolfGhost* pInstance = new WolfGhost(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"WolfGhost 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::WolfGhost::Free()
{
	__super::Free();

	Safe_Release(m_pPhase1Table);
	Safe_Release(m_pPhase2Table);
}
/******************************************************* 객체 반환 함수 *******************************************************/
