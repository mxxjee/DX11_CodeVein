#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_Text;
}
NS_BEGIN(Client)
class UIObj_GlowButton :
    public UIObject
{
protected:
    explicit UIObj_GlowButton();
    explicit UIObj_GlowButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_GlowButton(const UIObj_GlowButton& original);
    virtual ~UIObj_GlowButton();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
            //이 버튼의 텍스트를 바구는함수
    void        Set_Text(wstring str);
public:
    void Free() override;

public:
    static UIObj_GlowButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


    /*이 클래스는 호버 시 이벤트를전송한다.*/
public:
    virtual void OnHoverEnter();
    virtual void OnClick();

    void        OnHoverEnter_With_Keyboard();
    virtual void        After_ApplyData();
public:
    virtual void        Set_Idx(_uint i) { m_iIdx = i; }
public:
    //Event함수
    virtual void        Execute_By_Event(const string& strActionName, void* pArg);
    bool                m_bGlowLine = true;

    _uint             m_iIdx = 0;
    UI_Text*            m_pTextComp = nullptr;      //이 버튼이 표시하는 텍스트

};
NS_END

