#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_Image;

}
/*팔레트에서 선택한 현재 색상이뭔지전달*/
/*혹은 이미 기록된 색상이있다면 이를받아서 표시한다*/

NS_BEGIN(Client)
class UIObj_PaletteDisplay :
    public UIObject
{

public:
            //UPDATE_DISPLAYCOLOR-드래그할때마다 호출
            //SET_DISPLAYCOLOR-이미 이전에 지정된 색깔로 설정,혹은 강제로 세팅(초기화개념)
            //둘은 같이 세팅하는함수인데 어떨때 호출되는지 구분학이ㅟ해서 enum을 나눔

    enum class EventType{UPDATE_DISPLAYCOLOR, SET_DISPLAYCOLOR,END};
    struct PaletteDisplayUIEvent
    {
        EventType eType;
        _float4  vRGBColor;
    };
protected:
    explicit UIObj_PaletteDisplay();
    explicit UIObj_PaletteDisplay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_PaletteDisplay(const UIObj_PaletteDisplay& original);
    virtual ~UIObj_PaletteDisplay();
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
    static UIObj_PaletteDisplay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    virtual void        After_ApplyData();

private:
    void        Change_DisplayColor(_float4 vColor);


private:
    UIObject*           m_pDisplayColor = nullptr;
    UI_Image*        m_pDisplay_ImgComp = nullptr;



};
NS_END

