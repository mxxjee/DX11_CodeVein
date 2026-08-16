#pragma once
#include "UIObj_Slot.h"

NS_BEGIN(Client)
class UIObj_BitmapFont;

class UIObj_ShortCutQuickSlot :
    public UIObj_Slot
{
public:
  
    enum EventType{UPDATE_MENUMODE, UPDATE_EQUIPDATA};
    enum class ShortcutQuickSlotState
    {
        FOCUS,//내가 포커스 인덱스일때.
        SIDE,//포커스보다 +1이거나 -1일떄,
        HIDDEN,//그외일떄는 숨김
        END
    };

    struct tagShortCutQuickSlotEvent
    {
        EventType eType;
        _uint ShortCutMenuMode = 0; //EXTRA인지 DEFAULT인지

    };
protected:
    explicit UIObj_ShortCutQuickSlot();
    explicit UIObj_ShortCutQuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ShortCutQuickSlot(const UIObj_ShortCutQuickSlot& original);
    virtual ~UIObj_ShortCutQuickSlot();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

private:
    void    Move_By_FocusIdx(int iDiff,const _float fTimeDelta,bool bLerp);
    void    Size_By_FocusIdx(int iDiff);
public:
    void Free() override;
    virtual void        Set_Active(_bool _isActive);
    virtual void        Set_Visible(_bool _isVisible);
public:
    virtual void        After_ApplyData();

public:
    static UIObj_ShortCutQuickSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

    virtual HRESULT        Equip(ItemInfo* pInfo);

public:
    ShortcutQuickSlotState      Get_SlotState(int iDiff);


    //거리를 통해서 현재 상태설정(내부적으로 change_state를 부른다)
    void        Check_State(int iDiff, bool bLerp = true);
    void        Change_State(ShortcutQuickSlotState SlotState, bool bLerp);


    void        Update_State(const _float fTimeDelta);
    void        Set_Color(bool bDark);

private:
    ShortcutQuickSlotState      m_eSlotState = ShortcutQuickSlotState::END;
    _float2             m_vInitSize;
    _float2             m_vSideSize;
    string              MoveAnimKey = "OnMove";
    string              ShowAnimKey = "OnShow";


    bool            m_bShortCutDefault = true;

};
NS_END

