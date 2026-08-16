#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
/*매번 킬때마다 색깔세팅*/
class UIObj_PaletteArea;
class UIObj_VerticalSlider;


class UIObj_Window_Palette :
    public UIObject
{
protected:
    explicit UIObj_Window_Palette();
    explicit UIObj_Window_Palette(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_Palette(const UIObj_Window_Palette& original);
    virtual ~UIObj_Window_Palette();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);
    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

    
public:
    _float2     Get_CursorPosFromRGB(_float4 vRGB);
    _float      Get_SaturationFromRGB(_float4 vRGB);
    _float      Get_CursorPosYFromValue(_float fValue);
public:
    void Free() override;

public:
    static UIObj_Window_Palette* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
private:
    void        Init_Color(_float2 CursorLocalPos);

public:
    virtual void        Set_Active(_bool _isActive);
    virtual void        After_ApplyData();

private:
    UIObj_PaletteArea*      m_pPaletteArea = {};
    UIObject*               m_pCursor = nullptr;
    
    UIObj_VerticalSlider* m_pHueSlider = {};

};
NS_END

