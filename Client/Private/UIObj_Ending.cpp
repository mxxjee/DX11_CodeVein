#include "Client_Define.h"
#include "UIObj_Ending.h"
#include "UI_Animation.h"
#include "Player.h"



Client::UIObj_Ending::UIObj_Ending()
{
}

Client::UIObj_Ending::UIObj_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice, pContext)
{
}

Client::UIObj_Ending::UIObj_Ending(const UIObj_Ending& original)
	:UIObject(original)
{
}

Client::UIObj_Ending::~UIObj_Ending()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Ending::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Ending::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_DelayAlarm.m_AlarmFunc = [this]()
        {
            m_bMove = true;
            m_DelayAlarm.Off();
        };
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_Ending::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Ending::Update(const _float fTimeDelta)
{
   
    __super::Update(fTimeDelta);
    
    if (m_bMove)
    {
        m_fTargetPos.y += m_fSpeed * fTimeDelta;


        if (m_pWorldComponent)
            m_pWorldComponent->Set_WorldPos(m_fTargetPos);
    }



    return 0;
}

_int Client::UIObj_Ending::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    m_DelayAlarm.Update(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Ending::Render(const _float fTimeDelta)
{

    CHECK_FALSE_RESULT(m_bMove, S_OK);
    __super::Render(fTimeDelta);

   
    return S_OK;
}


////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Ending::After_ApplyData()
{
	m_pWorldComponent = dynamic_cast<UI_WorldComponent*>(Get_Component_FromName(Proto_UIWorld));
}


void        Client::UIObj_Ending::Set_Active(_bool _isActive)
{

    if (_isActive)
    {
        if (m_pWorldComponent)
        {
            
       
           
            m_fTargetPos = m_fInitWorldPos + m_fSpawnOffSet;
            

            m_pWorldComponent->Set_WorldPos(m_fTargetPos);

            m_DelayAlarm.Off();
            m_DelayAlarm.Limit = m_fDelay;
            m_bMove = false;
            m_DelayAlarm.On();
        }
    }

	__super::Set_Active(_isActive);
}

////////////////////////////////////////////////////////////////////////
UIObj_Ending* Client::UIObj_Ending::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Ending* pInstance = new UIObj_Ending(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Ending 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Ending::Clone(void* pArg)
{
    UIObj_Ending* pInstance = new UIObj_Ending(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Ending 복사 실패", L"경고!!!", nullptr);

        return pInstance;
}

void Client::UIObj_Ending::Free()
{
    __super::Free();

}
