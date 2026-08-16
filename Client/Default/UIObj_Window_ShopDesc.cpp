#include "Client_Define.h"
#include "UIObj_Window_ShopDesc.h"
#include "UI_Image.h"
#include "UIObj_Text.h"



Client::UIObj_Window_ShopDesc::UIObj_Window_ShopDesc()
{
}

Client::UIObj_Window_ShopDesc::UIObj_Window_ShopDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Window_ShopDesc::UIObj_Window_ShopDesc(const UIObj_Window_ShopDesc& original)
    :UIObject(original)
{
}

Client::UIObj_Window_ShopDesc::~UIObj_Window_ShopDesc()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_ShopDesc::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Window_ShopDesc::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

   m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< WINDOWSHOPEVENT>([this](const WINDOWSHOPEVENT& e)
        {
            if (e.eType == WINDOWSHOPEVNETTPYE::INITIALIZE)
                Change_Target(e.m_pTarget, e.TargetOffSet);

            else if (e.eType == WINDOWSHOPEVNETTPYE::END_INTERACTION)
                Clear_Target();

            else if (e.eType == WINDOWSHOPEVNETTPYE::UPDATE_HIGHLIGHT)
                Update_HighlightPos(e.m_eCategory);
        }));

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/

_int Client::UIObj_Window_ShopDesc::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_ShopDesc::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_ShopDesc::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Window_ShopDesc::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_Window_ShopDesc* Client::UIObj_Window_ShopDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_ShopDesc* pInstance = new UIObj_Window_ShopDesc(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_ShopDesc 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Window_ShopDesc::Clone(void* pArg)
{
    UIObj_Window_ShopDesc* pInstance = new UIObj_Window_ShopDesc(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_ShopDesc 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_ShopDesc::After_ApplyData()
{
    UIObject* pSelectIcon = Get_Child(L"SelectSlot_Icon");
    if (pSelectIcon)
    {
        m_pSelectIconImg = dynamic_cast<UI_Image*>(pSelectIcon->Get_Component_FromName(Proto_UIImage));

    }
   
    //자식캐싱
    m_pDesc_Text_Name = dynamic_cast<UIObj_Text*>(Get_Child(L"Desc_Text_Name"));
    m_pDesc_Text_Info = dynamic_cast<UIObj_Text*>(Get_Child(L"Desc_Text_Info"));





}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_Window_ShopDesc::Free()
{
}
