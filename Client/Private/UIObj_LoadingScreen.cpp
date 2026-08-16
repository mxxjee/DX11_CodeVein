#include "Client_Define.h"
#include "UIObj_LoadingScreen.h"
#include "UIObj_Text.h"

Client::UIObj_LoadingScreen::UIObj_LoadingScreen()
{
}

Client::UIObj_LoadingScreen::UIObj_LoadingScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_LoadingScreen::UIObj_LoadingScreen(const UIObj_LoadingScreen& original)
    :UIObject(original)
{
}

Client::UIObj_LoadingScreen::~UIObj_LoadingScreen()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_LoadingScreen::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_LoadingScreen::Initialize(void* arg)
{

    m_pGameInstance->Subscribe< LoadingScreenEvent>([this](const LoadingScreenEvent& Event)
        {
            m_Alarm_To_ToggleActive.Off();
            m_Alarm_To_ToggleActive.Elapsed = 0.f;

            

            switch (Event.eType)
            {
            case LoadingScreenEventType::DEACTIVE:
            {
                if (m_bIsActive)
                {
                    if (Event.m_fSecond > 0.f)
                    {
                        m_Alarm_To_ToggleActive.Limit = Event.m_fSecond;
                        m_Alarm_To_ToggleActive.m_AlarmFunc = [this]()
                            {
                                Set_Active(false);
                                m_Alarm_To_ToggleActive.Off();


                            };

                        m_Alarm_To_ToggleActive.On();
                    }

                    else
                        Set_Active(false);
                }
              
                
            }
                break;

            case LoadingScreenEventType::ACTIVE:
            {

                Set_Alpha(0.f);

                if (!m_bIsActive)
                {
                    if (Event.m_fSecond > 0.f)
                    {
                        m_Alarm_To_ToggleActive.Limit = Event.m_fSecond;
                        m_Alarm_To_ToggleActive.m_AlarmFunc = [this]()
                            {
                                Set_Active(true);
                                Set_Alpha(0.f);
                                m_Alarm_To_ToggleActive.Off();
                            };

                        m_Alarm_To_ToggleActive.On();
                    }

                    else
                    {
                        Set_Alpha(0.f);
                        Set_Active(true);
                    }
                }
               
            }
              

                break;

            case LoadingScreenEventType::UPDATE_TEXTS:
            {
                Update_Texts();
            }


            break;

            default:
                break;
            }
        });
    
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    Parsing_LoadingDatas();
 


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_LoadingScreen::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_LoadingScreen::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_Alarm_To_ToggleActive.Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_LoadingScreen::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_LoadingScreen::Render(const _float fTimeDelta)
{
    if (m_Local.m_fAlpha <= 0.001f)
        return S_OK;

    __super::Render(fTimeDelta);
    return 0;
}


UIObj_LoadingScreen* Client::UIObj_LoadingScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_LoadingScreen* pInstance = new UIObj_LoadingScreen(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_LoadingScreen 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_LoadingScreen::Clone(void* pArg)
{
    UIObj_LoadingScreen* pInstance = new UIObj_LoadingScreen(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_LoadingScreen 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void        Client::UIObj_LoadingScreen::After_ApplyData()
{
    //자식캐싱
    __super::After_ApplyData();

    m_pTitleText = dynamic_cast<UIObj_Text*>(Get_Child(L"LoadingText_Title"));
    m_pDescText = dynamic_cast<UIObj_Text*>(Get_Child(L"LoadingText_Desc"));



    Set_Alpha(0.f);

}

void Client::UIObj_LoadingScreen::Parsing_LoadingDatas()
{
    ifstream file("../../DataFiles/LoadingData/LoadingText.json");
    if (!file.is_open())
        return;

    ordered_json root = ordered_json::parse(file);

    for (auto& pData : root)
    {
        string Title = pData["title"];
        string Content= pData["content"];


        m_wstrTitleTexts.push_back(stringToWstring(Title));
        m_wstrLocalString.push_back(stringToWstring(Content));
    }
}

void Client::UIObj_LoadingScreen::Update_Texts()
{
    CHECK_JUST_NULL(m_pTitleText);
    CHECK_JUST_NULL(m_pDescText);


    //표시할 랜덤 인덱스구하기
    _uint iRandom = rand() % m_wstrTitleTexts.size();

    m_pTitleText->Set_Text(m_wstrTitleTexts[iRandom]);
    m_pDescText->Set_Text(m_wstrLocalString[iRandom]);
}


void Client::UIObj_LoadingScreen::Set_Active(_bool _isActive)
{


    if (_isActive)
        m_Local.m_fAlpha = 0.f;

    __super::Set_Active(_isActive);



}
void Client::UIObj_LoadingScreen::Free()
{
    __super::Free();
}
