#include "Client_Define.h"
#include "UIObj_PopUp_MapTitle.h"
#include "UIObj_Text.h"
#include "UI_Progress.h"
#include "UISoundUtil.h"

Client::UIObj_PopUp_MapTitle::UIObj_PopUp_MapTitle()
{
}

Client::UIObj_PopUp_MapTitle::UIObj_PopUp_MapTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_PopUp_MapTitle::UIObj_PopUp_MapTitle(const UIObj_PopUp_MapTitle& original)
    :UIObject(original)
{
}

Client::UIObj_PopUp_MapTitle::~UIObj_PopUp_MapTitle()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_PopUp_MapTitle::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_PopUp_MapTitle::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_DeActiveAlarm.m_AlarmFunc = [this]()
        {
            Set_Active(false);
            m_DeActiveAlarm.Elapsed = 0.f;
            m_DeActiveAlarm.Off();
        };
    m_DeActiveAlarm.Off();

    ////이벤트구독
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<MapTitleUIEvent>([this](const MapTitleUIEvent& event)
        {

        
            m_pGameInstance->Play_Sound("MapInfo_UI",UIVolume);

            m_DeActiveAlarm.Limit = event.m_fDeactiveSecond;
            m_DeActiveAlarm.Elapsed = 0.f;
            
            if (m_pText)
                m_pText->Set_Text(event.m_Text);

            m_DeActiveAlarm.On();

            Set_Active(true);
        }));


    m_ProgressAlarm.Limit = 1.f;
    m_ProgressAlarm.m_AlarmFunc = [this]()
        {
            m_fCurrent = 1.f;
            m_ProgressAlarm.Off();
        };

    m_ProgressAlarm.Off();


 
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_PopUp_MapTitle::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_PopUp_MapTitle::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    m_DeActiveAlarm.Update(fTimeDelta);
    m_ProgressAlarm.Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_PopUp_MapTitle::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_PopUp_MapTitle::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void        Client::UIObj_PopUp_MapTitle::After_ApplyData()
{
    //자식캐싱
    __super::After_ApplyData();

    m_pLogo = Get_Child(L"Logo");
    m_pText = dynamic_cast<UIObj_Text*>(Get_Child(L"Text"));
    m_pLine = Get_Child(L"Line");
    m_pGlowTexture = Get_Child(L"GlowTexture");

    if (m_pLine)
    {
        m_LineProgress = dynamic_cast<UI_Progress*>(m_pLine->Get_Component_FromName(Proto_UIProgress));
        CHECK_TRUE(m_LineProgress == nullptr);

        m_LineProgress->Set_Current(&m_fCurrent);
        m_LineProgress->Set_Max(&m_fMax);

    }

}

UIObj_PopUp_MapTitle* Client::UIObj_PopUp_MapTitle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_PopUp_MapTitle* pInstance = new UIObj_PopUp_MapTitle(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_PopUp_MapTitle 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_PopUp_MapTitle::Clone(void* pArg)
{
    UIObj_PopUp_MapTitle* pInstance = new UIObj_PopUp_MapTitle(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PopUp_MapTitle 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}


void Client::UIObj_PopUp_MapTitle::Set_Active(_bool _isActive)
{

    if (_isActive)
    {

        m_fCurrent = 0.f;
        m_LineProgress->Reset();

        m_ProgressAlarm.On();
        
     
        m_pLogo->Set_Alpha(1.f);
        m_pLine->Set_Alpha(1.f);
        m_pGlowTexture->Set_Alpha(1.f);

        Set_Alpha(1.f);
    }

    __super::Set_Active(_isActive);



}

void Client::UIObj_PopUp_MapTitle::Free()
{
    __super::Free();
}
