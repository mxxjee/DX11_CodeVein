#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CustomizingManager;
class UIObj_Text;


class UIObj_CustomMenuButton :
    public UIObject
{
private:
    struct CutomMenuButtonDesc
    {
        wstring OpenMenuName = L"";     //호버 시 열릴창/클릭시 enter할 창
        
    };

public:
    enum class CustomMenuButtonUIType {ENTER_MENU,EXIT_MENU,END};
    struct CustomMenuButtonUIEvent
    {
        CustomMenuButtonUIType eType;
        CUSTOMIZING_TYPE m_eCustomType;


    };

protected:
    explicit UIObj_CustomMenuButton();
    explicit UIObj_CustomMenuButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_CustomMenuButton(const UIObj_CustomMenuButton& original);
    virtual ~UIObj_CustomMenuButton();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

public:
    void Free() override;
public:
    virtual void        After_ApplyData();
    virtual void        OnHoverEnter();
    virtual void        OnClick();

public:
    static UIObj_CustomMenuButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


private:
    UIObj_Text*             m_pText = nullptr;

private:
    CUSTOMIZING_TYPE m_eCustomType= CUSTOMIZING_TYPE::END;
    CutomMenuButtonDesc         m_eButtonDesc = {};


private:
    CustomizingManager*         m_pCustomizingManager = nullptr;

};
NS_END

