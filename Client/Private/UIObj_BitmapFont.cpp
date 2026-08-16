#include "Client_Define.h"
#include "UIObj_BitmapFont.h"
#include "UI_BitmapText.h"

Client::UIObj_BitmapFont::UIObj_BitmapFont()
{
}

Client::UIObj_BitmapFont::UIObj_BitmapFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_BitmapFont::UIObj_BitmapFont(const UIObj_BitmapFont& original)
    :UIObject(original)
{
}

Client::UIObj_BitmapFont::~UIObj_BitmapFont()
{
}


////////////////////////////
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_BitmapFont::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_BitmapFont::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_BitmapFont::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_BitmapFont::Update(const _float fTimeDelta)
{
    if (m_fValue)
    {
    
        m_pBitmapFont->Set_Text(to_wstring((int)*m_fValue));
    }
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_BitmapFont::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    return 0;
}

HRESULT Client::UIObj_BitmapFont::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}
void Client::UIObj_BitmapFont::Set_Text(wstring str)
{
    if (m_pBitmapFont)
        m_pBitmapFont->Set_Text(str);
}
UIObj_BitmapFont* Client::UIObj_BitmapFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_BitmapFont* pInstance = new UIObj_BitmapFont(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ProgressBar 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_BitmapFont::Clone(void* pArg)
{
    UIObj_BitmapFont* pInstance = new UIObj_BitmapFont(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_BitmapFont 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
void Client::UIObj_BitmapFont::Free()
{
    __super::Free();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_BitmapFont::After_ApplyData()
{
    //컴포넌트 캐싱
    Component* pBitmapFont = Get_Component_FromName(Proto_BitmapText);
    if (pBitmapFont)
    {
        m_pBitmapFont = dynamic_cast<UI_BitmapText*>(pBitmapFont);
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////
