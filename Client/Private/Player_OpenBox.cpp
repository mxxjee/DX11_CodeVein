#include "Client_Define.h"
#include "Player_OpenBox.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "Player_DamageBlow.h"
#include "Player_BloodWeapon.h"

Client::Player_OpenBox::Player_OpenBox()
{
}

HRESULT Client::Player_OpenBox::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_OpenBox::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	auto* ActiveBloodWeapon = m_pPlayer->Get_ActivePlayerBloodWeapon();
	if (ActiveBloodWeapon == nullptr)
		return;
	else
	{
		m_pPlayerMasterRig->Set_Animation(229, false);
	}

}

void Client::Player_OpenBox::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	if (!EventDesc.bCanMove())
	{
		m_pPlayer->Set_ApplyTranslation(false);
	}

	if (EventDesc.bCanMove() && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::RUN);
		return;
	}


	if (m_pPlayerMasterRig->Is_AnimFinished() == true)
	{
		m_pStateMachine->Change_State(Player::IDLE);
	}
}

void Client::Player_OpenBox::Exit_State()
{
}

Player_OpenBox* Client::Player_OpenBox::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_OpenBox* pInstance = new Player_OpenBox();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_OpenBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_OpenBox::Free()
{
	__super::Free();
}
