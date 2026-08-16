#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_SlotGrid;


class UIObj_Window_Inventory :
    public UIObject
{
 
protected:
    explicit UIObj_Window_Inventory();
    explicit UIObj_Window_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_Inventory(const UIObj_Window_Inventory& original);
    virtual ~UIObj_Window_Inventory();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    virtual void        After_ApplyData();


public:
    void Free() override;
public:
    static UIObj_Window_Inventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


private:
   unordered_map<SLOT_TYPE,UIObj_SlotGrid*>     m_GridsMap;
};
NS_END

