#include "Client_Define.h"
#include "UIObj_ManaInfo.h"
#include "UIObj_BitmapFont.h"

Client::UIObj_ManaInfo::UIObj_ManaInfo()
{
}

Client::UIObj_ManaInfo::UIObj_ManaInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_ManaInfo::UIObj_ManaInfo(const UIObj_ManaInfo& original)
    :UIObject(original)
{
}

Client::UIObj_ManaInfo::~UIObj_ManaInfo()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ManaInfo::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ManaInfo::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<ManaUIEvent>([this](const ManaUIEvent& event)
        {
            if (event.eType == ManaUIEventType::UPDATE_CURRENTMANA)
            {
                if (m_pCurrentManaText)
                    m_pCurrentManaText->Set_Text(to_wstring((int)event.fValue));

                
            }

            else if (event.eType == ManaUIEventType::UPDATE_MAXMANA)
            {
                if (m_pMaxManaText)
                    m_pMaxManaText->Set_Text(to_wstring((int)event.fMaxValue));
            }


            else if (event.eType == ManaUIEventType::INTIALIZE)
            {
                if (m_pCurrentManaText)
                    m_pCurrentManaText->Set_Text(to_wstring((int)event.fValue));

                if (m_pMaxManaText)
                    m_pMaxManaText->Set_Text(to_wstring((int)event.fMaxValue));
            }
        }));

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_ManaInfo::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_ManaInfo::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);



    return 0;
}

_int Client::UIObj_ManaInfo::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_ManaInfo::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void        Client::UIObj_ManaInfo::After_ApplyData()
{
    //자식캐싱
    __super::After_ApplyData();

    m_pCurrentManaText = dynamic_cast<UIObj_BitmapFont*>(Get_Child(L"CurrentMana_Text"));
    m_pMaxManaText= dynamic_cast<UIObj_BitmapFont*>(Get_Child(L"TotalMana_Text"));
  

}

UIObj_ManaInfo* Client::UIObj_ManaInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ManaInfo* pInstance = new UIObj_ManaInfo(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ManaInfo 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_ManaInfo::Clone(void* pArg)
{
    UIObj_ManaInfo* pInstance = new UIObj_ManaInfo(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ManaInfo 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_ManaInfo::Free()
{
    __super::Free();
}
