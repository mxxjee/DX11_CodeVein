#include "Client_Define.h"
#include "UIObj_Minimap.h"
#include "MinimapManager.h"

Client::UIObj_Minimap::UIObj_Minimap()
{
}

Client::UIObj_Minimap::UIObj_Minimap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Minimap::UIObj_Minimap(const UIObj_Minimap& original)
    :UIObject(original), m_pMinimapManager(original.m_pMinimapManager)
{
}

Client::UIObj_Minimap::~UIObj_Minimap()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Minimap::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    m_pMinimapManager = MinimapManager::GetInstance();

    return S_OK;
}

HRESULT Client::UIObj_Minimap::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< MiniampUIEvent>([this](const MiniampUIEvent& e)
        {

			Set_Visible(!e.bHide);
       
        }));


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_Minimap::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Minimap::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Minimap::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    return 0;
}

HRESULT Client::UIObj_Minimap::Render(const _float fTimeDelta)
{
    if (m_MinimapType == MINIMAP_TYPE::CENTER)
    {
        m_pMinimapManager->Render_Centered_Minimap(2.f,m_pShaderCom);
        __super::Render(fTimeDelta);
        m_pMinimapManager->Render_Icons_On_UI(2.f, true, fTimeDelta);
    }


    else
    {
        m_pMinimapManager->Render_Full_Minimap(m_pShaderCom);
        __super::Render(fTimeDelta);
        

    }



    return S_OK;
}

UIObj_Minimap* Client::UIObj_Minimap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Minimap* pInstance = new UIObj_Minimap(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ProgressBar 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Minimap::Clone(void* pArg)
{
    UIObj_Minimap* pInstance = new UIObj_Minimap(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Minimap 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Minimap::Free()
{
    __super::Free();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Minimap::After_ApplyData()
{
    __super::After_ApplyData();

    if (m_UIType == "CENTER")
        m_MinimapType = MINIMAP_TYPE::CENTER;

    if (m_UIType == "FULL")
        m_MinimapType = MINIMAP_TYPE::FULL;


}
///////////////////////////////////////////////////////////////////////////////////////////////////////
