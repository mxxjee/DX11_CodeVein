#include "Client_Define.h"
#include "UIObj_Window_Palette.h"
#include "UIObj_PaletteArea.h"
#include "UIObj_VerticalSlider.h"
#include "Camera.h"
#include "Mouse.h"



Client::UIObj_Window_Palette::UIObj_Window_Palette()
{
}

Client::UIObj_Window_Palette::UIObj_Window_Palette(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Window_Palette::UIObj_Window_Palette(const UIObj_Window_Palette& original)
    :UIObject(original)
{
}

Client::UIObj_Window_Palette::~UIObj_Window_Palette()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_Palette::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_Window_Palette::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);


	return S_OK;
}

/// //////////////////////////////////////////////////////////////////////////

_int Client::UIObj_Window_Palette::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

    return 0;
}

_int Client::UIObj_Window_Palette::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_Window_Palette::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    if (IsMouseOver(m_pMouse->Get_MousePos()))
        m_pGameInstance->Get_CurrentCamera()->Camera_Lock(true);
  

    else
        m_pGameInstance->Get_CurrentCamera()->Camera_Lock(false);

    return 0;
}

HRESULT Client::UIObj_Window_Palette::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);

    return S_OK;
}void        Client::UIObj_Window_Palette::After_ApplyData()
{

    __super::After_ApplyData();
   
    m_pPaletteArea = dynamic_cast<UIObj_PaletteArea*>(Get_Child(L"Palette_Area"));
    m_pCursor = Get_Child(L"Palette_Cursor");
    if(m_pCursor)
        Init_Color(_float2(m_pCursor->Get_X(), m_pCursor->Get_Y()));
    

    m_pHueSlider = dynamic_cast<UIObj_VerticalSlider*>(Get_Child(L"Slider_Vertical_Bg"));
    if (m_pHueSlider)
    {
        m_pHueSlider->Bind_UpdateFunc([this]()
            {
                if (m_pPaletteArea)
                {
                    m_pPaletteArea->Set_Saturation(m_pHueSlider->Get_Value());
                    m_pPaletteArea->Set_DisplayColor();

                    //manager에게도 갱신..

                }
            });

        m_pHueSlider->Bind_InitFunc([this]()        
            {
                if (m_pPaletteArea)
                {
                    m_pPaletteArea->Set_Saturation(m_pHueSlider->Get_Value());
                    
                    //manager에게도 갱신..

                }
            });

    }
}



///////////////////////////////////////////////////
UIObj_Window_Palette* Client::UIObj_Window_Palette::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_Palette* pInstance = new UIObj_Window_Palette(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_Palette 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Window_Palette::Clone(void* pArg)
{

    UIObj_Window_Palette* pInstance = new UIObj_Window_Palette(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_Palette 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
void Client::UIObj_Window_Palette::Init_Color(_float2 CursorLocalPos)
{

    if (m_pPaletteArea)
    {
        _float2         UV = m_pPaletteArea->Get_CursorUV(CursorLocalPos);
        m_pPaletteArea->Set_CursorUV(UV);   
        m_pPaletteArea->Set_DisplayColor();
    }
}
////////////////////////////////////////////////////
void        Client::UIObj_Window_Palette::Set_Active(_bool _isActive)
{
    /*이 WINDOW가 켜질때 색깔세팅해준다.*/
    if (_isActive)
    {
        //Init_Color();
    }

    __super::Set_Active(_isActive);
}

void Client::UIObj_Window_Palette::Free()
{
    __super::Free();

}


_float2 Client::UIObj_Window_Palette::Get_CursorPosFromRGB(_float4 vRGB)
{
    if (m_pPaletteArea)
        return m_pPaletteArea->Get_CursorPosFromRGB(vRGB);

    return _float2();


}
_float Client::UIObj_Window_Palette::Get_SaturationFromRGB(_float4 vRGB)
{ 
    if (m_pPaletteArea)
        return m_pPaletteArea->Get_SaturationFromRGB(vRGB);

    return _float();
}

_float Client::UIObj_Window_Palette::Get_CursorPosYFromValue(_float fValue)
{
    if (m_pHueSlider)
        m_pHueSlider->Get_CursorPosYFromValue(fValue);

    return _float();

}