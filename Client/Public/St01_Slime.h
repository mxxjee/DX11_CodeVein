#pragma once
#include "Monster.h"

NS_BEGIN(Client)
class Monster_EventShape;

//나중에 공중상태에서 hit해도 바로 떨어져야함 추가

class St01_Slime :
    public Monster
{
    enum AttackType { NORMALATTACK = 9, ATTACKSHOOT, END };
public:
    //IDLE-지상의 IDLE
    //CELING-공중에서 IDLE(아직 플레이어 만나기전, 거꾸로매달려있는상태)
    enum class SLIMESTATE { IDLE = ENEMYSTATE::END, WAIT, IDLE_CEILING, FALL_START, FALL_LOOP, FALL_END };
private:
    explicit St01_Slime(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit St01_Slime(const St01_Slime& original);
    virtual ~St01_Slime() = default;


public:
    virtual HRESULT Initialize_Prototype(LEVEL _level) override;
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update_Priority(const _float fTimeDelta) override;
    virtual _int Update_Parallel(const _float fTimeDelta) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;
    virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;


private:
    virtual HRESULT Ready_Components() override;
    virtual HRESULT Ready_PartObjects();
    HRESULT         Ready_PhysXEvent();
    HRESULT         Ready_Event();
    HRESULT         Ready_Value();
    HRESULT         Ready_Socket();
    HRESULT         Ready_State();


public:
    static St01_Slime* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    virtual GameObject* Clone(void* pArg) override;
    virtual void Free() override;

private:
    _float Calcuate_To_PlayerDistance_XZ();

private:
    void Change_State(_uint iNewState);
    void Register_Alarm();

    void Update_RootMotion_Physics(_float fTimeDelta);

    //컨트롤러(피직스)랑 모델위치 동기화
    void Sync_Transform_With_Controller();      //위에서 구한 발밀림현상의 값을 적용

private:
                //플레이와 거리 비교해서 떨어질지 감지함
    void        Check_To_Change_FallState();    
    void        Snap_To_Ceilng();       //Initialize()시에, 자신의 위치에서 ray를 쏴서 천장에 고정시키는역할
    void        Find_Floor();//떨어질때바닥감지


private:
    void OnDamaged(const DAMAGE_EVENT& _damageEvent);

    // 디버깅 하려고 override뺀것
    void OnSpawn(void* _arg) override;
    void OnDespawn() override;
 
private:

    HRESULT Enter_State();

    //한번 엔터시에 들어감
    virtual HRESULT Set_Animation() override;

    void        LookAtPlayer(bool bUpdateNavDir, const _float fTimeDelta);
    //STate동안 계쏙돌아갈 update(이거는 로직관련,행동바꾸는거 관련)
    virtual void Update_AI(const _float fTimeDelta) override;

    //이거는 stat에 따른 물리관련
    virtual void    Update_AI_Physcis(const _float fTimeDelta);

    void        Set_MonsterEventShape();

    void        Update_Alarms(const _float fTimeDelta);

#ifdef _DEBUG
    void    Debug_State();
#endif // _DEBUG

private:
    //ENEMYSTATE도 쓰고 현재 내 클래스에 정의된 enum도쓸려구
    _uint       m_iState = 1;
    _uint       m_iPreState = 0;

private:
    _float3                 m_vCeilingPos;
    Monster_EventShape* m_pEventShape = nullptr;


private:
    Alarm               m_Alarm_To_IDLE;
    Alarm               m_Alarm_To_CHASE;
    Alarm               m_Alarm_To_DEADSHADER;        //이 알람이 울린이후 사라지는 쉐이더 재생,

    _uint               AttackAnim;
    _float              fAttackDist = 0.f;//어택타입판단ㅇ하기위함
private:
   bool        m_bAlive = true;    //Dead상태일떄 다른상태로진입하지않기위한변수
   bool         m_bFallen = false;

};

NS_END
