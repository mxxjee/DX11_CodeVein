#include "Client_Define.h"
#include "UIObj_PopUp_SavePoint.h"
#include "UIObj_Text.h"

Client::UIObj_PopUp_SavePoint::UIObj_PopUp_SavePoint()
{
}

Client::UIObj_PopUp_SavePoint::UIObj_PopUp_SavePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_PopUp_SavePoint::UIObj_PopUp_SavePoint(const UIObj_PopUp_SavePoint& original)
    :UIObject(original)
{
}

Client::UIObj_PopUp_SavePoint::~UIObj_PopUp_SavePoint()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////

HRESULT Client::UIObj_PopUp_SavePoint::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_PopUp_SavePoint::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    return S_OK;
}

_int Client::UIObj_PopUp_SavePoint::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_PopUp_SavePoint::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_pGameInstance->KeyDown(DIK_E))
    {
        //셀렉트 판넬 키기
        if (m_pPanel_Select)
        {
            m_pPanel_Select->Set_Visible(true);
            m_pPanel_Select->Set_Active(true);

        }


        //행동처리하기.
    }

    else if (m_pGameInstance->KeyUp(DIK_E))
    {
        if (m_pPanel_Select)
        {

            m_pPanel_Select->Set_Active(false);
            m_pPanel_Select->Set_Visible(false);

            Set_Active(false);
        }


    }
    return 0;
}

_int Client::UIObj_PopUp_SavePoint::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_PopUp_SavePoint::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

UIObj_PopUp_SavePoint* Client::UIObj_PopUp_SavePoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_PopUp_SavePoint* pInstance = new UIObj_PopUp_SavePoint(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_PopUp_SavePoint 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_PopUp_SavePoint::Clone(void* pArg)
{
    UIObj_PopUp_SavePoint* pInstance = new UIObj_PopUp_SavePoint(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PopUp_SavePoint 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_PopUp_SavePoint::Clone(void* pArg)
{
    UIObj_PopUp_SavePoint* pInstance = new UIObj_PopUp_SavePoint(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PopUp_SavePoint 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_PopUp_SavePoint::Free()
{
    __super::Free();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_PopUp_SavePoint::After_ApplyData()
{
    __super::After_ApplyData();
    m_Progress = dynamic_cast<UI_Progress*>(Get_Component_FromName(Proto_UIProgress));

    m_pLeft = Get_Child(L"Left");
    m_pRight = Get_Child(L"Right");

    m_pInteraction_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"Interaction_Text"));

    m_pKey = Get_Child(L"Interaction_Key");

    m_pPanel = Get_Child(L"Panel");

    m_pPanel_Select = Get_Child(L"Panel_Select");

}

void Client::UIObj_PopUp_SavePoint::Set_Active(_bool _isActive)
{


    //활성화되었을때 할일..
    if (_isActive)
    {
        m_Progress->Reset();
        m_Progress->Set_TargetRatio(1.f);

        if (m_pPanel)
            m_pPanel->Play_Animation(ShowAnimKey);
    }

    __super::Set_Active(_isActive);



}