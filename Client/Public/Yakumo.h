#pragma once
#include "Client_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class Model;
class TrailEffect;
class StateMachine;
class Shader;
class Stat;
class Player_Stat;
NS_END


NS_BEGIN(Client)
class Yakumo : public ContainerObject
{
public:
    typedef struct tagYakumoDesc : public GameObject::GAMEOBJECT_DESC
    {
        PHYSX_CONTROLLER_DESC tControllerDesc;

    }YAKUMO_DESC;

public:
    enum YAKUMOSTATE
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
        SPECIALATTACK,	//무기별 고유 액션 10 
        DAMAGE,			//기본 피격 상태 11
        DAMAGEBLOW,		//피격 날라가는 모션 (Blow,Strike,Loop)12
        DAMAGEEND,		//피격시 일어나는 모션13
        BUDDY,          //플레이어와 상호작용하는 플레이어 스킬(안씀) 14
        GIFTHEAL,       //플레이어에게 투사체(회복) 날리는 모션 15
        DEATH,          //죽음
        END
    };

    enum FOUR_DIR //4방향
    {
        FRONT,
        BACK,
        RIGHT,
        LEFT,
    };

    typedef struct tagYakumoAnimRunTimeEvent
    {
        _int iInputAreaCount = 0; //입력 받을수 있는 구간
        _int iCanMoveCount = 0; //이동할수있는구간
        _int iCanComboCount = 0; //공격할수있는구간
        _int iCanEscapeCount = 0; //회피기 사용할수있는구간
        _int iCanSuperArmorCount = 0; //슈퍼아머구간
        _int iCanInvincible = 0; //무적 구간

        _bool bInputArea() const { return iInputAreaCount > 0; }
        _bool bCanMove()  const { return iCanMoveCount > 0; }
        _bool bCanCombo() const { return iCanComboCount > 0; }
        _bool bCanEscape()const { return iCanEscapeCount > 0; }
        _bool bCanSuperArmor() const { return iCanSuperArmorCount > 0; }
        _bool bCanInvincible() const { return iCanInvincible > 0; }

    }YAKUMORUNTIMEEVENT_DESC;

    typedef struct tagYakumoAICommandDesc
    {
        YAKUMOSTATE     eReserveAction = { YAKUMOSTATE::END }; //예약 행동
        _vector         vMoveDir = {}; //방향
        _bool           bHasCommand = { false }; //예약있는지 체크
        _int            iProbability = 0; 

        void Reset() //리셋용
        {
            eReserveAction = { YAKUMOSTATE::END }; //예약 행동
            bHasCommand = { false }; //예약있는지 체크
            iProbability = 0;
        }

    }AICOMMAND_DESC;

    typedef struct tagAITargetInfo
    {
        GameObject*     pBattleTarget = { nullptr }; //현재 전투 대상
        _float          fDistToTarget = { 0.f }; //적과의 거리
        _float          fDistToPlayer = { 0.f }; //플레이어와의 거리
        _vector         vDirToTarget = {}; //적 방향
        _vector         vDirToPlayer = {}; //플레이어 방향
        _bool           bCombat = { false }; //전투 중인지

    }AITARGET_INFO;

private:
    explicit Yakumo();
    explicit Yakumo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Yakumo(const Yakumo& original);
    virtual ~Yakumo();

public:         
    void                        Set_Position_ByController(_fvector vWorldPos); //피직스 위치 갱신
    YAKUMORUNTIMEEVENT_DESC&    Get_RunTimeEvent()  { return m_tAnimRunTimeEvent; }
    AICOMMAND_DESC&             Get_AICommandDesc() { return m_tAICommandDesc; }
    AITARGET_INFO&              Get_AITargetInfo() { return m_tAITargetInfo; }
    void						Set_ApplyTranslation(_bool bApplyTranslation) { m_bApplyTranslation = bApplyTranslation; } //루트모션 이동 제어
    void						Reset_RunTimeAnimEventDesc(); //이벤트 애니메이션 바뀔때 리셋해야할것들
    _vector						Get_EnemyHitDirection() const { return m_vEnemyHitDirection; }
    DAMAGEPOWER					Get_EnemyDamagePower() const { return m_eEnemyDamagePower; }
    _float						Get_LoseHpRatio() const { return m_fLoseHpRatio; }
    _float                      Get_AdjustAttackRange() const { return m_fAdjustAttackRange; }
    void                        Set_EvaluateCoolTime(_float fEvaluateCoolTime) { m_fEvaluateCoolTime = fEvaluateCoolTime; }
    _float4x4*                  Get_LeftHandMatrix() const { return m_pLeftHandMatrix; }

    /*컴포넌트가져오기*/
    class Player_Stat*          Get_YakumoStatCom() { return m_pYakumoStatCom; }

public:
    virtual HRESULT				Initialize_Prototype(LEVEL _level);
    virtual HRESULT				Initialize(void* arg) override;
    virtual _int				Update_Priority(const _float fTimeDelta) override;
    virtual _int				Update_Parallel(const _float fTimeDelta) override;
    virtual _int				Update(const _float fTimeDelta) override;
    virtual _int				Update_Late(const _float fTimeDelta) override;
    virtual HRESULT				Render(const _float fTimeDelta) override;
    virtual HRESULT             Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
    _bool		                Ensure_PlayerCached();	// 플레이어 캐싱 및 null 체크 통합
    void                        Update_Trail();
    void                        Move_Smooth(const _float fTimeDelta);
    void                        Update_Rotation(const _float fTimeDelta);
    void						OnDamaged(const DAMAGE_EVENT& DamageEvent); //데미지 받기 + 피격 상태들 제어
    void						Apply_AnimEventCount(const PLAYERANIM_EVENT& PlayerAnimEvent, _int iSign); //이벤트 수집해서 카운트 증가로 정수형으로 _bool 변수 판단
    void						Update_AnimationSpeed(const PLAYERANIM_EVENT& PlayerAnimEvent);
    void                        Update_Stamina(_float fTimeDelta);
    void						Create_Projectile(const PLAYERANIM_EVENT& PlayerAnimEvent);


public:
    FOUR_DIR                    Calculate_FourDir(_fvector vCurrent, _fvector vDesired);
    void                        Enter_DeadState(); //죽었을때 초기화할값들
    void                        Enter_AppearSetting(); //부활 or 다시 야쿠모 등장시킬때 초기화할값들
    //AI로직 함수들 
private:
    void                        Update_AITarget(); // AITARGET_INFO 구조체 값 채워주기 
    void                        Find_AITarget(); //몬스터 순회해서 Target 찾기
    void                        Teleport_ToPlayer();
    void                        Update_HealToPlayer();
    void                        Update_SavePlayerPos();
    void                        Update_TeleportToPlayer();
public:
    void                        Reset_AICommand();
    void                        Evaluate_AI(); //여기서 AI 행동 결정
    void                        Evaluate_Follow_Player(); //플레이어 따라가기 평가
    _bool                       Evaluate_Dodge(); //회피 판단
    _bool                       Evalute_Attack(); //공격 판단
    _bool                       Evaluate_Approach(); //접근 판단 
    FOUR_DIR                    Calculate_FourMoveDir(_fvector vMoveDir);

    void				        Set_Animation(_uint iAnimationIndex, _bool bIsLoop = false, _float fLerpDuration = 0.2f, _float fAnimationSpeed = 1.0f); //기존 전신 애니메이션 
    //AI로직 변수들
    virtual class Stat*         Get_StatComponent() const { return m_pStatCom; }

#pragma region 시네마틱
    void                        Cinematic_Oliver();
    void                        Cinematic_Change_Speed();
    void                        Cinematic_Teleport_Again();
    void                        Cinematic_Oliver_End();

#pragma endregion 시네마틱

private:
    YAKUMORUNTIMEEVENT_DESC     m_tAnimRunTimeEvent;
    AITARGET_INFO               m_tAITargetInfo;
    AICOMMAND_DESC              m_tAICommandDesc;
    _float                      m_fDetectRange = { 10.f }; //탐지 범위
    _float                      m_fDetectRangeSq = { 100.f };
    _float                      m_fEvaluateCoolTime = {0.f}; //평가 쿨타임 

    //비전투
    _float                      m_fWalkRange = {3.f}; //걷기 범위
    _float                      m_fRunRange = {5.0f}; //뛰기 범위
    _float                      m_fDashRange = {7.0f}; //빨리 뛰기 범위
    _float                      m_fMaxPlayerDist = { 15.f }; //플레이어와 위치 리셋 거리
    _float                      m_fAppearDist = { 4.0f }; //플레이어 뒤에서 나타날때의 제한거리
    //전투
    _float                      m_fAttackRange = { 2.0f }; //공격 범위
    _float                      m_fDodgeRange = { 3.0f }; //회피 범위
    _float                      m_fFrontRange = { 4.0f }; //전방으로 회피 가동 범위
    _float                      m_fApproachRange = { 4.0f }; //앞으로 직진만 할수있는 범위
    _float                      m_fMaxYDifferece = { 3.0f }; //Y축 맥시멈 제한 거리

    //몬스터의 Raidus와 보정된 범위 체크용
    _float                      m_fAdjustAttackRange = { 0.f }; //공격 범위
    _float                      m_fAdjustDodgeRange = { 0.f }; //회피 범위
    _float                      m_fAdjustFrontRange = { 0.f }; //전방으로 회피 가동 범위
    _float                      m_fAdjustApproachRange = { 0.f }; //앞으로 직진만 할수있는 범위

private:
    class Yakumo_Weapon*        m_pActiveWeapon = { nullptr };
    class Player*               m_pPlayer = nullptr;
    class Stat*                 m_pStatCom = { nullptr };
    class Player_Stat*          m_pYakumoStatCom = { nullptr };

private:
    // Sword Trail 관련 변수
    vector<TrailEffect*>		m_vecTrailEffects;
    _bool						m_bTrailActive = {};

    //이동 관련
    _float3                     m_vRootMotionDelta = {};
    _float                      m_fSpeed = { 5.f };
    _float                      m_fYOffset = { 0.05f };

    // Dissolve 관련 변수
    _bool						m_bDissolving = {};
    _float						m_fDissolveDir = {};		// 1 이면 사라지고 -1 이면 나타남
    _float						m_fDissolveMax = { 5.f };	// Dissolve 총 시간

    /* PhysX 캐릭터 충돌체용 */
    _float						m_fVerticalVelocity = 0.f;  //수직 속도
    _bool						m_bIsGrounded = false;
    _bool						m_bEnablePhysics = false;

    _bool						m_bAttackSweepActive = { false };	// 공격중인지
    _float						m_fAttackRadius = { 0.5f };			// 공격 범위
    _float						m_fAttackDamage = { 250.f };	    // 공격 데미지
    _float						m_fAttackHalfHeight = { 0.f };		// 공격 반높이
    _float3						m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
    _float3						m_vPrevWeaponPosEnd = {};				// 이 전 프레임 무기 위치2
    _float3						m_vCurrentWeaponPos = {};			// 현재 프레임 무기 위치
    _float3						m_vCurrentWeaponPosEnd = {};			// 현재 프레임 무기 위치2
    set<_uint>					m_setHitTargets = {};				// 이미 공격한 몬스터 저장
    _uint						m_iColliderEventHandle = {};		// 
    _float						m_fKnockbackForce = {};
    _float4x4*                  m_pWeaponBoneMatrix = {};			// 무기 본 위치
    _float4x4*                  m_pWeaponBoneMatrixEnd = {};		// 캡슐용 무기 본 위치
    _float4x4*                  m_pWeaponTrailTip = {};
    _float4x4*                  m_pWeaponTrailRoot = {};
    _uint                       m_iYakumoAnimEventHandle = {};

    //상태 제어 
    _bool                       m_bApplyTranslation = { true }; // 루트모션 제어
    _vector						m_vEnemyHitDirection = {}; //피격 이벤트시 값 받아오기
    DAMAGEPOWER					m_eEnemyDamagePower = {}; //피격 이벤트시 값 받아오기
    _float						m_fRegenPerSecond = { 50.f }; //리젠속도
    _float						m_fLoseHpRatio = {}; //잃은 체력 비율 받기 
    _float4x4*                  m_pLeftHandMatrix = { nullptr }; //왼손 소켓행렬
    _uint                       m_iMaxGiftHealCount = {2}; //힐 최대 횟수
    _uint                       m_iGiftHealCount = { 0 }; //현재 힐 카운트
    _bool                       m_bProjectileFlying = { false }; //투사체 날아가는중인지
    _bool                       m_bDead = { false };

    _float                      m_fTeleportDelay = { 0.f }; //텔레포트 딜레이 시간
    _bool                       m_bPendingTeleport = { false }; //텔레포트 보류중인지

    _float                      m_fMaxSoundDist = { 10.f }; // 플레이어와 사운드 최대 거리

    //플레이어 따라가기
    deque<_vector>              m_dequePlayerPosition;

#pragma region 시네마틱
    enum YAKUMO_CINEMATIC { YC_OLIVER = 1, YC_OLIVER_SPEED, YC_TP_AGAIN, YC_OLIVER_END, YC_END };

    _bool                       m_bCinematic = false;       // 시네마틱 중인지
    _bool                       m_bFirstCinematic = false;  // 첫 만남 했는지
    _uint                       m_iCinematicNum = { YC_END };   // 시네마틱 할 번호
#pragma endregion 시네마틱

private:
    HRESULT						Ready_Components();
    HRESULT                     Bind_ShaderResources();
    HRESULT						Ready_PartObjects();
    HRESULT						Ready_States();
    HRESULT						Ready_Event();
    HRESULT						Ready_PhysXEvent();


public:
#ifdef _DEBUG
    void                        Add_Debug_WeaponCapsule(_float _radius = 1.f, _float3 _color = _float3(1.0f, 0.2f, 0.2f));
#endif // _DEBUG

private:
#pragma region PhysX 이동, 전투
    void						Update_WeaponPosition();
    void						Process_AttackSweep_Capsule();
#pragma endregion PhysX 이동, 전투

public:
    static Yakumo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override final;
};

NS_END