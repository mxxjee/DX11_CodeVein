#pragma once
#include "UIObject.h"


NS_BEGIN(Client)
class UIObj_Text;
class UIObj_CustomizeSelector;

class UIObj_CustomizeGrid :
    public UIObject
{
public:
    //UPDATE_FOCUSCATEGORY - 현재 포커싱된 카테고리를 바군다(타입에 따라서 자기 스스로 VISIBLE판단)
    //UPDATE_HOVERTEXT - 호버 시 표시하기위한 텍스트를 변경한다(이는 COLORSELECTOR 혹은 TEXTUREBUTTON이 호출)
    //OPEN_MENU-(피부)라는 메뉴버튼으 클릭했을때 메뉴에 진입하고, 메뉴 진입 시 첫번째 슬롯이 자동으로 호버된 상태로만들기위함
    
    //OPEN_SELECTOR- 편집기(팔레트,그리드)를 열었을때 메뉴들 색깔 바꾸게하려고
    enum class CustomizeGridEventType { UPDATE_FOCUSCATEGORY,UPDATE_HOVERTEXT,EXIT_MENU,OPEN_MENU,OPEN_SELECTOR,EXIT_SELECTOR, END };
    struct CustomizeGridUIEvent
    {
        CustomizeGridEventType       eventType;
        CUSTOMIZING_TYPE            eTargetCategory;//현재 포커싱된 타겟카테고리
        UIObj_CustomizeSelector* m_pCaller = nullptr;
        
        void*       pSelectType = nullptr;
        void*       pArg = nullptr;

        int         idx = 0;

    };

protected:
    explicit UIObj_CustomizeGrid();
    explicit UIObj_CustomizeGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_CustomizeGrid(const UIObj_CustomizeGrid& original);
    virtual ~UIObj_CustomizeGrid();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

public:
    void Free() override;

public:
    static UIObj_CustomizeGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;



public:
    virtual void        Set_Active(_bool _isActive);
    virtual void        After_ApplyData();

private:
    CUSTOMIZING_TYPE m_eCustomType;  //이 클래스의 커스터마이징메뉴

private:
    UIObj_Text*         m_pHoverText = nullptr;


    vector<UIObject*>       m_Clickable;    //클릭가능한 요소들 모아둠. 메뉴열렸을때 투명하게처리 등 묶어서쓸데가이씀





};
NS_END

