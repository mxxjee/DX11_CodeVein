#include "Client_Define.h"
#include "Player_AttackStrongStart.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "PoolingManager.h"
#include "Bayonet_Bullet.h"

Client::Player_AttackStrongStart::Player_AttackStrongStart()
{
}

HRESULT Client::Player_AttackStrongStart::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false);

	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_AttackStrongStart::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	m_fHoldTime = 0.f; //시간 초기화 해주고 

}

void Client::Player_AttackStrongStart::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	m_fHoldTime += fTimeDelta;

	if (m_pGameInstance->MouseUp(MOUSEKEYSTATE::RB)) //우클릭을해서 현재 상태들어왔을때 우클릭을 뗐더라면 
	{
		m_pStateMachine->Change_State(Player::ATTACKSTRONG);
		return;
	}
	else if (m_fHoldTime >= m_fHoldTriggerTime) //누르고 있는 시간이 0.2초보다 크다면 차징이라 판단 
	{
		m_pStateMachine->Change_State(Player::ATTACKSTRONGCHARGE);
		return;
	}


}

void Client::Player_AttackStrongStart::Exit_State()
{
}

Player_AttackStrongStart* Client::Player_AttackStrongStart::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_AttackStrongStart* pInstance = new Player_AttackStrongStart();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_AttackStrongStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_AttackStrongStart::Free()
{
	__super::Free();
}
