#include "Client_Define.h"
#include "UIObj_Text.h"


Client::UIObj_Text::UIObj_Text()
{
}

Client::UIObj_Text::UIObj_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_Text::UIObj_Text(const UIObj_Text& original)
    :UIObject(original)
{
}

Client::UIObj_Text::~UIObj_Text()
{
}

////////////////////////////
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Text::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Text::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_Alarm.m_AlarmFunc = [this]()
        {
            Set_Active(false);
            m_Alarm.Off();

        };


    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< SYSTEMTEXTEVENT>([this](const SYSTEMTEXTEVENT& e)
        {
            if (m_eTextType == TEXTTYPE::SYSTEM && e.eType==TEXTTYPE::SYSTEM)
            {
                Set_Text(e.Text);

                Set_Active(true);

                m_Alarm.Limit = e.m_fTime;
                m_Alarm.Elapsed = 0.f;
                m_Alarm.On();
            }
           
            else if (m_eTextType == TEXTTYPE::WARNING && e.eType == TEXTTYPE::WARNING)
            {

                Set_Text(e.Text);
                Set_Active(true);
            }
           
        }));

    m_Alarm.Off();
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_Text::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Text::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (m_eTextType == TEXTTYPE::SYSTEM)
        m_Alarm.Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_Text::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    return 0;
}

HRESULT Client::UIObj_Text::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_Text::Set_Text(wstring str)
{
    if (m_pText)
    {
        m_wstrText = str;
        m_pText->Set_Text(str);

        if (m_pText->Get_IsWorldFont())
            m_pText->Bake_Text(true);

    }
}

void Client::UIObj_Text::Set_Color(_float4 vColor)
{
    if (m_pText)
    {
        m_pText->Set_Color(vColor);
    }
}
void Client::UIObj_Text::Set_Spacing(float f)
{
    if (m_pText)
        m_pText->Set_Spacing(f);
}


////////////////////////////////////////////////////////////////
UIObj_Text* Client::UIObj_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Text* pInstance = new UIObj_Text(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Text 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Text::Clone(void* pArg)
{
    UIObj_Text* pInstance = new UIObj_Text(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Text 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
void Client::UIObj_Text::Free()
{
    __super::Free();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Text::After_ApplyData()
{
    //컴포넌트 캐싱
    Component* pFont = Get_Component_FromName(Proto_UIText);
    if (pFont)
    {
        m_pText = dynamic_cast<UI_Text*>(pFont);
    }


    if (m_UIType == "SYSTEM")
        m_eTextType = TEXTTYPE::SYSTEM;


    else if (m_UIType == "WARNING")
        m_eTextType = TEXTTYPE::WARNING;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////
