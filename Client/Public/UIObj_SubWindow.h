#pragma once
#include "UIObject.h"


/*인벤토리 슬롯을 클릭해서 나오는 서브창들*/
//이창이켜질떄 메인인벤토리느 꺼지도록해야함


NS_BEGIN(Client)
class UIObj_SlotGrid;
class UIObj_Slot;
class UIObj_Text;
class InventoryManager;
class UIObj_SubDesc;

class UIObj_SubWindow :
    public UIObject
{
public:
    struct SubWindowEvent
    {
        string m_ActionName = "";
        size_t ActionHash = 0;
        ItemInfo* m_Info = nullptr;     //보낼 iteminfo구조체
        ITEM_CATEGORY m_eCategory = ITEM_CATEGORY::END;

    };
protected:
    explicit UIObj_SubWindow();
    explicit UIObj_SubWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SubWindow(const UIObj_SubWindow& original);
    virtual ~UIObj_SubWindow();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
public:
    virtual void        Set_Active(_bool _isActive);

public:
    void Free() override;
public:
    virtual void        After_ApplyData();


private:
            //출력할 total창에 맞는 아이콘을 호버
    void        Update_SelectCategoryMark(ITEM_CATEGORY eCategory);
    void        Update_DescriptionInfo(ItemInfo* pInfo);
public:
    static UIObj_SubWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


protected:
    UIObj_SlotGrid* m_SlotGrid = nullptr;
    UIObj_Text* m_pDescTextName = nullptr;
    UIObj_Text* m_pDescInfoText = nullptr;
    UIObject*   m_pDescIcon = nullptr;

private:
    UIObject* m_pCategory_SelectMark = nullptr;
    vector<UIObject*>   m_CategoryItems;


    unordered_map<ITEM_CATEGORY, UIObj_SubDesc*> m_mapSubDescGroups;
private:
    InventoryManager*           m_pInventoryManager = nullptr;

private:
    unordered_map<size_t, function<void(const SubWindowEvent&)>> m_Actions;

};
NS_END
