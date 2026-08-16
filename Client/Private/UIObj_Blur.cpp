#include "Client_Define.h"
#include "UIObj_Blur.h"

Client::UIObj_Blur::UIObj_Blur()
{
}

Client::UIObj_Blur::UIObj_Blur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Blur::UIObj_Blur(const UIObj_Blur& original)
    :UIObject(original)
{
}

Client::UIObj_Blur::~UIObj_Blur()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Blur::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Blur::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_Blur::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Blur::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_Blur::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Blur::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

UIObj_Blur* Client::UIObj_Blur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Blur* pInstance = new UIObj_Blur(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Blur 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_Blur::Clone(void* pArg)
{
    UIObj_Blur* pInstance = new UIObj_Blur(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Blur 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Blur::Free()
{
    __super::Free();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Blur::After_ApplyData()
{
    //자식캐싱
    __super::After_ApplyData();

 
}

void Client::UIObj_Blur::Set_Active(_bool _isActive)
{


    __super::Set_Active(_isActive);



}