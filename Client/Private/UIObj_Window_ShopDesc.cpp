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

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< ShopDescUIEvent>([this](const ShopDescUIEvent& e)
        {
            switch (e.eType)
            {
            case ShopDescUIEventType::SET_ICONTEXTURE:
                Change_Texture(e.pInfo);
                break;

            case ShopDescUIEventType::SET_ITEMDESC:
                Change_ItemDesc(e.pInfo);
                break;
            default:
                break;
            }
       



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
    UIObject* pSelectIcon_Slot = Get_Child(L"SelectSlot_Icon");
    if (pSelectIcon_Slot)
    {
        UIObject* pSelectIcon = pSelectIcon_Slot->Get_Child(L"Slot_Icon");
        if(pSelectIcon)
            m_pSelectIconImg = dynamic_cast<UI_Image*>(pSelectIcon->Get_Component_FromName(Proto_UIImage));

    }
   
    //자식캐싱
    m_pDesc_Text_Name = dynamic_cast<UIObj_Text*>(Get_Child(L"Desc_Text_Name"));
    m_pDesc_Text_Info = dynamic_cast<UIObj_Text*>(Get_Child(L"Desc_Text_Info"));





}

//////////////////////////////////////Event함수///////////////////////////////////
void Client::UIObj_Window_ShopDesc::Change_Texture(ItemInfo* pInfo)
{
    CHECK_JUST_NULL(pInfo);
    CHECK_JUST_NULL(m_pSelectIconImg);


    m_pSelectIconImg->Change_Texture(pInfo->ItemTexKey);


}
void Client::UIObj_Window_ShopDesc::Change_ItemDesc(ItemInfo* pInfo)
{
    CHECK_JUST_NULL(m_pDesc_Text_Name);
    CHECK_JUST_NULL(m_pDesc_Text_Info);

    m_pDesc_Text_Name->Set_Text(pInfo->ItemName);
    m_pDesc_Text_Info->Set_Text(pInfo->ItemDescription);

    if(!m_pDesc_Text_Name->Is_Active())
        m_pDesc_Text_Name->Set_Active(true);

    if (!m_pDesc_Text_Info->Is_Active())
        m_pDesc_Text_Info->Set_Active(true);


}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_Window_ShopDesc::Free()
{
    __super::Free();

}
