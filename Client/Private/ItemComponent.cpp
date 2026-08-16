#include "Client_Define.h"
#include "ItemComponent.h"



Client::ItemComponent::ItemComponent()
{
}

Client::ItemComponent::ItemComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :Component(pDevice,pContext)
{
}

Client::ItemComponent::ItemComponent(const ItemComponent& original)
    :Component(original)
{
}

Client::ItemComponent::~ItemComponent()
{
}




//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::ItemComponent::Initialize_Prototype(LIGHT_DESC& Desc)
{
   
    return S_OK;
}


HRESULT Client::ItemComponent::Initialize(void* _arg)
{
    
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::ItemComponent::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::ItemComponent::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::ItemComponent::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/


HRESULT Client::ItemComponent::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

bool Client::ItemComponent::Use_Item(void* pArg)
{
    if (m_ItemInfo.itemCount > 1)
        return true;

    return false;
}

ItemInfo& Client::ItemComponent::Get_Item()
{
    // TODO: 여기에 return 문을 삽입합니다.
    return m_ItemInfo;
}

void Client::ItemComponent::Free()
{
    __super::Free();
}
