#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class CustomizingManager;
/*- UI_Image→미리보기 사진 갱신
- 누르면 열릴 창 연결
- 인덱스
- 타입
- Customize_Value_Type(MESH/TEXTURE/COLOR)*/

class UIObj_CustomizeSelector:
    public UIObject
{
public:
    enum class EventType { SET_DEFAULTCOLOR, END };
    struct CustomizeSelectorUIEvent
    {
        EventType eType;

    };
protected:
    explicit UIObj_CustomizeSelector();
    explicit UIObj_CustomizeSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_CustomizeSelector(const UIObj_CustomizeSelector& original);
    virtual ~UIObj_CustomizeSelector();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

public:
    void Free() override;

public:
    virtual void        Set_Active(_bool _isActive);
    virtual void        After_ApplyData();

    virtual void        OnHoverEnter();
    virtual void        OnHoverExit();
    virtual void        OnClick();

    void        OnHoverEnter_Force();

    int*     Get_Idx() { return &m_iIdx; }
    CUSTOMIZING_VALUE_TYPE      Get_CustomValueType() { return m_eCustomValueType; }
public:
    virtual void        On_Close() {};//창나갈떄 걍 호출할거정의
public:
    /*얘를 통해 열은 창의 정보를 넘겨줄떄 부르는함수(보통 미리보기설정)*/
    virtual void        Update_Display(void* pArg);

public:
    CUSTOMIZING_VALUE_TYPE      Get_Custom_ValueType() { return m_eCustomValueType; }
protected:
    ////////////////캐싱할 자식///////////////
    UIObject* m_pHover_Light = nullptr;
    UIObject* m_pHover_Line = nullptr;
    UI_Image* m_pDisplay_ImgComp = nullptr;


    ///////////////////////데이터들///////////////////////////////////
    CUSTOMIZING_TYPE m_eCustomizeType;      //어떤메뉴에 속하는지
    CUSTOMIZING_VALUE_TYPE  m_eCustomValueType; //어떤것을 편집하는애인지(텍스처/메쉬/색상)
    int             m_iIdx = 0;//이 메뉴 중 몇번쨰 colorselector인지
    wstring         m_HoverText = L""; //이 selector hover시 표시할 속성이름

    wstring         m_WindowName = L""; //클릭시 열 창 이름 

    CustomizingManager* m_pCustomizingManager = nullptr;



};
NS_END

