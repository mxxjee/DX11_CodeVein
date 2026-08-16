#include "Client_Define.h"
#include "UIObj_SkillDesc.h"

Client::UIObj_SkillDesc::UIObj_SkillDesc()
{
}

Client::UIObj_SkillDesc::UIObj_SkillDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_SubDesc(pDevice,pContext)
{
}

Client::UIObj_SkillDesc::UIObj_SkillDesc(const UIObj_SubDesc& original)
    :UIObj_SubDesc(original)
{
}

Client::UIObj_SkillDesc::~UIObj_SkillDesc()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SkillDesc::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_SkillDesc::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

   // m_Texts.reserve(Values::)
    return S_OK;
}
//////////////////////////////////////////////////////////////////////


_int Client::UIObj_SkillDesc::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_SkillDesc::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_SkillDesc::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    return 0;
}

HRESULT Client::UIObj_SkillDesc::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////
UIObj_SkillDesc* Client::UIObj_SkillDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_SkillDesc* pInstance = new UIObj_SkillDesc(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SkillDesc 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_SkillDesc::Clone(void* pArg)
{

    UIObj_SkillDesc* pInstance = new UIObj_SkillDesc(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SkillDesc 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_SkillDesc::Update_Group(ItemInfo* pInfo)
{
    __super::Update_Group(pInfo);
}


///////////////////////////////////////////////////////////////////////////////////////

void Client::UIObj_SkillDesc::Free()
{
    __super::Free();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SkillDesc::After_ApplyData()
{
    __super::After_ApplyData();


}
///////////////////////////////////////////////////////////////////////////////////////////////////////
