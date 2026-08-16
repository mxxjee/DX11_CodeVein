#pragma once
#include "UIObj_Slot.h"
#include "Skill_Base.h"

namespace Engine
{
    class UI_Progress;
}
NS_BEGIN(Client)

#define ACTIVE_SLOT_COLOR    _float4(0.f,0.f,0.f,0.5f)
#define ACTIVE_ICON_COLOR  _float4(1.f,1.f,1.f,1.f)

                                    //108
#define DEACTIVE_SLOT_COLOR    _float4(0.42f,0.42f,0.42f,0.5f)
                            //130
#define DEACTIVE_ICON_COLOR  _float4(0.509f,0.509f,0.509f,1.f)




class UIObj_QuickSlot :
    public UIObj_Slot
{

public:
    //READY->플레이어가 이펙트사용이벤트->ACTIVE->(플레이어 이펙트사용끝남)->COOLDOWN
    enum class ESlotState
    {
        READY,//사용가능한 상태
        ACTIVE,//현재 사용중(빨간색깜빢깜빡)
        COOLDOWN,//(쿨다운 이펙트나오는중)
        COOLDOWN_END,
        CANCLE_ACTIVE,
        END
    };

    enum class QuickSlotEventType{EQUIP,WEAPON_CHANGE,SET_PROGRESS,START_ACTIVE,START_COOLDOWN,END_COOLTIME, CANCEL_ACTIVE
    };




public:
    //InventoryManager를 통해 쿨타임전달받기
    struct QuickSlotEvent
    {
        QuickSlotEventType  m_eEventType;
        _uint TargetSlotGrid = 0;       //LEFT 퀵슬롯인지 RIGHt퀵스롯인지
        ItemInfo* m_pItemInfo = nullptr;    //참고할 iteminfo
        _uint iIdx = 0;//바꿀 슬롯의인덱스
        float       fValue = 0.f;
        
        Skill_Base* m_pSkillBase = nullptr;
        _float      fSkillDuration = 0.f;
        
        WEAPON_TYPE eWeaponType;
    };

    
protected:
    explicit UIObj_QuickSlot();
    explicit UIObj_QuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_QuickSlot(const UIObj_QuickSlot& original);
    virtual ~UIObj_QuickSlot();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    void Free() override;


public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);


private:
    void Apply_State_UI(ESlotState eState);
    void        Change_Color_By_WeaponType();

public:
    virtual     void    Ready_Slot();
    virtual HRESULT        Equip(ItemInfo* pInfo) override;

public:
    static UIObj_QuickSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    //상태 바꿀때 호출하는함수(상태진입시 필요한 행동을 정의)
    void        Change_State(ESlotState eNewState);
    void        Exit_State(ESlotState ePrevState);

    //특정상태일대 매프레임 수행하는 행동
    void        Update_State(const _float fTimeDelta);

    //슬롯 배경색이미지바꾸기
    void        Set_BackImgColor(_float4 vColor);

    //아이콘 색바꾸기
    void        Set_IconImgColor(_float4 vColor);


private:
    MAINUISLOT           m_eQuickSlotType;


private:
    UIObject* m_pBackImg = nullptr;                  ///equip시 검은투명처리하기위해,빨간색이미지
    UI_Progress* m_pBack_Progress = nullptr;        //지속시간에 따라 깎임
    /*부모에 정의되어있는 m_pIcon*/


    UIObject*   m_pSlot_Fill = nullptr;         //쿨타임 fillprogress
    UI_Progress* m_pSlotFill_Progress = nullptr;


    UIObject* m_pEffect1 = nullptr;             //쿨타임완료시 번쩍빛나는효과


    UIObject* m_pEffect_Blur= nullptr; 

private:
    /*맵핑 키 설정*/
    _ubyte      m_bActionKey;
    ESlotState      m_eSlotState=ESlotState::READY;
    ESlotState      m_ePreSlotState = ESlotState::END;


private:
        string      SkillUseAnimKey = "OnSkillUse";
        string      SkillReactiveAnimKey = "OnSkillReactive";
        _float      m_fEndTime = 0.f;


        //스킬정보
        Skill_Base* m_pSkill = nullptr;

        _float4 vTargetColor_Slot;
        _float4 vTargetColor_Icon;
     
private:
    unordered_map<QuickSlotEventType, function<void()>> m_Actions;



};
NS_END

