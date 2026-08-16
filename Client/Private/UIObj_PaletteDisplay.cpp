#include "Client_Define.h"
#include "UIObj_PaletteDisplay.h"

Client::UIObj_PaletteDisplay::UIObj_PaletteDisplay()
{
}

Client::UIObj_PaletteDisplay::UIObj_PaletteDisplay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_PaletteDisplay::UIObj_PaletteDisplay(const UIObj_PaletteDisplay& original)
    :UIObject(original)
{
}

Client::UIObj_PaletteDisplay::~UIObj_PaletteDisplay()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_PaletteDisplay::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_PaletteDisplay::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< PaletteDisplayUIEvent>([this](const PaletteDisplayUIEvent& e)
        {
            if (e.eType == EventType::SET_DISPLAYCOLOR)
                Change_DisplayColor(e.vRGBColor);

            else if (e.eType == EventType::UPDATE_DISPLAYCOLOR)
                Change_DisplayColor(e.vRGBColor);
        }));


    return S_OK;
}

/// //////////////////////////////////////////////////////////////////////////

_int Client::UIObj_PaletteDisplay::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

    return 0;
}

_int Client::UIObj_PaletteDisplay::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_PaletteDisplay::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    return 0;
}

HRESULT Client::UIObj_PaletteDisplay::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);

    return S_OK;
}

void Client::UIObj_PaletteDisplay::After_ApplyData()
{
    __super::After_ApplyData();


    m_pDisplayColor = Get_Child(L"Display_Color");
    if (m_pDisplayColor)
        m_pDisplay_ImgComp = dynamic_cast<UI_Image*>(m_pDisplayColor->Get_Component_FromName(Proto_UIImage));


}

void Client::UIObj_PaletteDisplay::Change_DisplayColor(_float4 vColor)
{
    CHECK_JUST_NULL(m_pDisplay_ImgComp);
    m_pDisplay_ImgComp->Set_Color(vColor);

}

///////////////////////////////////////////////////
UIObj_PaletteDisplay* Client::UIObj_PaletteDisplay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_PaletteDisplay* pInstance = new UIObj_PaletteDisplay(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_PaletteDisplay 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_PaletteDisplay::Clone(void* pArg)
{
    UIObj_PaletteDisplay* pInstance = new UIObj_PaletteDisplay(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PaletteDisplay 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
////////////////////////////////////////////////////

void Client::UIObj_PaletteDisplay::Free()
{
    __super::Free();
}
