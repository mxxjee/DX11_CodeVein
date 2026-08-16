#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_Image;
}
NS_BEGIN(Client)
class ShopManager;

class UIObj_ShopSlot :
    public UIObject
{

protected:
    explicit UIObj_ShopSlot();
    explicit UIObj_ShopSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ShopSlot(const UIObj_ShopSlot& original);
    virtual ~UIObj_ShopSlot();
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
    static UIObj_ShopSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    virtual void        After_ApplyData();
    virtual void        Execute_By_Event(const string& strActionName, void* pArg);
    virtual void        OnClick();
    
public:
    void        Set_ItemInfo(ItemInfo* pInfo);
    inline ITEM_CATEGORY   Get_ItemCategory()
    {
        if (m_pItemInfo)
            return m_pItemInfo->m_eCategory;

        else
            return ITEM_CATEGORY::END;
    }

private:
    ITEM_CATEGORY m_eItemCategory;

private:
    ItemInfo* m_pItemInfo=nullptr;      //이 슬롯이 점유하고있는 아이템정보.

    UI_Image* m_pIcon_Image = nullptr;        //표시할 아이콘오브젝트의 image 컴포넌트
    ShopManager* m_pShopManager = nullptr;


private:
    unordered_map<size_t, function<void()>> m_Actions;


};
NS_END

