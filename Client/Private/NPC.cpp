#include "Client_Define.h"
#include "NPC.h"
#include "GameInstance.h"

#include "InventoryManager.h"
#include "ItemManager.h"
#include "InteractionManager.h"

Client::NPC::NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Character(pDevice, pContext)
{
}

Client::NPC::NPC(const NPC& original)
    : Character(original)
{
}

HRESULT Client::NPC::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);
    return S_OK;
}

HRESULT Client::NPC::Initialize(void* _arg)
{
    NPC_DESC* pArgDesc = static_cast<NPC_DESC*>(_arg);

    CHECK_FAILED(GameObject::Initialize(pArgDesc), E_FAIL);

    Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

    static _uint namenum = 0;
    m_wstrName = L"NPC_" + to_wstring(namenum++);

    m_eObjType = OBJTYPE::TYPE_CHARACTER;

    m_fGravity = -9.81f;
    m_bIsGrounded = false;

    m_eInteractionType = INTERACTION_TYPE::NPC;
    InteractionManager::GetInstance()->Register_InteractableObject(this);
    
    m_ActionHashs.reserve(10);

    return S_OK;
}

_int Client::NPC::Update_Priority(_float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    __super::Update_Priority(fTimeDelta);

    return 0;
}

_int Client::NPC::Update_Parallel(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    __super::Update_Parallel(fTimeDelta);

    return 0;
}

_int Client::NPC::Update(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    __super::Update(fTimeDelta);
    Apply_Gravity(fTimeDelta);

    return 0;
}

_int Client::NPC::Update_Late(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    __super::Update_Late(fTimeDelta);
    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    return 0;
}

HRESULT Client::NPC::Render(const _float fTimeDelta)
{
    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

    return S_OK;
}

HRESULT Client::NPC::Ready_Components()
{
    return S_OK;
}

HRESULT Client::NPC::Ready_States()
{
    return S_OK;
}

void Client::NPC::Apply_Gravity(const _float fTimeDelta)
{
    if (m_pController == nullptr)
        return;

    if (!m_bIsGrounded)
    {
        m_fVelocityY += m_fGravity * fTimeDelta;
    }
    else
    {
        m_fVelocityY = -0.1f;
    }

    PxVec3 displacement(0.f, m_fVelocityY * fTimeDelta, 0.f);

    PxControllerCollisionFlags collisionFlags = m_pController->move(
        displacement, 0.01f, fTimeDelta, PxControllerFilters()
    );

    if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        m_bIsGrounded = true;
    }
    else
    {
        m_bIsGrounded = false;
    }

    if (m_pTransformCom)
    {
        physx::PxExtendedVec3 pxFootPos = m_pController->getFootPosition();

        _vector vNewPos = XMVectorSet((_float)pxFootPos.x, (_float)pxFootPos.y, (_float)pxFootPos.z, 1.f);

        m_pTransformCom->Set_State(DIRECTION::POSITION, vNewPos);
    }
}

_float Client::NPC::Calculate_DistanceToPlayer()
{
    GameObject* pPlayer = m_pGameInstance->Get_Player();
    if (pPlayer == nullptr)
        return 99999.f;

    Transform* pPlayerTransform = static_cast<Transform*>(pPlayer->Get_Component_FromName(TEXT("Com_Transform")));
    if (pPlayerTransform == nullptr || m_pTransformCom == nullptr)
        return 99999.f;

    _vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
    _vector vPlayerPos = pPlayerTransform->Get_State(DIRECTION::POSITION);

    _vector vDistance = XMVector3Length(vPlayerPos - vMyPos);

    return XMVectorGetX(vDistance);
}

_bool Client::NPC::Is_PlayerInRange(_float fRange)
{
    _float fDistance = Calculate_DistanceToPlayer();

    if (fDistance <= fRange)
        return true;

    return false;
}

#pragma region IInteractable
bool Client::NPC::IsInteractable()
{
    //거리비교말고 다른 진입조건이 없으므로 true
    return true;
}

void Client::NPC::Enter_InteractionRange(GameObject* pPlayer)
{
    //UI띄우깅
    //WCOUT(L"현재 " << m_wstrName << L" 의 interaction 범위에 들어왔습니다. Enter_InteractionRange ");
    UI_MasterEvent Event;
    Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
    Event.m_ActionName = "ActiveUI";
    Event.m_Text = L"PopUp_NPC";
    Event.m_bPersistent = true;
    Event.m_bFlag = true;
    m_pGameInstance->Publish(Event);

}

void Client::NPC::Stay_InteractionRange(GameObject* pPlayer, const _float& fTimeDelta)
{
    WCOUT(L"현재 " << m_wstrName << L" 의 interaction 범위에 있습니다.Stay_InteractionRange ");

}

void Client::NPC::Exit_InteractionRange(GameObject* pPlayer)
{
    //UI없애깅
    //WCOUT(L"현재 " << m_wstrName << L" 의 interaction 범위에서 나갔습니다.Exit_InteractionRange ");
    UI_MasterEvent Event;
    Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
    Event.m_ActionName = "ActiveUI";
    Event.m_Text = L"PopUp_NPC";
    Event.m_bPersistent = true;
    Event.m_bFlag = false;
    m_pGameInstance->Publish(Event);
}

void Client::NPC::Enter_Interaction(GameObject* pPlayer)
{
   
}

void Client::NPC::Stay_Interaction(GameObject* pPlayer, const _float& fTimeDelta)
{
    //WCOUT(L"현재 " << m_wstrName << L" 의 interaction 중입니다.Stay_Interaction ");


}

void Client::NPC::Exit_Interaction(GameObject* pPlayer)
{
    //머ㅜ......나중에....플레이어상태도려녹ㅎ기..??머..
    //WCOUT(L"현재 " << m_wstrName << L" 의 Exit_Interaction ");
    m_bPreInteraction = false;
    InteractionManager::GetInstance()->Reset_BestPriority();
}


#pragma endregion

void Client::NPC::Free()
{
    if (m_pController)
    {
        m_pGameInstance->PhysX_Remove_Actor(m_pController->getActor());
        m_pGameInstance->PhysX_Remove_Controller(m_pController);
    }
    if (m_bIsClone)
        InteractionManager::GetInstance()->UnRegisterInteractable(this);
    __super::Free();
}