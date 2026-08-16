#pragma once
#include "UIObj_SlotGrid.h"

NS_BEGIN(Client)
class UIObj_ShortCutQuickSlot;

class UIObj_SlotGrid_Shortcut :
    public UIObj_SlotGrid
{
protected:
    explicit UIObj_SlotGrid_Shortcut();
    explicit UIObj_SlotGrid_Shortcut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SlotGrid_Shortcut(const UIObj_SlotGrid_Shortcut& original);
    virtual ~UIObj_SlotGrid_Shortcut();
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
    static UIObj_SlotGrid_Shortcut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    //자식 (다른포인터로캐싱)
    vector< UIObj_ShortCutQuickSlot*>       m_ShortCutSlots;

};
NS_END
