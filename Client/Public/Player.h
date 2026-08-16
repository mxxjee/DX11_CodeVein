#pragma once
#include "Client_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class Model;
class TrailEffect;
class StateMachine;
class Stat;
class Player_Stat;
NS_END

NS_BEGIN(Client)
class MinimapRenderComponent;
class InteractionManager;
class Monster;
class Skill_Base;

class Player : public ContainerObject
{
public:
	typedef struct tagPlayerDesc : public GameObject::GAMEOBJECT_DESC
	{
		PHYSX_ACTOR_DESC tActorDesc;
		PHYSX_CONTROLLER_DESC tControllerDesc;

	}PLAYER_DESC;

public:
	enum PLAYERSTATE
	{
		IDLE,			//기본 상태(무기별) + 거점의 경우에는 기본 Idle  0
		WALK,			//걷기 (락온 or 막기) 1 
		RUN,			//기본 키입력이동 2
		DASH,			//RUN + Shift 일때 3 
		ROLL,			//키입력 + Space 구르기 4
		BACKSTEP,		//키입력x + Space 구르기 5 
		ATTACKNORMAL,	//기본공격 6
		ATTACKSTRONG,	//강공격 7
		ATTACKDODGEF,	//대쉬에서 마우스 클릭시 공격 모션 8 
		ATTACKDODGEB,	//제자리 백스텝 후 마우스 클릭시 공격모션 9
		SPECIALATTACK,	// 무기별 고유 액션 10 
		GUARDWALK,		//가드 전용 걷기모션 11
		GUARDHIT,		//가드시에 피격 12
		GUARD_BREAK,	//가드상태 + 스태미나가 0일때 재생 13
		DAMAGE,			//기본 피격 상태 14
		DAMAGEBLOW,		//피격 날라가는 모션 (Blow,Strike,Loop)15
		DAMAGEEND,		//피격시 일어나는 모션16
		ATTACKPARRY,    //패링 모션 17
		SPECIALSUCK,	//패링성공 연출 모션 18 
		BACKSTAB ,		//뒤잡 성공 + 연출 전 모션 19
		ATTACKSTRONGSTART, //강공격 (일반,차징 분기용) 20
		ATTACKSTRONGCHARGE, //강공격 차징 21
		CHECKPOINT,			//체크포인트(겨우살이) 진입 22
		PICKITEM,		//아이템 줍기 상태 23
		OPENBOX,		//상자 열기 상태 24
		DEATH,			//죽을때 상태25
		SKILL,			//스킬 상태 26
		LADDER,			//사다리 상태 27
		CUSTOM,			//커스텀씬 상태 28
		END
	};

	enum PLAYERUPPERSTATE
	{
		GUARDSTART,		//Alt 시작 가드 상태 0
		GUARDLOOP,		//Alt 루프 가드 상태 1
		GUARDEND,		//Alt 끝날때 가드 상태 2
		USEITEM,		//아이템 사용 상태 3
		WEAPONCHANGE,   //무기 교체 4 
		PLAYERUPPERSTATE_END 
	};

	enum INPUT_MASK
	{
		INPUT_W = 1 << 0, //0001
		INPUT_A = 1 << 1, //0010
		INPUT_S = 1 << 2, //0100
		INPUT_D = 1 << 3, //1000
	};

	enum DAMAGE_LEVEL : _int //기본 피격 데미지 레벨 
	{
		DAMAGE1 = 0,DAMAGE2 =1,DAMAGE3 = 2,DAMAGE4= 3,DAMAGELEVEL_END = 4
	};

	enum CINEMATIC_PLAYER_NUMBER
	{
		CP_OLIVER_TELEPORT = 0, CP_WOLF_TELEPORT = 5, CP_END
	};

	typedef struct tagPlayerInput
	{
		_vector			vMoveDir = {};
		INPUT_DIR		InputDir = INPUT_DIR::INPUT_END;

		MOUSEKEYSTATE	eLastMouseKeyState = {};
		_bool			bShiftPressed = { false };
		_bool			bAltPressed = { false };
		_bool			bSpaceDown = { false };
		_bool			bAltDown = { false };
		_bool			bAltUp = { false };
		_bool			bZeroStamina = { false };
		_bool			bLockOn = { false };
		_bool			bCDown = { false }; //C 입력
		_bool			bUpKeyDown = { false }; 
		_bool			bDownKeyDown = { false };

		_int			iSkillSlotKeyDown = { -1 }; //스킬 1~8 눌렀는지

	}INPUT_DESC;

	typedef struct tagPlayerAnimRunTimeEvent
	{
		_int iInputAreaCount = 0; //입력 받을수 있는 구간
		_int iCanMoveCount = 0; //이동할수있는구간
		_int iCanComboCount = 0; //공격할수있는구간
		_int iCanEscapeCount = 0; //회피기 사용할수있는구간
		_int iCanSuperArmorCount = 0; //슈퍼아머구간
		_int iCanInvincible = 0; //무적 구간
		_int iCanLockOnHomingRotation = 0; //락온일때 회전해야하는구간
		_int iCanWeaponVisible = 0; //기본 무기 비활성화 구간
		_int iCanBloodWeaponVisible = 0; //블러드 무기 활성화 구간
		_int iCanParry = 0; //패링 가능한 구간
		_int iCanInjectionVisible = 0; //주사기 활성 구간

		_bool bInputArea() const { return iInputAreaCount > 0; }
		_bool bCanMove()  const { return iCanMoveCount > 0; }
		_bool bCanCombo() const { return iCanComboCount > 0; }
		_bool bCanEscape()const { return iCanEscapeCount > 0; }
		_bool bCanSuperArmor() const { return iCanSuperArmorCount > 0; }
		_bool bCanInvincible() const { return iCanInvincible > 0; }
		_bool bCanLockOnHomingRotation() const { return iCanLockOnHomingRotation > 0; }
		_bool bCanWeaponVisible() const { return iCanWeaponVisible > 0; }
		_bool bCanBloodWeaponVisible() const { return iCanBloodWeaponVisible > 0; }
		_bool bCanParry() const { return iCanParry > 0; }
		_bool bCanInjectionVisible() const { return iCanInjectionVisible > 0; }

		_float fStaminaCost = { 0.f }; //감소해야하는 스태미나 비용
		_float fFalterResistance = { 0.f }; //경직/경감 관련 내성
		_float fHomingRotationSpeedRatio = { 0.f }; //회전 얼마나 강하게 할지

		_bool bBlockRegenStamina = { false }; //스태미나 리젠 막는구간
		_bool bUseItem = { false };

	}PLAYERRUNTIMEEVENT_DESC;


private:
	explicit Player();
	explicit Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player(const Player& original);
	virtual ~Player();

public:
	void						Set_Gravity(_bool _gravityBool) { m_bEnablePhysics = _gravityBool; }
	void						Set_Position_ByController(_fvector vWorldPos);
	class Player_Weapon*		Get_ActivePlayerWeapon() { return m_pActiveWeapon; }
	class Player_BloodWeapon*	Get_ActivePlayerBloodWeapon() { return m_pActiveBloodWeapon; }
	class Player_Injection*		Get_PlayerInjection() { return m_pActiveInjection; }


	void						Set_CanTurn(bool bTurn) { m_bTurn = bTurn; }
	INPUT_DESC&					Get_PlayerInputDesc() { return m_tInputDesc; }
	PxCapsuleController*		Get_PlayerPxControllder() { return m_pController; }
	PLAYERRUNTIMEEVENT_DESC&	Get_PlayerAnimEventDesc() { return m_tPlayerRunTimeEvent; }
	void						Reset_PlayerAnimEventDesc(); //이벤트 애니메이션 바뀔때 리셋해야할것들
	void						Reset_PlayerMoveInputDesc(); //이동 관련 입력값만 리셋용 매프레임
	void						Reset_PlayerPreesedInputDesc(); //지속형 Pressed 관련  매 프레임 리셋
	void						Reset_PlayerConsumeDesc(); //단발(소비)형 Dowm , Up 과 같은것들 
	void						Set_ApplyTranslation(_bool bApplyTranslation) { m_bApplyTranslation = bApplyTranslation; } //루트모션 이동 제어
	_vector						Get_EnemyHitDirection() const { return m_vEnemyHitDirection; }
	DAMAGEPOWER					Get_EnemyDamagePower() const { return m_eEnemyDamagePower; }
	_bool						Get_OnGuard() const { return m_bOnGuard; }
	_float						Get_LoseHpRatio() const { return m_fLoseHpRatio; }
	GameObject*					Get_LockOnTarget() const { return m_pLockOnTarget; }
	const _float4x4*			Get_RenderWorldMatrixPtr() const { return &m_RenderWorldMatrix; }
	_vector						Get_RenderPos() const { return m_vRenderPos; }
	void						Set_PlayCinematic(_bool bPlayCinematic) { m_bPlayCineamtic = bPlayCinematic; }
	GameObject*					Get_BackStabTarget() const { return m_pBackStabTarget; }
	_bool						Get_SuccessBackStab() const { return m_bSuccessBackStab; }
	void						Set_SuccessBackStab(_bool bSucceessBackStab) { m_bSuccessBackStab = bSucceessBackStab; }
	_float4x4*					Get_BayonetMuzzleMatrixPtr() const { return m_pBayonetMuzzleMatrix; }
	LEVEL						Get_SavePointLevel() const { return m_eSavePointLevel; }
	_uint						Get_SavePointIndex() const { return m_iSavePointIndex; }
	_bool*						Get_FocuseStatePtr() { return &m_bFocusState; }
	PLAYER_SHADER_DESC&			Get_PlayerShaderDesc() { return m_tPlayerShaderDesc; }
	PLAYER_SHADER_DESC*			Get_PlayerShaderDescPtr() { return &m_tPlayerShaderDesc;}
	_bool						Get_LockOnShift() const { return m_bLockOnShift; }


	void						Set_PlayerShaderDesc(PLAYER_SHADER_DESC _tPlayerShaderDesc) { m_tPlayerShaderDesc = _tPlayerShaderDesc; }
	void						Set_ActiveYeomgeom(_bool bActive) { m_bActiveYeongeom = bActive; }

	void						Set_SkillAttempt(bool b) { m_bIsSkillAttempt = b; }

	_bool						Get_ClimbUp() const { return m_bClimbUp; }
	_float3						Get_LadderTopPos() const { return m_vLadderTopPos; }
	_float3						Get_LadderBottomPos() const { return m_vLadderBottomPos; }
	Monster*					Get_TargetMonster() const { return m_pTargetMonster; }

	PLAYER_MESHTEX_NUM*			Get_PlayerMeshTexNumPtr() { return &m_tPlayerMeshTex; }
	/*컴포넌트가져오기*/
	class Player_Stat*			Get_PlayerStatCom() { return m_pPlayerStatCom; }
	class StateMachine*			Get_PlayerStateMachine() { return m_pStateMachineCom; }

	_bool Get_PlayerDead() const { return m_bPlayerDead; }
	void Set_PlayerDead(_bool _dead) { m_bPlayerDead = _dead; }

	void						Register_Collider_Player();
public:
	virtual HRESULT				Initialize_Prototype(LEVEL _level);
	virtual HRESULT				Initialize(void* arg) override;
	virtual _int				Update_Priority(const _float fTimeDelta) override;
	virtual _int				Update_Parallel(const _float fTimeDelta) override;
	virtual _int				Update(const _float fTimeDelta) override;
	virtual _int				Update_Late(const _float fTimeDelta) override;
	virtual HRESULT				Render(const _float fTimeDelta) override;



private:
	class Stat*					m_pStatCom = { nullptr };
	class Player_Stat*			m_pPlayerStatCom = { nullptr };
	class StateMachine*			m_pUpperStateMachineCom = { nullptr }; //상체용 스테이트머신, 기존 사용하던건 Character에있음
	class Player_MasterRig*		m_pMasterRig = { nullptr };
	class Player_Weapon*		m_pActiveWeapon = { nullptr };
	class Player_Weapon*		m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };
	class Player_BloodWeapon*	m_pActiveBloodWeapon = { nullptr }; // 특수무기
	WEAPON_TYPE					m_eCurrentWeaponType = { WEAPON_TYPE::WP_END };
	class Player_Outer*			m_pActiveOuter = { nullptr };
	class Player_Injection*		m_pActiveInjection = { nullptr };

private:
	// Sword Trail 관련 변수
	vector<TrailEffect*>		m_vecTrailEffects;
	_bool						m_bTrailActive = {};
	_float						m_fSpeed = 0.f;

	// Dissolve 관련 변수
	_bool						m_bDissolving = {};
	_float						m_fDissolveDir = {};		// 1 이면 사라지고 -1 이면 나타남
	_float						m_fDissolveMax = { 1.f };	// Dissolve 총 시간

	/* PhysX 캐릭터 충돌체용 */
	_float						m_fGravity      = 0.f;      // 누적될 중력 값
	_bool						m_bIsGrounded   = false;
	_bool						m_bEnablePhysics = false;

	_bool						m_bAttackSweepActive = { false };	// 공격중인지
	_float						m_fAttackRadius = { 0.5f };			// 공격 범위
	_float						m_fAttackDamage = { 10.f };			// 공격 데미지
	_float						m_fAttackHalfHeight = { 0.f };		// 공격 반높이
	_float3						m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
	_float3						m_vPrevWeaponPosEnd = {};				// 이 전 프레임 무기 위치2
	_float3						m_vCurrentWeaponPos = {};			// 현재 프레임 무기 위치
	_float3						m_vCurrentWeaponPosEnd = {};			// 현재 프레임 무기 위치2
	set<_uint>					m_setHitTargets = {};				// 이미 공격한 몬스터 저장
	_uint						m_iColliderEventHandle = {};		// 
	_float						m_fKnockbackForce = {};
	_float4x4*					m_pWeaponBoneMatrix = {};			// 무기 본 위치
	_float4x4*					m_pWeaponBoneMatrixEnd = {};		// 캡슐용 무기 본 위치
	_float4x4*					m_pWeaponTrailTip = {};
	_float4x4*					m_pWeaponTrailRoot = {};
	
	vector<_float4x4*>			m_vecWeaponBoneMatrices;		// 무기들 본 위치

	Model*						m_pMasterRigModel = { nullptr };
	Model*						m_pMasterModel = { nullptr };
	//플레이어 높이 보간 + 렌더용 분리
	_vector						m_vPhysicsPos = {}; //피직스 위치 저장
	_vector						m_vRenderPos = {}; //렌더용 위치 저장
	_float4x4					m_RenderWorldMatrix = {}; //렌더용 행렬 저장

	_float						m_fRenderFollowUpSpeed = {10.f}; //올라갈때 보간에 곱할 속도 
	_float						m_fRenderFollowDownSpeed = {13.f}; //내려갈때 보간에 곱할 속도
	_float						m_fVerticalVelocity = { 0.f }; //수직속도

	//플레이어 입력 관리
	INPUT_DESC					m_tInputDesc = {};
	_uint						m_iPlayerAnimEventHandle = {};
	PLAYERRUNTIMEEVENT_DESC		m_tPlayerRunTimeEvent = {};
	_bool						m_bApplyTranslation = { true };
	_vector						m_vCacheAttackMoveDir = {};
	_bool						m_bDebugControl = { false };

	//Guard 상태 일때 제어
	_bool						m_bOnGuard = { false };
	_vector						m_vSaveCameraLook = {}; //가드 시점에 카메라의 마지막 Look 방향벡터를 저장하기 위해

	//플레이어 스탯 제어
	_float						m_fRegenPerSecond = { 40.f }; //리젠속도
	_float						m_fDashDecrasePerSecond = { 20.f }; // 대쉬 감소속도
	_float						m_fRegenCoolTime = { 0.f }; // 리젠 쿨타임 제어
	_float						m_fFoucsGaugeDecreasePerSecond = { 5.0f }; // 포커스 게이지 감소 속도 (얘는 디폴트로 계속 감소하는놈)
	ANIM_FRAMEPHASE				m_eAnimPhase = {};
	_bool						m_bFocusState = { false };
	_bool						m_bIsDealingDamage = { false }; //데미지를 주는중인지 (데미지 이벤트 발행 성고하면 true)로

	//피격 상태 제어 
	_vector						m_vEnemyHitDirection = {}; //피격 이벤트시 값 받아오기
	DAMAGEPOWER					m_eEnemyDamagePower = {}; //피격 이벤트시 값 받아오기
	_float						m_fLoseHpRatio = {}; //잃은 체력 비율 받기 
	Alarm						m_tCinematicAlarm;

	//락온
	GameObject*					m_pLockOnTarget = { nullptr };
	_float						m_fFixDist = { 5.f };
	_float						m_fLockOnRotationSpeed = { 720.f };
	_float						m_fMaxLockOnTargetDist = { 40.f }; //락온대상 타겟 제한 거리
	_float						m_fLockOnShiftPressTime = { 0.f };
	_bool						m_bLockOnShift = { false };
	_float						m_fAttackAutoRotationDist = { 5.f }; //락온아닐때 회전보정 거리

	//패링 및 시네마틱 상태 제어
	_bool						m_bPlayCineamtic = { false };
	GameObject*					m_pBackStabTarget = { nullptr };
	_bool						m_bSuccessBackStab = { false };

	//바요네트 총구 위치 가져오기
	_float4x4*					m_pBayonetMuzzleMatrix = {}; //바요네트 총구 위치

	//세이브 포인트 상태 전용
	LEVEL						m_eSavePointLevel = {};
	_uint						m_iSavePointIndex = {};

	//스킬 관리
	_bool						m_bActiveYeongeom = { false };
	_float						m_fYeonmgeomRadius = { 3.3f };
	_float						m_fYeongeomTime = { 0.f };
	
	//플레이어 상호작용 + 전투상태 관리
	//_bool						m_bCombat = { false };
	//_int						m_iCombatCount = { };
	_bool						m_bClimbUp = { false };
	_float3						m_vLadderTopPos = {};
	_float3						m_vLadderBottomPos = {};

	//무기 체인지 맵핑 테이블
	UMAP<_uint, WEAPON_TYPE>	m_umapWeaponTable;
	_bool						m_bLadderAdditional = { false };
	_float						m_fLadderAdditional = {};


	_bool						m_bPlayerDead = false;
	_bool						m_bDAMAGEMODE = false;
	_float						m_fAdditionalDamage = 0.F;
	Alarm m_PlayerDeadAlarm;

public:
#ifdef _DEBUG
		void Add_Debug_WeaponCapsule(_float _radius = 1.f, _float3 _color = _float3(1.0f, 0.2f, 0.2f));
#endif // _DEBUG

public:
	void						Gather_Input(const _float fTimeDelta);//키입력 모으기 + 키입력 상태제어
	void						Apply_MoveAndRotation(_float fTimeDelta); //키입력 모은걸 토대로 실제 이동 및 회전
	void						Start_Dir_RotationLerp(); //시작해야하는 방향 알려주고
	void						Update_RotationLerp(_float fTimeDelta); //회전 행렬 보간 시작

	void						Apply_AnimEventCount(const PLAYERANIM_EVENT& PlayerAnimEvent, _int iSign); //이벤트 수집해서 카운트 증가로 정수형으로 _bool 변수 판단

	void						Update_Stamina(_float fTimeDelta); //플레이어 스태미나 업데이트
	void						Update_FocusGauge(_float fTimeDelta); //플레이어 집중 상태 업데이트 
	void						Decrease_Stamina(const PLAYERANIM_EVENT& PlayerAnimEvent); //스태미나 감소
	void						Update_UseItem(const PLAYERANIM_EVENT& PlayerAnimEvent); //이거 아이템 이벤트 로직용으로 만들어놓음

	void						OnDamaged(const DAMAGE_EVENT& DamageEvent); //데미지 받기 + 피격 상태들 제어
	void						Find_LockOnTarget(); //기존 몬스터 순회해서 사용하던 락온 찾기
	void						Find_BestLockOnTarget(); //피직스 오버랩으로 락온 찾기
	void						Find_BackStabTarget(); //뒤잡 대상 찾기
	void						Find_NearestMonsterAndRotation(_float fMaxAngle, _float fLerpDuration); //락온 아닐때도 플레이어 공격시 짧은 회전 보정

	void						Update_LockOn(_float fTimeDelta);


	_bool						Is_AttackingState(); //중간 회전 가능한 애들 체크용
	void						Sample_AttackMoveDir_FromInput(); //해당 시점에 키입력값 저장
	void						Start_AttackDir_RotationLerp(_float fMaxRotation, _float fLerpDuration);//해당 시점에 저장해놓은 입력값으로 회전

	void						Update_Weapon(); //무기,블러드 무기 활성,비활성 제어중
	void						Update_ChangeWeapon(const PLAYERANIM_EVENT& PlayerAnimEvent); //무기 체인지용
	void						Switch_Weapon(WEAPON_TYPE eChangeWeaponType); //무기 스위칭 

	void						Update_RenderMatrix();	//렌더용 위치 및 행렬 계산 
	void						Update_RenderPos(_float fTimeDelta);//렌더용 위치 및 행렬 계산 
	void						Create_Projectile(const PLAYERANIM_EVENT& PlayerAnimEvent);

	void						Update_AnimationSpeed(const PLAYERANIM_EVENT& PlayerAnimEvent);

	void						Enter_LadderState(const PlayerIntercation& e); //사다리 상태 진입 전 및 상태변경

	void						Enter_DeadState(); //죽었을때 초기화 및 세팅해야하는 값들 넣기
	void						Enter_CustomScene(); //커스텀 씬에서 세팅해야할것들
	void						Enter_MainScene(); //기본 씬에서 세팅해야할것들 
	_bool						Check_FocusStateSuperArmor() const; //집중상태 슈퍼아머 체크

	void						Setting_Damage();

	//스킬 관련
	_bool						CanUseSkill(); //키입력해서 스킬 사용가능한지 / 스킬 상태 전이
	void						Update_SkillEvent(const PLAYERANIM_EVENT& PlayerAnimEvent);
	void						Update_Yeongeomjeonmu(_float fTimeDelta);

public:
	INPUT_DIR					Calculate_Dir(_fvector vCurrent, _fvector vDesired); //8방향 내적,외적 구하기
	INPUT_DIR					Calculate_LockOnDir(_fvector vMoveDir, GameObject* pGameObject); //타겟 기준 8방향 구하기

	DAMAGE_LEVEL				Calculate_DamageLevel(_float fLoseHpRatio, DAMAGEPOWER eDamagePower); //피 잃은 비율과 데미지 파워에 따라서 레벨 정하기
	INPUT_DIR					Convert8To4Dir(INPUT_DIR eInputDir); //8방향->4방향으로 
	void						Sync_BoneMatrix(WEAPON_TYPE _type = WEAPON_TYPE::WP_END); //트레일에 사용하는 뼈행렬 액티브 무기로 뼈 동기화

	void						Change_State(_uint _state);

private:
	HRESULT						Ready_Components();
	HRESULT						Ready_PartObjects();
	HRESULT						Ready_States();
	HRESULT						Ready_Event();
	HRESULT						Ready_PhysXEvent();
	HRESULT						Ready_Skills();
	HRESULT						Bind_ShaderResources();
	void						Update_PlayerCustomDesc();

#pragma region PhysX 이동, 전투
	class Model*				Get_MasterRigModel();
	void						Update_WeaponPosition();
	void						Process_AttackSweep();
	void						Process_AttackSweep_Capsule();
#pragma endregion PhysX 이동, 전투


#pragma region 시네마틱
	void						Teleport_Oliver();
	void						Teleport_Wolf();
#pragma endregion 시네마틱


public:
	static Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

private:
	_bool		m_bTurn = true;
	_bool		m_bOpenUI = false;
	MinimapRenderComponent* m_pMinimapRendercom = nullptr;
	_bool		m_bCanInput = true;	// 입력 아예 방지

	class Collider* m_pColliderCom = { nullptr };
	Monster* m_pTargetMonster = { nullptr };

private:
	InteractionManager* m_pInteractionManager = nullptr;	
	PLAYER_SHADER_DESC m_tPlayerShaderDesc;
	_bool m_bOnetime = false;
	PLAYER_MESHTEX_NUM m_tPlayerMeshTex;
	vector<vector<_float4>> m_vInnerColor;
	_int m_iPrevBodyNum = 0;


private:
	bool		m_bIsSkillAttempt=false;//Requestskill호출시 바로 true, 쿨타임 감소이벤트나 실제스킬이벤트가 불리면 false만듬
									//UI이벤트 제어용
};
NS_END