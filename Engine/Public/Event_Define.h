#pragma once
// Event_Define.h

NS_BEGIN(Engine)

using EventHandle = _uint; // typedef _uint EventHandle; 이랑 같음, C++ 11부터 이걸로 바꼈다는듯

// typedef struct 하기 귀찮아아아아악
typedef struct ListnerDesc {
	EventHandle iListnerNum = 0; // 객체별 고유번호(통관번호ㅎ)
	function<void(const void*)> callback = {};  // 콜백 함수
}LISTNER_DESC, Listner_Info, LISTNER_INFO;

typedef struct TestEvent {
	_uint m_iSource = 0;
	_uint m_iTarget = 0;
	_uint m_iDamage = 0;
}TESTEVENT, SAMPLE_EVENT;

typedef struct DamageEvent
{
	_uint		iAttackerID = {};         // 공격자 ObjectID
	_uint		iTargetID = {};           // 피격자 ObjectID
	_float		fDamage = {};            // 데미지 양
	_float3		vHitPosition = {};      // 맞은 월드 좌표 (이펙트 스폰용)
	_float3		vHitDirection = {};     // 맞은 방향 (넉백 방향)
	_float		fKnockbackForce = {};    // 넉백 세기
	DAMAGEPOWER eDamagePower = {}; //데미지 파워타입  
	GameObject* pDamageOwner = { nullptr };

} DAMAGE_EVENT;

//애니메이션 이벤트

typedef struct AnimEventBase //애니메이션 이벤트 공통적으로 사용할 구조체
{
	_uint	iOwnerId = {}; //누가 실행했는지
	_int	iAnimationIndex = { -1 };

	ANIM_FRAMEPHASE ePhase = { ANIM_FRAMEPHASE::START };

	//클라 디버그용	
	_float		fPrevFrame = {};
	_float		fCurrentFrame = {};
	_float		fDurtaion = {};

}ANIMBASE_EVENT;

typedef struct SoundEvent : public ANIMBASE_EVENT
{
	_bool		bLoop = { false };
	_bool		bRandomPlay = { false };
	_bool		bInGroup = { false };
	_bool		bGroupPlay = { false }; //그룹전체 재생
	_float		fVolume = { 1.f };
	_string		SoundName = "";
	_string		strGroupName = "";

}SOUND_EVENT;

typedef struct ParticleEvent : public ANIMBASE_EVENT
{
	_string		ParticleSystemName = {};		// ParticleSystem 이름
	_string		SocketName = {};	 			// 부착할 뼈의 이름
	_string		EndSocketName = {};				// 부착할 뼈의 끝부분 비어있다면 기존 동작 그대로
	_bool		bAttached = {};					// 무기 / 몸 뼈 구분
	_bool		bFollow = {};					// 생성 위치가 매 프레임 달라져야 하는가
	_bool		bParticleFollow = {};			// 메쉬 말고 파티클 이펙트들이 매 프레임 따라가는가
	_bool		bOnlyPosition = {};				// 소켓 행렬에서 위치 값만 사용할 것인가
	_bool		bOwnerRotation = {};			// 뼈 위치 + Owner 회전을 사용할 것인가
	POOL_ID		ePoolId = {};					// 풀링 아이디

}PARTICLE_EVENT;

typedef struct TrailEvent : public ANIMBASE_EVENT
{
	_string		TrailEffectName;
	_string		RootBoneName;
	_string		TipBoneName;
	_bool		bEnable;
	POOL_ID		ePoolId = {};					// 풀링 아이디
}TRAIL_EVENT;

typedef struct ColliderEvent : public ANIMBASE_EVENT
{
	//어떤 콜라이더 그룹 활성화 시킬지
	COLLISION_GROUP eColGroup = { COLGROUP::END };
	PX_ACTOR_TYPE   eAttackerType = { PX_ACTOR_TYPE::END }; // PhysX 공격 타입
	_float          fAttackRadius = { 0.5f };               // Sweep 반지름
	_float          fAttackDamage = { 10.f };				// 공격 데미지
	_float			fAttackHalfHeight = { 0.f };			// 캡슐 Sweep시 반높이	


}COLLIDER_EVENT;

typedef struct DissolveEvent : public ANIMBASE_EVENT
{
	_bool	bDissolve = {};			// Dissolve 플래그
	_bool	bWeapon_Dissolve = {};	// BloodWeapon 플래그
	_bool	bIsWeaponTarget = {};
	_float	fDissolveMax = {};
}DISSOLVE_EVENT;

enum class CAMERA_STATE { FREE_FOLLOW, LOCK_ON, FIXED, CINEMATIC, END };
enum class SHAKE_PRIORITY { NONE, NORMAL_ATTACK, STRONG_ATTACK, EARTH_QUAKE };

enum class CINEMATIC_LOOKAT : _uint
{
	CUSTOM_TARGET,
	PLAYER,
	YAKUMO,
	END
};

struct CameraShake {
	_bool bShakeStart = false;
	_float fDuration = {};
	_float fBlendOutTime = 0.05f;		// 셰이크 끝날때 페이드아웃 시간

	/* 위치 진동 (축별 독립) */			// 축별 분리
	_float fAmpX = {};					// 전방 강도 (가장 강하게)
	_float fAmpY = {};					// 수직 강도
	_float fAmpZ = {};					// 횡방향 강도
	_float fFreqX = 40.f;				// 전방 주파수
	_float fFreqY = 100.f;				// 수직 주파수
	_float fFreqZ = 100.f;				// 횡방향 주파수

	/* 회전 진동 */
	_float fAmpPitch = {};
	_float fAmpYaw = {};
	_float fFreqPitch = 100.f;
	_float fFreqYaw = 50.f;

	/* FOV 진동 */                        // FOV 셰이크
	_float fAmpFov = {};
	_float fFreqFov = 60.f;

	/* 임팩트 킥 */
	_float fKickStrength = {};
	_float4 vKickDirection = {};
	_float fKickDecay = 15.f;

	SHAKE_PRIORITY ePriority = { SHAKE_PRIORITY::NONE };
};

struct CINEMATIC_CHARACTER_CONTROLL {
	_uint iNumber = {};
	_float3 vPosition = {};
};

// 키프레임 모드
enum class KEYFRAME_MODE
{
	RELATIVEMODE,   // 시작 시점 기준 오프셋 (기존 방식)
	ABSOLUTEMODE,   // 절대 위치 + 절대 방향 (정해진 위치에서 정해진 곳을 봐야 할 때)
	PLAYER_OFFSET,	// 플레이어 기준 Offset
	YAKUMO_OFFSET    // Yakumo 기준 Offset
};

// 시네마틱 이벤트 타입
enum class CINEMATIC_EVENT_TYPE : _uint
{
	CAMERA_SHAKE,
	CHARACTER_CONTROL,
	// SOUND, UI 등 필요시 여기에 추가
	END
};

// 시네마틱 키프레임 구간 내 이벤트
struct CINEMATIC_EVENT
{
	struct Payload_Empty {};

	using Payload = variant<
		Payload_Empty,
		CameraShake,
		CINEMATIC_CHARACTER_CONTROLL
		// 타입 추가시 여기에 페이로드 구조체 추가
	>;

	CINEMATIC_EVENT_TYPE eType = CINEMATIC_EVENT_TYPE::END;
	_float fTriggerTime = {};    // 키프레임 구간 시작 기준 상대 시간 (초)
	_bool  bFired = {};

	Payload tPayload = Payload_Empty{};
};

// 카메라 키프레임
typedef struct tagCinematicKeyframe
{
	KEYFRAME_MODE eMode = KEYFRAME_MODE::ABSOLUTEMODE;
	EASE_TYPE eEaseType = EASE_TYPE::LINEAR;

	/* RELATIVEMODE값(상대 위치) */
	_float3 vPosOffset = {};      // 플레이어 기준 위치 오프셋
	_float  fPitchOffset = {};    // 현재 pitch 기준 오프셋 (라디안)
	_float  fYawOffset = {};      // 현재 yaw 기준 오프셋 (라디안)

	/* ABSOLUTEMODE값(절대 위치) */
	_float3 vAbsPosition = {};	  // 키프레임 카메라 위치
	_float3 vlookAtTarget = {};   // 이 지점을 바라봄 (플레이어 위치 등)
	CINEMATIC_LOOKAT eLookAt = CINEMATIC_LOOKAT::CUSTOM_TARGET; // LookAt 타겟

	/* 공통값 */
	_float  fFov = {};            // 이 키프레임의 FOV (라디안, 0이면 현재 유지)
	_float  fDuration = {};       // 이전 키프레임에서 여기까지 걸리는 시간 (초)

	/* 이벤트 목록으로 교체 */
	vector<CINEMATIC_EVENT> vecEvents;
}CINEMATIC_KEYFRAME;

typedef struct CameraEvent : public ANIMBASE_EVENT
{
	//카메라 쉐이크
	//줌인,줌아웃
	CAMERA_ACTION eCameraAction = { CAMERA_ACTION::CAMERAATCION_END };
	CAMERA_STATE eCameraState = CAMERA_STATE::END;
	CameraShake tShake;
	GameObject* pLockOnTarget = { nullptr };
	_bool bChaneCamera = { false };
	_uint iCameraNum = {};	// Client에서 캐스팅해서 넣을것

	/* 시네마틱 */
	vector<CINEMATIC_KEYFRAME> vecKeyframes;
	_bool bCinematicAutoReturn = true;  // 끝나면 이전 상태로 자동 복귀

	//카메라 액션에 필요한 변수들 추가는 구독자들이 구현하는게 맞다고 생각해서 추가는안한상태
	//카메라 액션마다 세세하게 값이 달라지낟면 _string 으로 이름까지 저장해서 해당 이름에 따른 변수의 값을 다르게 사용해도 될듯
}CAMERA_EVENT;

namespace ShakePreset
{
	enum class SHAKE_PRESET : _int
	{
		HitStop,
		HitStop_PlayerDamage,
		HitStop_PlayerWeapon,
		HitStop_LSword,
		FieldStamp_01,
		FieldStamp_01_2,
		FieldStamp_02,
		FieldStamp_03,
		Renketsu_01,
		Renketsu_02,
		SPSuck,
		PRESET_END
	};

	inline const char* ShakePresetComboStr()
	{
		return "HitStop\0HitStop_PlayerDamage\0HitStop_PlayerWeapon\0HitStop_LSword\0"
			"FieldStamp_01\0FieldStamp_01_2\0FieldStamp_02\0FieldStamp_03\0"
			"Renketsu_01\0Renketsu_02\0SPSuck\0\0";
	}

	// 일반 타격 (적 피격)
	inline CameraShake HitStop()
	{
		CameraShake shake = {};
		shake.fDuration = 1.f;
		shake.fBlendOutTime = 0.1f;
		shake.fAmpPitch = XMConvertToRadians(5.73f);
		shake.fAmpYaw = XMConvertToRadians(5.73f);
		shake.fFreqPitch = 15.f;
		shake.fFreqYaw = 6.f;
		shake.fAmpFov = 1.f;
		shake.fFreqFov = 17.f;
		shake.ePriority = SHAKE_PRIORITY::EARTH_QUAKE;
		return shake;
	}

	// 플레이어 피격 (HitStop과 동일)
	inline CameraShake HitStop_PlayerDamage()
	{
		CameraShake shake = {};
		shake.fDuration = 0.2f;
		shake.fBlendOutTime = 0.05f; //원래 0.05f
		shake.fAmpX = 0.1f; //0.25f
		shake.fAmpY = 0.1f; //0.02f
		shake.fAmpZ = 0.15f; //0.02f
		shake.fFreqX = 40.f;
		shake.fFreqY = 100.f;
		shake.fFreqZ = 100.f;
		shake.fAmpPitch = XMConvertToRadians(0.1f);
		shake.fAmpYaw = XMConvertToRadians(0.1f);	
		shake.fFreqPitch = 100.f;
		shake.fFreqYaw = 50.f;
		shake.fAmpFov = 0.5f;
		shake.fFreqFov = 60.f;
		shake.ePriority = SHAKE_PRIORITY::NORMAL_ATTACK;
		return shake;
	}

	// 플레이어 무기 타격
	inline CameraShake HitStop_PlayerWeapon()
	{
		CameraShake shake = {};
		shake.fDuration = 0.08f;
		shake.fBlendOutTime = 0.03f;
		shake.fAmpX = 0.20f;
		shake.fAmpY = 0.05f;
		shake.fAmpZ = 0.05f;
		shake.fFreqX = 100.f;
		shake.fFreqY = 100.f;
		shake.fFreqZ = 100.f;
		shake.fAmpFov = 1.f;
		shake.fFreqFov = 60.f;
		shake.ePriority = SHAKE_PRIORITY::NORMAL_ATTACK;
		return shake;
	}

	// 대검 타격 (Y도 강함, 회전 없음)
	inline CameraShake HitStop_LSword()
	{
		CameraShake shake = {};
		shake.fDuration = 0.08f;
		shake.fBlendOutTime = 0.05f;
		shake.fAmpX = 0.50f;
		shake.fAmpY = 0.50f;
		shake.fFreqX = 50.f;
		shake.fFreqY = 5.f;
		shake.fAmpFov = 0.5f;
		shake.fFreqFov = 60.f;
		shake.ePriority = SHAKE_PRIORITY::NORMAL_ATTACK;
		return shake;
	}

	// 바닥 찍기 강(가장 강하고 길다)
	inline CameraShake FieldStamp_01()
	{
		CameraShake shake = {};
		shake.fDuration = 1.f;
		shake.fBlendOutTime = 0.1f;
		shake.fAmpPitch = XMConvertToRadians(5.73f);
		shake.fAmpYaw = XMConvertToRadians(5.73f);
		shake.fFreqPitch = 15.f;
		shake.fFreqYaw = 6.f;
		shake.fAmpFov = 0.5f;
		shake.fFreqFov = 17.f;
		shake.ePriority = SHAKE_PRIORITY::EARTH_QUAKE;
 		return shake;
	}

	// 바닥 찍기 약(03에서 길이만 길어진 버전)
	inline CameraShake FieldStamp_01_2()
	{
		CameraShake shake = {};
		shake.fDuration = 0.5f;
		shake.fBlendOutTime = 0.1f;
		shake.fAmpPitch = XMConvertToRadians(1.15f);
		shake.fAmpYaw = XMConvertToRadians(1.15f);
		shake.fFreqPitch = 17.f;
		shake.fFreqYaw = 7.5f;
		shake.fAmpFov = 0.3f;
		shake.fFreqFov = 15.f;
		shake.ePriority = SHAKE_PRIORITY::NORMAL_ATTACK;
		return shake;
	}

	// 바닥 찍기 중
	inline CameraShake FieldStamp_02()
	{
		CameraShake shake = {};
		shake.fDuration = 0.7f;
		shake.fBlendOutTime = 0.1f;
		shake.fAmpPitch = XMConvertToRadians(2.86f);
		shake.fAmpYaw = XMConvertToRadians(2.86f);
		shake.fFreqPitch = 13.f;
		shake.fFreqYaw = 5.f;
		shake.fAmpFov = 0.3f;
		shake.fFreqFov = 15.f;
		shake.ePriority = SHAKE_PRIORITY::STRONG_ATTACK;
		return shake;
	}

	// 바닥 찍기 약(가장 약함)
	inline CameraShake FieldStamp_03()
	{
		CameraShake shake = {};
		shake.fDuration = 0.3f;
		shake.fBlendOutTime = 0.1f;
		shake.fAmpPitch = XMConvertToRadians(1.15f);
		shake.fAmpYaw = XMConvertToRadians(1.15f);
		shake.fFreqPitch = 17.f;
		shake.fFreqYaw = 7.5f;
		shake.fAmpFov = 0.3f;
		shake.fFreqFov = 15.f;
		shake.ePriority = SHAKE_PRIORITY::NORMAL_ATTACK;
		return shake;
	}

	// 연결 공격 01 (느린 울렁임)
	inline CameraShake Renketsu_01()
	{
		CameraShake shake = {};
		shake.fDuration = 0.5f;
		shake.fAmpPitch = XMConvertToRadians(1.0f);
		shake.fAmpYaw = XMConvertToRadians(1.0f);
		shake.fFreqPitch = 50.f;
		shake.fFreqYaw = 25.f;
		shake.fAmpFov = 1.0f;
		shake.fFreqFov = 60.f;
		shake.ePriority = SHAKE_PRIORITY::STRONG_ATTACK;
		return shake;
	}

	// 연결 공격 02 (더 느리고 짧음)
	inline CameraShake Renketsu_02()
	{
		CameraShake shake = {};
		shake.fDuration = 0.4f;
		shake.fAmpPitch = XMConvertToRadians(1.0f);
		shake.fAmpYaw = XMConvertToRadians(1.0f);
		shake.fFreqPitch = 30.f;
		shake.fFreqYaw = 15.f;
		shake.fAmpFov = 1.0f;
		shake.fFreqFov = 40.f;
		shake.ePriority = SHAKE_PRIORITY::STRONG_ATTACK;
		return shake;
	}

	// SP 흡수 (길고 울렁이는 연출)
	inline CameraShake SPSuck()
	{
		CameraShake shake = {};
		shake.fDuration = 1.3f;
		shake.fAmpPitch = XMConvertToRadians(1.0f);
		shake.fAmpYaw = XMConvertToRadians(1.0f);
		shake.fFreqPitch = 100.f;
		shake.fFreqYaw = 50.f;
		shake.fAmpFov = 1.0f;
		shake.fFreqFov = 60.f;
		shake.ePriority = SHAKE_PRIORITY::NORMAL_ATTACK;
		return shake;
	}

	inline CameraShake GetPresetShake(SHAKE_PRESET _ePreset)
	{
		switch (_ePreset)
		{
		case SHAKE_PRESET::HitStop:					return HitStop();
		case SHAKE_PRESET::HitStop_PlayerDamage:	return HitStop_PlayerDamage();
		case SHAKE_PRESET::HitStop_PlayerWeapon:	return HitStop_PlayerWeapon();
		case SHAKE_PRESET::HitStop_LSword:			return HitStop_LSword();
		case SHAKE_PRESET::FieldStamp_01:			return FieldStamp_01();
		case SHAKE_PRESET::FieldStamp_01_2:			return FieldStamp_01_2();
		case SHAKE_PRESET::FieldStamp_02:			return FieldStamp_02();
		case SHAKE_PRESET::FieldStamp_03:			return FieldStamp_03();
		case SHAKE_PRESET::Renketsu_01:				return Renketsu_01();
		case SHAKE_PRESET::Renketsu_02:				return Renketsu_02();
		case SHAKE_PRESET::SPSuck:					return SPSuck();
		default:									return CameraShake{};
		}
	}
}

using namespace ShakePreset;

typedef struct SpawnEvent {
	//				 특정 위치에, 미리 지정한 특정 Object에
	enum Spawn_Type { SPAWN_POINT, SPAWN_PREDEFINED, SPAWN_END };
	Spawn_Type _eType = { SPAWN_END };	// 생성 방법
	_wstring wstrPrototypeName = L"";	// 생성할 GameObject 프로토타입
	_wstring wstrLayerName = L"";		// 생성할 레이어
	_float3 vSpawnPoint = {};			// 생성할 위치
	_wstring wstrPointName = {};		// 등록된 Point이름
}SPAWNEVENT_DESC, SPAWN_EVENT;

typedef struct PlayerAnimEvent :public ANIMBASE_EVENT
{
	_bool	bInputArea = { false }; //입력 허용 구간 
	_bool	bCanMove = { false };   //이동 상태 가능 구간
	_bool	bCanCombo = { false };  //공격  가능한 구간 (bCanAttack으로 했어야했는데, 콤보공격가능한 구간인줄) 
	_bool	bCanEscape = { false }; //회피(구르기/백스텝)가능한구간
	_bool	bSuperArmor = { false }; //슈퍼아머 구간(데미지는 그대로 받지만  / 피격 애니메이션이 재생되지 않는 상태)
	_float	fFalterResistance = { 0.f }; //경직/경감 관련 내성 구간 (즉 슈퍼아머일때 이 값을 활용?)
	_bool	bInvincible = { false }; //무적 구간(데미지도 안받고 + 피격 애니메이션도 재생되지않음)
	_float	fStaminaCost = { 0.f }; //감소해야하는 스태미나 비용
	_bool	bLockOnHomingRotation = { false }; //락온시 회전 여부
	_float  fHomingRotationSpeedRatio = { 0.f }; //회전 얼마나 강하게 할건지
	_bool	bBlockRegenStamina = { false }; //스태미나 재생 제어
	_bool	bUseItem = { false }; // 아이템 사용
	_bool	bWeaponVisible = { false }; //웨폰 비활성화
	_bool	bBloodWeaponVisible = { false }; //블러드 웨폰 활성화
	_bool	bCanParry = { false }; //패링 가능한 구간
	_bool	bCreateProjectile = { false }; //투사체 발사 구간
	_bool	bChangeEquipWeapon = { false }; //무기 교체 시간
	_bool   bControlAnimSpeed = {}; //애니메이션 속도 재생 제어
	_float  fAnimationSpeed = { 0.f }; //애니메이션 속도 
	_bool   bKetsugiStart = { false }; //스킬(버프)관련 시작 구간제어
	_bool	bInjectionVisible = { false }; //주사기 활성화

}PLAYERANIM_EVENT;

// ==================================================================================
// Monster 애니메이션 이벤트 
// 이벤트(몽타주 파일 내 이름) 설명
typedef struct MonsterAnimEvent : public ANIMBASE_EVENT
{
	ANIM_EVENT_TYPE eMonsterEventType = { ANIM_EVENT_TYPE::MONSTER_ANIM };

	// 1. 상태 고정 (ANS_Stiff) : 쳐맞아도 모션 안 끊기는 구간
	_bool bCancelable = { true }; // false면 공격받아도 캔슬 안 됨
	_bool bChangeValue = { false }; // 이 Notify가 값들을 바꿀건지

	// 2. 슈퍼아머 (ANS_SuperArmor)
	_bool bEnableSuperArmor = { false };
	_float fResistance = { 0.f }; // 버틸 수 있는 경직 수치

	// 3. 시선 고정 해제 (ANS_EnemyAIDisableLookAt) 
	_bool bDisableLookAt = { false }; // true면 플레이어 안 쳐다봄

	// 4. 단발성 거리 보정 (ANS_AIMoveOneShotMotion) : 루트 모션 자체 이동거리에 추가로 보정해줄 이동거리 
	_bool   bTargetPosCached = false;// AN_AICacheTargetPos 도약같은 특수 모션의 경우 플레이어의 현재 위치가 아니라 도약했을때 점찎은 위치를 찍어야됨

	_float fMaxMoveDistance = { 0.f }; // 최대로 미끄러질 거리
	_float fTargetOffset = { 0.f }; // 타겟과 남겨둘 최소 거리

	// 5. 유도 회전 (ANS_AIHomingRotation)
	_float fHomingSpeedDegree = { 0.f }; //초당 회전 각도
	_bool	bEnableSlide = { false };			// 이동 할지
	_bool	bEnableHoming = { false };			// 회전 할지

	// 6. 턴 인터럽트 (AN_AITurnInterrupt)
	_float fYawThreshold = { 0.f }; // 캔슬 발동 최소 각도
	_float fTurnSpeed = { 0.f }; // 돌 때 속도
	_float fTimeLimit = { 0.f }; // 최대 허용 시간

	// 7. 콤보 체커 (AN_AIComboCheck)
	_bool bComboAttack = { false }; // 연계공격 할건지
	_int iProbabilityRate = { 0 }; // 발동 확률
	_float fComboMaxDistance = { 0.f };	// 연계 허용 최대 거리
	_int iNextState = { 0 }; // 전이할 다음 상태

	// 8. 광역기 장판 마커 (Marker:Field_Attack)
	_string strMarkerID = { "" }; // 스폰할 마커/장판의 ID (이건 고민좀해봐야됨)

} MONSTER_ANIM_EVENT;

/* 마우스 고정 이벤트 */
typedef struct MouseLockEvent
{
	_bool bLock = false;
}MOUSELOCK_EVENT;

typedef struct DeadObjectEvent {
	_bool bDeadObject = { false };
	GameObject* pDeadObject = { nullptr };  // 죽은 오브젝트 포인터
} DEAD_EVENT;

#pragma region ObjectPooling
enum class MONSTER {
	SLIME, SLAVE_DEVIL, SLAVE_VAMPIRE, OLIVER,		// Stage : UnderPass 
	END
};

enum class POOLING_TYPE {
	MONSTER, EFFECT, END
};

/* 오브젝트 풀링 이벤트 */
typedef struct PoolingManagerEvent {
	/* 몬스터 스폰 */
	struct SpawnMonster {
		_bool	bSpawn = false;
		_uint   iCount = {};
		MONSTER eMonster = MONSTER::END;
		_float3 vPosition = {};
	};

	POOLING_TYPE eType = POOLING_TYPE::END;
	SpawnMonster tSpawn;

}POLLING_EVENT;
#pragma endregion ObjectPooling

/*ui활성화 시 입력막는 이벤트*/
//카메라,플레이어,커서가 구독한다.
struct INPUT_LOCK_EVENT
{
	bool bLock = false;
};

//SET : 강제 설정
//ADD: 추가값보내기 
enum class HazeChangeType{SET,ADD,REDUCE,END};
struct HazeChangeEvent
{
	HazeChangeType		m_eChangeType;
	int iHaze;//ADD인경우 추가값, SET인경우 최종값.



};


/*Stat 관련 이벤트 */
enum class GuardBarUIEventType { ONGUARDEVENT, END };
struct GuardBarUIEvent
{
	_uint m_iOwnerID;
	GuardBarUIEventType  m_EventType;
	_float m_fValue = 0.f;  //넘길값

};

enum class  ManaUIEventType { INTIALIZE,UPDATE_CURRENTMANA, UPDATE_MAXMANA, END };
struct ManaUIEvent
{
	ManaUIEventType eType;
	_float fMaxValue = 0.f;
	_float fValue = 0.f;

};

enum class CheckPointEventType {START,END};
struct PlayerCheckPointEvnet
{
	CheckPointEventType eCheckPointType;
	_uint iLevel;
	_uint iIdx; 
};



NS_END