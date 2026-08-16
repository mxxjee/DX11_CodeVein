#pragma once
#include "UIObject.h"

/*창열떄-push window 로 열기*/


NS_BEGIN(Client)
class ShopManager;

class UIObj_Window_Shop :
    public UIObject
{
public:
    enum class  WINDOWSHOPEVNETTPYE { INITIALIZE, UPDATE_HIGHLIGHT,END_INTERACTION };

    struct WINDOWSHOPEVENT
    {
        WINDOWSHOPEVNETTPYE eType;

        GameObject* m_pTarget = nullptr;
        _float3 TargetOffSet = _float3(-0.3f, 1.5f, 0.f);

        ITEM_CATEGORY m_eCategory;


    };
protected:
    explicit UIObj_Window_Shop();
    explicit UIObj_Window_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_Shop(const UIObj_Window_Shop& original);
    virtual ~UIObj_Window_Shop();

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
    static UIObj_Window_Shop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    void        Change_Target(GameObject* pTarget, _float3 Offset);
    void        Clear_Target();
    void        Update_HighlightPos(ITEM_CATEGORY eCategory);


private:
    /*캐싱할 자식들..*/
    UIObject*     m_CategoryHighlight=nullptr;        //카테고리 선택시 움직여야할 하이라이트
    // 
    UIObject*     m_Category_Icon_Weapon=nullptr;     //버튼입력을 받는 카테고리아이콘들,누르면 각 타입에맞는 slotgird 띄우기
    UIObject*     m_Category_Icon_Armor=nullptr;
    UIObject*     m_Category_Icon_Items=nullptr;


private:
    UI_WorldComponent* m_pWorldUIComp = nullptr;
    ShopManager*        m_pShopManager = nullptr;

     

};
NS_END

