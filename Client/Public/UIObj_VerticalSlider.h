#pragma once
#include "UIObject.h"
/*Hue와 연결*/

/*최대,최소값을 지정*/
NS_BEGIN(Client)
class UIObj_VerticalSlider :
    public UIObject
{
public:
    enum class VerticalSliderUIEventType { OPEN_PALETTE, END };
    struct VerticalSliderUIEvent
    {
        VerticalSliderUIEventType eType;
        CustomColorInfo* ColorInfo = nullptr;


    };
protected:
    explicit UIObj_VerticalSlider();
    explicit UIObj_VerticalSlider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_VerticalSlider(const UIObj_VerticalSlider& original);
    virtual ~UIObj_VerticalSlider();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

public:
    void Set_MaxValue(_float fValue) { m_fMaxValue = fValue; }
    void Set_MinValue(_float fValue) { m_fMinValue = fValue; }
    void Free() override;

public:
    virtual void        After_ApplyData();


    virtual void    OnDragging();
    _float         Get_Value() { return m_fValue; }

public:
    static UIObj_VerticalSlider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

private:
    virtual void        Set_Active(_bool _isActive);
    _float          Get_MouseRatio();
    void            Set_SliderByLocalPosY(_float fLocalPosY);

                //값이 갱신되었으면 알릴곳.
    void            Update_CursorValue();
    void            Init_CursorValue();
 
public:
    void         Bind_UpdateFunc(function<void()> Func) { m_UpdateFunc = Func; }
    void         Bind_InitFunc(function<void()> Func) { m_InitFunc = Func; }


public:
                //메뉴바꼇을떄 customize manager를 통해서 세팅하기위해 부르는함수
                //팔레트랑 슬라이더를 돌려쓰므로.. 이렇게 세팅해줘야함.
    void        Reset_Cursor_And_Value(_float CursorPosY);

        //커서위치를 통해서 value값을 세팅 (메뉴바꼈을때 세팅하기위해서 필요)
    void        Set_Value(_float CursorPos);

    //value값을알때, cursor위치를 반환
    _float      Get_CursorPosYFromValue(_float fValue);
private:
    _float      m_fValue = 0.f;     //현재 슬라이더에서의 값
    _float      m_fPreValue = -1.f;

    _float      m_fMinValue=0.f;        //슬라이더 최대
    _float      m_fMaxValue=1.f;        //슬라이더 내부

private:
    function<void()> m_UpdateFunc = nullptr;      //이 값을 업데이트할 곳
    function<void()>    m_InitFunc = nullptr;
    
    UIObject*   m_pCursor = nullptr;
};
NS_END

