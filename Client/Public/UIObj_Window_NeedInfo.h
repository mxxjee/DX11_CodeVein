#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Text;


class UIObj_Window_NeedInfo :
    public UIObject
{
public:
    struct NeedInfoUIEvent
    {
        ItemInfo* pInfo;

    };
protected:
    explicit UIObj_Window_NeedInfo();
    explicit UIObj_Window_NeedInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_NeedInfo(const UIObj_Window_NeedInfo& original);
    virtual ~UIObj_Window_NeedInfo();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);
public:
    void Free() override;
public:
    virtual void        After_ApplyData();
public:
    static UIObj_Window_NeedInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    UIObj_Text* m_HaveHaze_Text = nullptr;
    UIObj_Text* m_NeedHaze_Text = nullptr;

};
NS_END

