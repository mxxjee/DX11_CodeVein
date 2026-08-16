#pragma once
#include "UIObj_CustomizeSelector.h"

namespace Engine
{
    class UI_Progress;
}

NS_BEGIN(Client)
class UIObj_Text;

class UIObj_ValueSelector :
    public UIObj_CustomizeSelector
{
public:
    enum SelectorType{VERTICAL,HORIZONTAL};
public:
    struct ValueSelectorEvent
    {
        _uint ownerID;
        bool bValue = false;
    };
protected:
    explicit UIObj_ValueSelector();
    explicit UIObj_ValueSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ValueSelector(const UIObj_ValueSelector& original);
    virtual ~UIObj_ValueSelector();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

public:
    void Free() override;

public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);

    virtual void        OnHoverEnter();
    virtual void        OnHoverExit();

    virtual void        OnClick();
    void        Update_Display(void* pArg);
public:
    void        Set_Enter(bool b) { m_bEnter = b; }
    virtual void        On_Close();

    SelectorType        Get_SelectorType() { return m_eSelectorType; }
private:
    bool        m_bEnter = false;//한번 클릭시 true이고, 이거 true여야지만 값조절가능

public:
    static UIObj_ValueSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    UI_Progress*    m_pProgressComp = nullptr;

    vector<UIObject*> m_Arrows;
    UIObject* m_pFill_Info = nullptr;
    UIObj_Text* m_pRatioText = nullptr;

    _float      m_fRatio = 0.f;


    SelectorType     m_eSelectorType= SelectorType::VERTICAL;

};
NS_END

