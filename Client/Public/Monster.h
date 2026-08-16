#pragma once
#include "Client_Define.h"
#include "ContainerObject.h"

#include "Monster_Turn.h"
#include "Monster_Idle.h"
#include "Monster_Run.h"
#include "Monster_Walk.h"
#include "Monster_AttackNormal.h"


#include "PoolingManager.h"
#include "ParticleSystem.h"

#include "Monster_Hit_Defines.h"

NS_BEGIN(Engine)
class Model;
class Shader;
class Monster_Stat;
class StateMachine;
NS_END

NS_BEGIN(Client)

class MinimapRenderComponent;
class MState_TransitionTable;

class Monster abstract : public ContainerObject
{
    friend class IMGUI_MonsterEditor;
public:
    enum VISIBLE_STATE { VS_NONE, VS_VISIBLE, VS_INVISIBLE };
    
    typedef struct tagMonsterDesc : public GameObject::GAMEOBJECT_DESC
    {
        PHYSX_CONTROLLER_DESC tControllerDesc;

    } MONSTER_DESC;

    // 몬스터 공통 상태 (보스/쫄 공통)
    enum class ENEMYSTATE { PATROL, CHASE, ATTACK, RETURN, HIT, DEAD, END };

    //몬스터 상태 추가할때 꼭!!!!!한번말해주십쇼
    enum ENEMY_STATE
    {
        IDLE,
        PATROL,//새로운상태추가, 혹시 ENUM순서중요하면 말점.,,
        PATROL_TURN,//이미 MONSTER_TURN클래스가 PLAYER방향으로 무조건 턴하는걸로 고정되어있어서..수정하기 좀 그래서 그냥 패트롤전용 TURN추가
        AWAKE,
        WALK,
        RUN,
        DASH,
        DODGE,
        TURN,
        ATTACKNORMAL, //기본공격
        ATTACKSTRONG, //강공격
        HIT,
        DEAD,
        MON_END,//13
    };

    // 추가할때 MState_TransitionTable이랑 IMGUI도 수정 필요함
    /* 추가할때 MState_TransitionTable이랑 IMGUI도 수정 필요함(중요해서 한 번 더) */
    enum MONSTER_STATE {
        MS_IDLE, MS_PATROL, MS_PATROL_TURN, MS_AWAKE_IDLE,MS_AWAKE, MS_FALLBACK,
        MS_BATTLE, MS_WALK, MS_RUN, MS_HIT, MS_TURN,
        MS_ATTACKMELEE0, MS_ATTACKMELEE1, MS_ATTACKMELEE2, MS_ATTACKMELEE3, MS_ATTACKMELEE4,
        MS_ATTACKFAR0, MS_ATTACKFAR1, MS_ATTACKFAR2, MS_ATTACKFAR3, MS_ATTACKFAR4,
        MS_ATTACKSPECIAL0, MS_ATTACKSPECIAL1, MS_ATTACKSPECIAL2, MS_ATTACKSPECIAL3, MS_ATTACKSPECIAL4,
        MS_EVADE,MS_STUN, MS_CINEMATIC, MS_REPEL, MS_DOWN, MS_PHASECHANGE,
        MS_DEAD, MS_END
    };

    //                  감속      걷기    달리기   회피
    enum ACCEL_TYPE { ACC_DEC, ACC_WALK, ACC_RUN, ACC_EVADE,ACC_END };

    //모든 몬스터가 정찰하는것은 아니므로, 정찰유무를 판단하기위함
    enum class ENEMY_IDLE_TYPE { STATIC, PATROL, SLEEP };

    // 걷기 방향(애니메이션이 어디 방향으로 걸을지)
public: enum WALK_DIR { WALK_NONE = -1, WALK_FRONT = 0, WALK_LEFT, WALK_RIGHT, WALK_END };

        enum EAVDE_DIR {EVADE_LEFT=1,EVADE_RIGHT,EVADE_END};
    // 회전 타입
public: enum TURN_TYPE { TURN_WALK, TURN_RUN, TURN_ATTACK, TURN_END };
    // 스턴 타입
public: enum STUN_STATE { STUN_NONE = -1, STUN_LOOP = 0, STUN_BACKSTAB, STUN_END, STUN_WAKEUP, STUNSTATE_END };
    // 시네마틱 연출 타입
public: enum CINEMATIC_STATE {
    CINEMATIC_DRAPE_FULL, CINEMATIC_DRAPE_SHORT, CINEMATIC_DRAPE_FULL_BACKSTAB, CINEMATIC_DRAPE_SHORT_BACKSTAB, // DRAPE
    CINEMATIC_GAUNTLET_FULL, CINEMATIC_GAUNTLET_SHORT,
    CINEMATIC_LONGCOAT_FULL, CINEMATIC_LONGCOAT_SHORT,
    CINEMATIC_MUFFLER_FULL, CINEMATIC_MUFFLER_SHORT,
    CINEMATIC_END };
    // 다운 상태 타입
public: enum DOWN_STATE { DOWN_FRONT, DOWN_BACK, DOWN_LEFT, DOWN_RIGHT, DOWN_END };

protected:
    explicit Monster();
    explicit Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Monster(const Monster& original);
    virtual ~Monster() = default;



public:
    virtual HRESULT Initialize_Prototype(LEVEL _level);
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update_Priority(const _float fTimeDelta) override;
    virtual _int Update_Parallel(const _float fTimeDelta) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

    virtual void Update_AI(const _float fTimeDelta) {}

public:
    virtual _uint Select_NextPattern() { return IDLE; }
    void LookAt_Target();
    void LookAt_Smooth_Target(const _float fTimeDelta);


#pragma region Setter
    void Set_MoveDirectionToTarget();

    void Set_Animation_CS(_uint _animIndex, _float _animSpeed = 1.f, _bool _isLoop = false, _float _lerpTime = 0.2f);
    
    void Set_CanBackAttack(_bool _canBackAttack) { m_bCanBackAttack = _canBackAttack; }
    void Set_CanParry(_bool _canParrry) { m_bCanParry = _canParrry; }       // 패링 가능한 몬스터인지
    void Set_StunState(STUN_STATE _stunstate) { m_eStunState = _stunstate; }
    void Set_CinematicState(CINEMATIC_STATE _cinestate) { m_eCinematicState = _cinestate; }
    void Set_DownState(DOWN_STATE _downstate) { m_eDownState = _downstate; }
    void Set_IsAttacking(_bool _attacking) { m_bIsAttacking = _attacking; } // 몬스터가 공격중인 상태에 들어갔는지 세팅(Set_Attack은 공격 가능한 상태)
    void Set_Combo(_bool _combo) { m_bComboTriggeredThisFrame = _combo; }
    void Set_CurrentPattern(_uint _patternNum) { m_iCurrentPattern = _patternNum; }

    // 제곱값 캐싱으로 sqrt사용 줄이기
    void Set_Ranges_Square();
    void Set_NavDir(_fvector vDir) { XMStoreFloat3(&m_vNavDir, vDir); }
    void Set_SuperArmor(_bool bIsSuperArmor) { m_bIsSuperArmor = bIsSuperArmor; } // = ANS_SuperArmor
    void Set_AttackSweep(_bool _sweep) { m_bAttackSweepActive = _sweep; }

    void Set_ActionCancelable(_bool bCancelable) { m_bIsActionCancelable = bCancelable; }
    void Set_DisableLookAt(_bool bDisable) { m_bDisableLookAt = bDisable; }

    virtual HRESULT Set_Animation() { return S_OK; }
    void Set_Lifted(_bool _lift) { m_bLifted = _lift; } // 중력 적용 끄기용(시네마틱)

    void Set_TargetCached(_bool bTarget) { m_bTargetPosCached = bTarget; }

    void Set_TargetDirection(_fvector vDir) { XMStoreFloat3(&m_vTargetDirection, vDir); }
    void Set_TargetDirection(_float3 vDir) { m_vTargetDirection = vDir; }

    void Set_Battle(_bool _isBattle);
    void Set_TurnType(TURN_TYPE _type) { m_eTurnType = _type; }
    void Set_Attack(_bool _canAttack) { m_bCanAttack = _canAttack; }
    void Set_AttackDuration(_float _attackDuration) { m_fAttackDuration = _attackDuration; }

    void Set_Walk_Dir(_int _direction) { m_iWalkDirection = _direction; }
    void Set_RootPosAble(_bool _rootposOn) { m_bRootPosOn = _rootposOn; }
    void Set_Acceleration(ACCEL_TYPE _accel) { m_eAccelType = _accel; }

    void    Set_FirstAware(bool b) { m_bFirstAware = b; }
    void Set_WaveRimOn(_bool _MonsterWave) { MonsterWave = _MonsterWave; }
    void Change_State(_uint _stateNum);

    // 죽었따
    void Set_Kill() { m_bIsKilled = true; }
#pragma endregion Setter

    _bool Update_HomingRotation(const _float fTimeDelta, _float fRotationSpeedDegree);
    void Update_MoveCorrection(const _float fTimeDelta, _float fMaxDistance, _float fTargetOffset);
    void Check_TurnInterrupt(_float fYawThreshold, _float fRotationSpeed, _float fTimeLimit);
    void Check_ComboTransition(_uint iProbabilityRate, _float fMaxDistance, _uint iNextState);

    void Save_TargetPosition();


#pragma region Getter
    _bool  Is_Killed() const { return m_bIsKilled; }    // 죽었니 살았니
    _bool  Is_Animation_Finished() { return m_pModelCom->Is_AnimFinished(); }
    _bool  Is_Loop_Animation() { return m_pModelCom->Is_Loop_Anim(); }
    _bool  Is_Grounded() { return m_bIsGrounded; }
    _bool  Is_Attacking() const { return m_bIsAttacking; }    // 몬스터가 공격중이니?
    _bool  Can_Attack() const { return m_bCanAttack; }
    _bool  Can_BackAttack() const { return m_bCanBackAttack && m_bBackAttackMonster; }
    _bool  Can_Parry() const { return m_bCanParry; }
    _bool  Check_ComboAttack() const { return m_bComboTriggeredThisFrame; }

    _float Get_HPRatio() { return m_fCurrentHp / m_fMaxHp * 100.f; }
    _float Get_RandomValue() { return m_fRandomValue; }

    class Monster_Weapon* Get_ActiveMonsterWeapon() { return m_pActiveWeapon; }

    _float Get_WalkRange() const { return m_fWalkRange; }
    _float Get_DetectRange() const { return m_fDetectRange; }
    _float Get_ChaseRange() const { return m_fChaseRange; }
    _float Get_AttackRange() const { return m_fAttackRange; }
    _float Get_AttackRange_Melee() const { return m_fAttackMeleeRange; }
    _float Get_AttackRange_Far() const { return m_fAttackFarRange; }
    _float Get_DistanceToTarget();

    _float Get_WalkRangeSq() const { return m_fWalkRangeSq; }
    _float Get_DetectRangeSq() const { return m_fDetectRangeSq; }
    _float Get_ChaseRangeSq() const { return m_fChaseRangeSq; }
    _float Get_AttackRangeSq() const { return m_fAttackRangeSq; }
    _float Get_AttackRange_MeleeSq() const { return m_fAttackMeleeRangeSq; }
    _float Get_AttackRange_FarSq() const { return m_fAttackFarRangeSq; }
    _float Get_DistanceSq() const { return m_fDistanceSq; }

    _int    Get_Walk_Dir() const { return m_iWalkDirection; }
    _float  Get_WalkSpeed() const { return m_fWalkSpeed; }
    _float  Get_RunSpeed() const { return m_fRunSpeed; }
    _float  Get_Fov() const { return m_fFOV; }

    ENEMY_IDLE_TYPE     Get_IdleType() const { return m_eIdleType; }
    physx::PxController* Get_Controller() const { return m_pController; }
    _vector Get_PlayerPos() const;
    _float Get_TurnSpeed() const { return m_fTurnSpeed; }
    _float Get_TurnTimeLimit() const { return m_fTurnTimeLimit; }
    _float3 Get_DirectionNorm() const { return m_vDirectionNorm; }
    _vector Get_SpawnPos() const { return m_vSpawnPos; }
    // 위에껀 vector라 Stage1몬스터만 씀(터질 위험)
    _float3 Get_SpawnPosition() const { return m_vSpawnPosition; }
    STUN_STATE Get_StunState() const { return m_eStunState; }
    CINEMATIC_STATE Get_CinematicState() const { return m_eCinematicState; }
    DOWN_STATE Get_DownState() const { return m_eDownState; }

    _float* Get_CurrentHp() { return &m_fCurrentHp; }
    _float* Get_MaxHp() { return &m_fMaxHp; }

    _float3     Get_TargetDirection() const { return m_vTargetDirection; }
    _bool       Get_AnimCancelable() const { return m_bIsActionCancelable; }
    _bool       Is_Battle() const { return m_bIsBattle; }
    _bool       Is_FirstAware() { return !m_bFirstAware; }

    _uint Get_CurrentPattern() const { return m_iCurrentPattern; }
    _uint Get_PrevPattern() const { return m_iPrevPattern; }
    const _float4x4* Get_SocketMatrix(const _char* pBoneName);
    const _float4x4* Get_SocketMatrix(_int _boneNum);

    MState_TransitionTable* Get_TransitionTable() { return m_pTransitionTable; }

    HIT_LEVEL           Get_HitLevel() { return m_eHitLevel; }
    HIT_DIR             Get_HitDir() { return m_eHitDir; }

    //이거 몬스터들 마다 크기가 달라서 라디우스로 동행자 행동 범위 체크용으로 가져갑니다 (이제 이 라디우스는 제겁니다.)
    _float              Get_ControllerRadius() const { return m_pController ? m_pController->getRadius() : 0.f; }
#pragma endregion Getter


#pragma region 몬스터 AI
public:
    _bool Detect_Player() const;      // 플레이어가 탐지 범위 안에 들어왔니?
    _bool Chase_Player() const;       // 플레이어가 추격 범위 안에 있니?
    
    void Update_Speed(const _float fTimeDelta); // 자연스러운 움직임을 위해 이동속도 조정(가속과 감속)
    void Move(const _float fTimeDelta);
    void Move_Smooth(const _float fTimeDelta);

    // 내 전방 시야 기준 플레이어는 어디쯤 각도에 있니?
    // radian true면 라디안 반환, 아니면 degree
    _float Calculate_Angle_To_Player(_bool _radian = false);

    // 정찰관련(State쪽에서 호출할것이므로 public)
    // 다음정찰위치 업데이트
    void        Update_Patrol_Target();
    //정찰위치까지와 현재위치 거리 
    _float      Get_DistanceToPatrolPoint() const;

    void        LookAt_PatrolPoint(_float fRotationSpeed, const _float fTimeDelta);
    void        Set_MoveDirectionToPatrolPoint();

    //detecrange에 들어왔다면,시야각판단
    bool        Detect_With_FOV();

    //정찰하면서 매프레임 raycast를 이용해 낭떠러지안떨어지도록 검사.
    _bool        Check_Floor_By_Raycast(_float fCheckDist);

    // 플레이어와 몬스터 사이의 거리와 방향 구해주는 함수
    void        Calculate_To_PlayerDistance_And_Direction();
    _bool		Ensure_PlayerCached();	// 플레이어 캐싱 및 null 체크 통합

    _bool       Ensure_CompanionCached();

    // 플레이어와 정찰지점 사이의 거리와 방향
    void        Calculate_To_PatrolDistance_And_Direction();
    // 랜덤값 반환(패턴 결정을 위해 0 ~ 100 사이의 float값 반환)
    _float      Roll_Random();

    void        Compute_CumulativeDamage(const _float fTimeDelta);

    /*UI에 쓰일 판단용*/
    virtual         void    OnAwarePlayer() {};    //몬스터가 플레이어를  감지한 첫 순간만 발동하는함수(보통 보스를 위해 사용)
    virtual         void    OnDead() {};

    virtual void Change_Phase() {}

#pragma endregion 몬스터 AI


#pragma region 상태 머신
    struct StateMachineDesc {

    };
    // 상태 머신 컴포넌트 추가딸깍
    HRESULT Add_StateMachine();
    // 상태 추가 딸깍
    HRESULT Add_State(MONSTER_STATE _stateNum_or_Name, _uint _animIndex, _float _animSpeed = 1.f, DAMAGEPOWER _damagePower = DAMAGEPOWER::POWER_END, _float _lerpTime = 0.2f);
    HRESULT Add_State(MONSTER_STATE _stateNum_or_Name, _uint _animIndex, StateMachine* _table, _float _animSpeed = 1.f, DAMAGEPOWER _damagePower = DAMAGEPOWER::POWER_END, _float _lerpTime = 0.2f);
    // 상태에 보조 애니메이션 추가
    HRESULT Add_Additional_Animation(_uint _stateNum, _uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop = false, _float _lerpTime = 0.2f);
    // 전이 테이블 로드
    HRESULT Load_TransitionTable(const wstring& _filePath);
    // 전이 테이블 업데이트
    void    Update_Transition();
#pragma endregion 상태 머신


protected:
    // 가상함수
    virtual HRESULT Ready_Components();
    virtual HRESULT Ready_States();
    virtual HRESULT Ready_Events();

    HRESULT Add_Minimap_Monster(bool bBoss=false);
    HRESULT Add_Stat_Monster(_float _maxHP, _uint _damage, _uint _defence, _uint _haze); // 스탯 컴포넌트 추가

    void			Update_WeaponPosition();        // 무기 콜라이더 위치 업데이트(스피어)
    void            Update_WeaponPosition_Capsule();// 무기 콜라이더 위치 업데이트(캡슐)
    void			Process_AttackSweep();
    void            Process_AttackSweep_Capsule();


    virtual void OnDamaged(const DAMAGE_EVENT& _damageEvent);
    virtual void OnSpawn(void* _arg) override;
    virtual void OnDespawn() override;

    void Spawn_FieldItem(const wstring& strItemName);

    HIT_DIR         Calculate_HitDir();

#pragma region 몬스터 UI
protected:
    /*HP바 관련*/
    //HP바세팅 및 연결
    HRESULT        Setup_MonsterStatusBar(_float3 vOffSet);

    //월드 hp바가있는 몬스터들만 정의 (혹시몰라서 가상함수) / 거리에 따라 hp바가 보여질건지아닌지 이벤트보낸다.
    virtual void Set_Visible_MonsterUI(_float fDistance);

    /*죽을때 랜덤 아이템떨구기*/
    void            Get_Item_OnDead(bool bBoss = false);

 #pragma endregion 몬스터 UI


#pragma region 몬스터 이펙트 EFFECT
    void Hit_Effect_Slash(const _float3& _position);
    void Hit_Effect_Blood(const _float3& _position);
    virtual void SpawnBloodDecal(const DAMAGE_EVENT& _damageEvent);

#pragma endregion 몬스터 이펙트 EFFECT


#ifdef _DEBUG
    void Add_Debug_WeaponCapsule(_float _radius = 1.f, _float3 _color = _float3(1.0f, 0.2f, 0.2f));
#endif // _DEBUG


protected:
    class Player*       m_pPlayer = nullptr;
    class Yakumo*       m_pCompanion = { nullptr };
    _uint               m_iAnimEventHandle = {};

    //물리
    _float               m_fGravity = 0.f;
    _bool                m_bIsGrounded = false;
    _bool                m_bEnablePhysics = false;
    Vector3              m_vNavDir = { 0.f, 0.f, 0.f };

    _vector m_vCachedTargetPos = {};
    _bool   m_bTargetPosCached = false;


    _bool m_bFirstAware = false;    //첫 감지 판단
//무기/공격이벤트용
protected:
    _uint       m_iColliderEventHandle = {};
    _float		m_fKnockbackForce = {};

    const _float4x4* m_pWeaponBoneMatrix = {};			// 무기 본 위치
    const _float4x4* m_pWeaponBoneMatrixEnd = {};			// 무기 본 위치
    _float4x4* m_pWeaponTrailTip = {};
    _float4x4* m_pWeaponTrailRoot = {};
    class Monster_Weapon* m_pActiveWeapon = { nullptr };
    class Monster_Weapon* m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };

//bp에서 사용하는 값들, 우리 범위에따라 간략화할건 빼도됨
protected:
    _float3     m_vTargetDirection = {};    //TURN상태에서 쓸 direction..?

    _float m_fAttackRange = 5.f;          // 기존에 쓰던 AttackRange(이미 쓰던거라 함부로 못 바꿈)
    _float m_fAttackRangeSq = {};         // 위에꺼 제곱

    // 슈아/그로기등
    _float m_fMaxGrit = 0.f;                // MaxFastness(강인도)
    _float m_fCurrentGrit = 0.f;        
    _bool  m_bSuppressDown = false;         // bSuppressDown(다운/뒤잡기 면역)
    _bool  m_bIsSuperArmor = false;         // 공격 모션 중 경직 무시용 플래그

protected:
    _vector m_vSpawnPos = {};       // 옛날에 쓰던거

    _float3 m_vRootMotionDelta = { 0.f, 0.f, 0.f };
    _float3 m_vAnimChaseTrans = {}; // 애니메이션 진행시 쫒아갈지

    _bool m_bIsActionCancelable = true; // ANS_Stiff (선후딜비슷한...무ㅏㄴ가)
    _bool m_bDisableLookAt = false;     // ANS_EnemyAIDisableLookAt (시선 추적 끄기)
    _vector m_vSavedTargetPos = {};

    _float      m_fMeshXOffset = {};
    _float      m_fMeshYOffset = 0.f;
    _bool       m_bLastBoss = false;

    VISIBLE_STATE        m_PreStatusVisible = VISIBLE_STATE::VS_NONE;

    /* 누적 데미지 구조체 */
    typedef struct tagCumulativeDamage {
        _float fElapsedTime = {};
        _float fDamage = {};
    }CUMULATIVEDAMAGE;

    _bool m_arrStateUsed[MS_END] = {};

public:
    _bool Is_StateUsed(_uint _iState) const { return m_arrStateUsed[_iState]; }
    void Set_StateUsed(_uint _iState) { m_arrStateUsed[_iState] = true; }

protected:
    //////////////////////////////////////////////
    /*               필수 정보들                */
    //////////////////////////////////////////////
    
    /* 스탯 */
    Monster_Stat* m_pStatCom = { nullptr };
    _float m_fMaxHp = 100.f;
    _float m_fCurrentHp = 100.f;
    _float m_iTotalDamage = 0.f;         //누적 데미지값 (UI표시용도)
    _uint m_iDefence = 0;

    /* 이동 및 회전 속도 */
    _bool  m_bRootPosOn = true;            // 루트모션 받을건지
    _bool  m_bIsLunge = false;             // 돌진 공격인지(공격하면서 앞으로 추가 전진)
    TURN_TYPE  m_eTurnType  = TURN_TYPE::TURN_END; // 방향 전환 상태
    ACCEL_TYPE m_eAccelType = ACCEL_TYPE::ACC_DEC; // 걷기, 달리기, 멈추기 상태 확인
    _float m_fCurrentSpeed = 0.f;          // 현재 속도
    _float m_fWalkSpeed = 1.f;             // 걷기 최대 속도
    _float m_fWalkAcceleration = 1.f;      // 걷기 가속도
    _float m_fRunSpeed = 5.f;              // 달리기 최대 속도
    _float m_fRunAcceleration = 3.f;       // 달리기 가속도
    _float m_fEvadeSpeed = 15.f;             //회피 속도
    _float m_fEvadeAccelration = 30.f;      //회피 가속도
    _float m_fNormalRotationSpeed = 1.f;   // RotateSpeed 평상시
    _float m_fChaseRotationSpeed = 2.f;    // RotateSpeed 추격시
    _float m_fTurnSpeed = 0.f;             // 턴 인터럽트 시 회전 속도
    _float m_fTurnTimeLimit = 0.f;         // 턴 인터럽트 최대 허용 시간
    _float m_fLungeDistance = 0.f;         // 돌진 공격 추가 이동 거리
    _float m_fLungeOffsetSq = 0.f;         // 돌진 공격시 타겟과의 거리 비교(타겟이 오프셋보다 멀면 더 크게 이동)
    /* 턴 인터럽트란? 공격이나 어떤 모션중에 회전을 할 지 결정하는것 */

    /* 공격 */
    _bool       m_bAttackSweepActive = { false };	// 공격중인지
    _bool       m_bComboTriggeredThisFrame = { false }; // 연계공격 체크
    _float      m_fAttackRadius = { 0.5f };			// 공격 범위
    _float      m_fAttackDamage = { 10.f };			// 공격 데미지
    _float      m_fAttackHalfHeight = { 0.f };		// 공격 반높이
    _float      m_fMinAttackDuration = { 1.f };     // 공격 타이머
    _float      m_fMaxAttackDuration = 3.f;         // 공격 타이머
    _float3     m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
    _float3     m_vPrevWeaponPosEnd = {};			// 이 전 프레임 무기 위치
    _float3     m_vCurrentWeaponPos = {};			// 현재 프레임 무기 위치
    _float3     m_vCurrentWeaponPosEnd = {};		// 현재 프레임 무기 위치
    set<_uint>  m_setHitTargets = {};				// 이미 공격한 적 저장

    /* 상태 */
    class StateMachine* m_pStateMachineCom = nullptr;   // 상태 머신
    MState_TransitionTable* m_pTransitionTable = { nullptr };   // 전이 테이블
    _float      m_fDeadTimer = {};      // 죽은 뒤 일정시간 지나면 Pool에 반납
    _bool       m_bIsKilled = false;    // 몬스터가 처치 당해서 poolManager로 Despawn될건지
    _bool       m_bIsBattle = false;    // 전투상태에 돌입했는지
    _float      m_fRandomValue = {};    // 랜덤값 체크용
    _bool       m_bBackAttackMonster = false;    // 뒤잡 가능한 몬스터인지
    _bool       m_bCanBackAttack = true;    // 플레이어 뒤잡 체크
    _bool       m_bCanParry = false;    // 패링 가능한 몬스터인지
    _bool       m_bLifted = false;      // 플레이어에 의해 들어올려진 상태인지(중력 끄기용)
    STUN_STATE  m_eStunState = { STUNSTATE_END };    // 몬스터의 스턴 상태
    CINEMATIC_STATE m_eCinematicState = { CINEMATIC_END };  // 어떤 연출 할건지
    DOWN_STATE  m_eDownState = { DOWN_END };  // 엎어져있니 일어나있니
    _bool       m_bIsAttacking = false; // 이 몬스터는 공격중이니?

    /*Hit관련*/
    HIT_LEVEL               m_eHitLevel = HIT_LEVEL::HIT_END;
    HIT_DIR                 m_eHitDir = HIT_DIR::END;
    bool                    m_bIgnoreNormalHit = false;
    _float      m_fMaxCumulativeTime = { 2.f };         // 누적 데미지는 몇초동안 누적될거니
    _float      m_fTargetCumulativeDamage = { 500.f };  // 누적 데미지가 얼마정도 되면 Hit애니메이션 재생할거니
    deque<CUMULATIVEDAMAGE> m_dequeCumulativeDamage;    // 누적 데미지 보관용 컨테이너

    /* 거리 및 범위 */
    _float      m_fWalkRange = 10.0f;           // 어디쯤부터 간 보면서 걸을지
    _float      m_fDetectRange = 40.0f;         // IDLE상태일때 어디까지 탐지할지
    _float      m_fChaseRange = 50.0f;          // BATTLE상태일 때 어디까지 쫒아갈지
    _float      m_fAttackMeleeRange = 1.f;      // 근접공격 시작 범위
    _float      m_fAttackFarRange = 0.f;        // 원거리공격 범위
    /* 거리 및 범위 연산량 줄이기용 제곱 */
    _float      m_fWalkRangeSq = {};           // 위에꺼 제곱
    _float      m_fDetectRangeSq = {};	      // 위에꺼 제곱
    _float      m_fChaseRangeSq = {};	      // 위에꺼 제곱
    _float      m_fAttackMeleeRangeSq = {};    // 위에꺼 제곱
    _float      m_fAttackFarRangeSq = {};      // 위에꺼 제곱

    /* 플레이어 상호작용 */
    _float3		m_vDirection = {};		    // 플레이어에게의 수평 방향
    _float3		m_vDirectionNorm = {};	    // 정규화된 수평 방향 (Calculate에서 캐싱)
    _float		m_fDistance = {};		    // 플레이어와의 거리
    _float		m_fDistanceSq = {};		    // 플레이어와의 거리 제곱(sqrt 줄이기)
    _bool		m_bIsSquared = { false };	// 제곱값 캐싱 안 했을때 방지

    /* AI 제어 및 상태 관리 */
    _float3 m_vSpawnPosition = {};  // OnSpawn시에 쓸 위치 // 위에 하나 더 있는데 _vector로 하면 터질수 있음
    _bool   m_bCanAttack = true;    // 공격 가능한지
    _float  m_fAttackTimer = {};    // 공격 타이머
    _float  m_fAttackDuration = {}; // 다음 공격까지 딜레이
    _int    m_iWalkDirection = { -1 };  // 걷기 방향

    /* 시야각 */
    _float      m_fFOV = 120.f;
    _float      m_fDetectCosHalf = {}; // 시야 코사인값 캐싱(내적 결과와 비교하려면 cosf함수가 한 번 필요하기 때문에 미리 해둠)
    _bool       m_bIgnoreFOV = false;

    /* 정찰관련 */
    _float3         m_vPatrolTargetPos;
    _float          m_fPatrolRange = 10.f;      // 정찰반경
    _float3		    m_vPatrolDirection = {};	// 패트롤 위치 방향
    _float3		    m_vPatrolDirectionNorm = {};// 정규화된 수평 방향 (Calculate에서 캐싱)
    _float		    m_fPatrolDistance = {};		// 패트롤 위치와의 거리
    _float		    m_fPatrolDistanceSq = {};   // 패트롤 위치와의 거리 제곱(sqrt 줄이기)

    /* 패턴(보스용) */
    _uint m_iCurrentPattern = {};	// 현재 패턴(중복 사용 방지용)
    _uint m_iPrevPattern = {};		// 이전 패턴(중복 사용 방지용)

    /* 미니맵에 띄우기 */
    MinimapRenderComponent* m_pMinimapRenderCom = { nullptr };

    /* Emissive UI위치*/
    _float2 m_vEmissionUV;

    /* Emissive UI위치*/
    _bool MonsterWave = false;

    // Dissolve 관련 변수
    _bool						m_bDissolving = {};
    _float						m_fDissolveDir = {};		// 1 이면 사라지고 -1 이면 나타남
    _float						m_fDissolveMax = {};	    // Dissolve 총 시간

protected:
    ENEMY_IDLE_TYPE     m_eIdleType = ENEMY_IDLE_TYPE::STATIC;     
public:
	void Free() override;

    static _uint SwordCount;

};

NS_END