#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

class UIObj_Selector; 
class UIObj_CustomMenuButton;

class UIObj_CustomMenu :
    public UIObject
{
public:
    /*UPDATE_SELECTOR : 호버한 메뉴가 바꼈을때 selector도 그 위치로 옮겨라*/

    enum class CustomMenuUIEventType{UPDATE_SELECTOR,END};
    struct CustomMenuUIEvent
    {
        CustomMenuUIEventType eType;
        CUSTOMIZING_TYPE     m_eCustomType;

    };
protected:
    explicit UIObj_CustomMenu();
    explicit UIObj_CustomMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_CustomMenu(const UIObj_CustomMenu& original);
    virtual ~UIObj_CustomMenu();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);
public:
    void Free() override;
public:
    virtual void        After_ApplyData();


public:
    static UIObj_CustomMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


private:
    /*커스텀 메뉴 버튼들 캐싱.*/
    vector< UIObj_CustomMenuButton*>        m_Buttons;

    /*selector캐싱*/
    UIObj_Selector* m_pSelector = {};
};
NS_END

