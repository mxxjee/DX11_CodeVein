#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_SlotGrid;

}

NS_BEGIN(Client)
class CustomizingManager;

class UIObj_ItemGrid :
    public UIObject
{
public:
    enum class ItemGridUIEventType { OPEN_TEXTURESELECTOR, END };
    struct ItemGridUIEventEvent
    {
        //얘한테는 열릴 slogrid의 texture를 지정해줘야한다
        ItemGridUIEventType eType;
        wstring NewTexKey = L"";
        int     iFocusTexIdx = 0;   //슬롯중어느거선택했는지
        GRID_DESC* Desc = nullptr;
        _uint iTotalTex = 9999;

        //누른애에 댛나 정보
        int     iFocusSlotIdx = 0;
        CUSTOMIZING_TYPE CustomizingType = CUSTOMIZING_TYPE::END;
        CUSTOMIZING_VALUE_TYPE CustomizingValueType = CUSTOMIZING_VALUE_TYPE::END;

    };
protected:
    explicit UIObj_ItemGrid();
    explicit UIObj_ItemGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ItemGrid(const UIObj_ItemGrid& original);
    virtual ~UIObj_ItemGrid();

public:
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
    static UIObj_ItemGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    virtual void        After_ApplyData();
    virtual void OnHover();    //
    virtual void OnHoverExit();

    virtual void    OnClick();//클릭해을때 어떤 슬롯중 인덱스반환했느지 체크
    virtual void Set_Active(_bool _isActive);
private:
    UI_SlotGrid* m_pSlotGridComp = nullptr;
    CustomizingManager* m_pCustomizingManager = nullptr;


private:
    CUSTOMIZING_TYPE        m_eFocusType=CUSTOMIZING_TYPE::END;
    CUSTOMIZING_VALUE_TYPE      m_eFousValueType= CUSTOMIZING_VALUE_TYPE::END;
    int                m_iFocusSlotIdx = 0; //이 객체를 열게 한 대상의 idx
    _uint               m_iClothesIdx = 0;
    _uint               m_iLastHoveredIdx = -1;


    UIObject*       m_pTattoUse = nullptr;  //전체 부모
    UIObject* m_pTattoButton = nullptr; //누를수있는범위
    UIObject* m_pCheckImg = nullptr; //누를수있는범위

    bool        m_bUseTatto = false;
};
NS_END

