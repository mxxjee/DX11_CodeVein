#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Text;

class UIObj_Window_ShopSubDesc :
    public UIObject
{
public:
    struct ShopSubDescUIEvent
    {
        ITEM_CATEGORY eCategory;
        ItemInfo* pInfo;
    };
protected:
    explicit UIObj_Window_ShopSubDesc();
    explicit UIObj_Window_ShopSubDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_ShopSubDesc(const UIObj_Window_ShopSubDesc& original);
    virtual ~UIObj_Window_ShopSubDesc();

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

public:
    static UIObj_Window_ShopSubDesc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    void        Update_SubDesc(ItemInfo* pInfo);

private:
    /*자식캐싱*/
    UIObj_Text* m_pMaxCount = nullptr;      //ITEMTYPE만사용
    UIObj_Text* m_pCurrentCount = nullptr;//ITEMTYPE만사용

    UIObj_Text* AttackType = nullptr;
    UIObj_Text* PhysicsType = nullptr;


private:
    ITEM_CATEGORY   m_eCategory;

};
NS_END

