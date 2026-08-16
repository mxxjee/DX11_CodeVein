#pragma once
#include "Client_Define.h"
#include "Base.h"


NS_BEGIN(Engine)
class Player_Stat;
class Transform;
class GameInstance;
NS_END

NS_BEGIN(Client)
class Skill_Base abstract : public Base
{
public:
    enum SKILL_TYPE
    {
        INSTANT_SKILL, //단발형
        DURATION_SKILL, //지속형

    };

public:
	typedef struct tagSkillBaseDesc
	{
        _uint       iSkillID = {};
        _int        iAnimIndex = {};
        _bool       bIsLoop = { false };
        DAMAGEPOWER eDamagePower = { DAMAGEPOWER::POWER_END };
        _float      fSkillDuration = {}; //스킬 지속시간
        SKILL_TYPE  eSkillType = {};
        //ItemManager 등록 + 스킬도 같이
        //Create_Skill_Prototype(L"블러드 샷", "KetsugiIcon/T_Ketsugi004", 1, 1, L"피의 힘을 거대한 탄환으로 바꿔 날린다.", L"양", L"액티브", 2, 2, L"관통");
        wstring     wstrSkillName = {};    //스킬이름
        string      strTexKey = {};        // 잘모름
        wstring     wstrDescription = {};  //스킬 설명 
        wstring     wstrLineageType = {};  // 음/양/기술
        wstring     wstrSkillType = {};    // 액티브/패시브
        wstring     wstrAttackType = {};   // 관통/파쇄/절단 등 , 없으면 L"-"
        _float      fMyeonghyeolCost = {}; // 소비 명혈(민지는 Ichor로 사용)
        _float      fCooldown = {};        // 재사용 대기시간
        _int        iItemCount = { 1 };    //아이템 개수
        _int        iMaxItemSize = { 1 };  //아이템 최대 개수
        vector<WEAPON_TYPE> vecWeaponType; //활성 가능한 웨폰타입

	}SKILL_DESC;

protected:
	explicit Skill_Base();
	explicit Skill_Base(const Skill_Base& original) = delete;
	virtual ~Skill_Base() = default;

public:
    virtual HRESULT Initialize(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);

    virtual void Enter_Skill() = 0;
    virtual void Update_Skill(_float fTimeDelta); //이거 어짜피 판단(CanMove,CanCombo와 같은 이벤트 분기는 동일하기 때문에) 필요한 애들만 사용하도록
    virtual void Exit_Skill() = 0;

   
    virtual void On_StartKetsugi() = 0; //플레이어에서 bStratKetsugi 이벤트 발생했을때 발생하는 버프,효과,명혈 감소 등등 (해당 이벤트 불릴때 호출할 함수) 
    virtual void On_SkillProjectileEvent(); //발사체 생성
    virtual void On_DurationEnd(); // 지속시간 끝날때 해제 or 적용할 함수 ex)버프 해제 등

public:
    //여기서 이제 자식들 사용할 기능들 
    _bool           Can_SkillEnter() const; //스킬 사용할수있는지
    void            Start_Cooldown(); //쿨다운 시작
    void            Update_Cooldown(_float fTimeDelta); //쿨다운 업데이트
    _bool           IsCooldownReady() const { return m_fCurrentCooldown <= 0.f; } //쿨다운 끝나서 다시 사용할수있는지
    void            Start_Duration(); //지속시간 시작
    void            Update_Duration(_float fTimeDelta);

    _uint           Get_SkillID() const { return m_tSkillDesc.iSkillID; } //스킬 아이디 아이템슬롯과 같이 4000번부터 사용할예정
    const wstring&  Get_SkillName() const { return m_tSkillDesc.wstrSkillName; } //스킬이름
    DAMAGEPOWER     Get_DamagePower() const { return m_eDamagePower; } //해당 스킬의 데미지 파워(상태에서 세팅해줄 예정)

    void            Set_DebugCoolAndDuration(_float fValue) { m_fCurrentCooldown = fValue, m_fSkillDurtaion = fValue; }


    //*추가 (UI에연결)*//
    _float          Get_SkillDurationRatio();
    _float          Get_CoolTimeRatio() { return m_fCurrentCooldown / m_tSkillDesc.fCooldown; }
    _bool           Can_Active_Skill();
    const vector<WEAPON_TYPE>* Get_WeaponTypes() { return &m_tSkillDesc.vecWeaponType; }

public:
    /*UI에게 보낼 이벤트를 따로정의함*/
    void        Request_SkillUI_Active() ;
    void        Request_SkillUI_StartCooldown();
    void        Request_SkillUI_EndCoolDown();

    void        Cancle_Request();

protected:
    SKILL_DESC              m_tSkillDesc = {};
    DAMAGEPOWER             m_eDamagePower = {DAMAGEPOWER::POWER_END};
    _float                  m_fCurrentCooldown = {};
    _float                  m_fSkillDurtaion = {};
    _bool                   m_bDurationActive = {false}; 

    _bool                   m_bStartCoolDown = false;

protected:
    class Player_Stat*      m_pPlayerStatCom = { nullptr };
    class Transform*        m_pPlayerTransformCom = { nullptr };
    class Player_MasterRig* m_pPlayerMasterRig = { nullptr };
    class Player*           m_pPlayer = { nullptr };
    class GameInstance*     m_pGameInstance = { nullptr };

public:
    virtual void Free();

private:
    bool        m_bDurationEnter = false;

};

NS_END