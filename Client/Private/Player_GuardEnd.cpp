#include "Client_Define.h"
#include "Player_GuardEnd.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_GuardEnd::Player_GuardEnd()
{
}

HRESULT Client::Player_GuardEnd::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false); //우선은 false 
	if (m_pPlayerTransformCom == nullptr || m_pUpperStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_GuardEnd::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
	if (ActiveWeapon == nullptr)
		return;
	else
	{
		//처음 무조건 진입은 Start 고 이거 끝나면 Loop인거고 만약에 Alt키를 뗏으면 End인거고
		Player::INPUT_DESC Desc = m_pPlayer->Get_PlayerInputDesc();
		ANIMREQUEST_DESC AnimDesc = {};


		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::GUARD_END;

		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_AnimationUpper(iAnimIndex, true,1.0f,false);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.2f);
		m_pUpperStateMachine->Set_CurStateType(Player::PLAYERUPPERSTATE_END); //이거 초기화 안하면 이전게 계속 남아있어서 체인지가 제대로 안됨.
	}
}

void Client::Player_GuardEnd::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	if (m_pPlayerMasterRig->Is_UpperAnimFinished() == true)
	{
		m_pUpperStateMachine->Set_CurStateType(Player::PLAYERUPPERSTATE_END); //이거 초기화 안하면 이전게 계속 남아있어서 체인지가 제대로 안됨.
	}

}

void Client::Player_GuardEnd::Exit_State()
{

}

Player_GuardEnd* Client::Player_GuardEnd::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_GuardEnd* pInstance = new Player_GuardEnd();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_GuardEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_GuardEnd::Free()
{
	__super::Free();
}
