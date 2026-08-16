#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class ShopManager;

class UIObj_ShopCategory :
    public UIObject
{
protected:
    explicit UIObj_ShopCategory();
    explicit UIObj_ShopCategory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ShopCategory(const UIObj_ShopCategory& original);
    virtual ~UIObj_ShopCategory();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
public:
    void Free() override;
public:
    virtual void        After_ApplyData();
    virtual void        Execute_By_Event(const string& strActionName, void* pArg);

public:
    static UIObj_ShopCategory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
private:
    ITEM_CATEGORY       m_eItemCategory = ITEM_CATEGORY::END;//이 클래스의 아이템카테고리

private:
    ShopManager* m_pShoptManager = nullptr;

};
NS_END

