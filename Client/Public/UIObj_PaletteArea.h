#pragma once
#include "UIObject.h"

/*palettecursor가 움직일때마다 display에 색상을보낸다.*/
//palettecursor는 여기 영역안에서만 움직이도록설정한다.
//palettecursor의 스크린좌표를 가져와 색상으로 변환하여 저장하게한다.

NS_BEGIN(Client)
class UIObj_PaletteCursor;

class UIObj_PaletteArea :
    public UIObject
{
public:
    enum class PaletteAreaUIEventType{OPEN_PALETTE,END};
    struct PaletteAreaUIEvent
    {
        PaletteAreaUIEventType eType;
        CustomColorInfo* ColorInfo = nullptr;


    };
protected:
    explicit UIObj_PaletteArea();
    explicit UIObj_PaletteArea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_PaletteArea(const UIObj_PaletteArea& original);
    virtual ~UIObj_PaletteArea();

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
    static UIObj_PaletteArea* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    virtual void        After_ApplyData();

    virtual void    OnDragging();           //드래그할떄마다 위치체크하기? 혹은 업데이트?
                                            //드래그할때마다 이벤트보내기(색상 )

                //palette cursor ui 위치로 사각형내의  uv를 가져옴.
    _float2         Get_CursorUV(_float2 CursorPos);
                                            //Cursor가 드래그입력을 안받고얘가 보내는이유-> cursor는 충돌구역이너무작아서
  
    /*이벤트함수,초기화시에도 호출*/

    void        Set_DisplayColor(); //(업데이트용)Display에게 이벤트보내서 색상세팅.
 
    void        Set_Saturation(_float fValue) { m_fSaturation = fValue; }
    void        Set_CursorUV(_float2 fValue) { m_fUV = fValue; }
    _float2     Get_CursorPosFromRGB(_float4 vRGB);
    _float      Get_SaturationFromRGB(_float4 vRGB);


private:
    _float2        Get_MouseRatio();




private:
    UIObject* m_pCursor = nullptr;

private:
    _float2      m_fUV = _float2(0.f, 0.f);

    Export_ColorData        Export_Data;        //보낼떄 묶어서보내기위해
    _float      m_fSaturation = 1.f;       //부모(window_palette)가 세팅해줌

};
NS_END

