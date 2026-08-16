#include "Client_Define.h"
#include "Player_BackStab.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "Player_DamageBlow.h"
#include "Player_BloodWeapon.h"

Client::Player_BackStab::Player_BackStab()
{
}

HRESULT Client::Player_BackStab::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false);
	Set_DamagePower(DAMAGEPOWER::STUNPOWER);
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_BackStab::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	auto* ActiveBloodWeapon = m_pPlayer->Get_ActivePlayerBloodWeapon();
	if (ActiveBloodWeapon == nullptr)
		return;
	else
	{


		_int iAnimIndex = 135;

		m_pPlayer->Get_ActivePlayerWeapon()->Set_PartActive(false);
		m_pPlayer->Get_ActivePlayerBloodWeapon()->Set_PartActive(false);
		
		m_pPlayerMasterRig->Set_Animation(iAnimIndex, false);

		//이시점에 뒤잡 성공한 몬스터의 OnDamaged를 직접호출한다던지 
	}

}

void Client::Player_BackStab::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	m_pPlayer->Set_ApplyTranslation(false);

	//뒤잡은 무적상태
	if (EventDesc.bCanCombo()) //뒤잡일때 공격가능한 상태에서 바로 시네마틱으로 체인지 / 애니메이션재생끝나고 체인지를 공격 가능 상태일때로 바꿈
	{
		m_pPlayer->Get_ActivePlayerWeapon()->Set_PartActive(false);
		m_pPlayer->Get_ActivePlayerBloodWeapon()->Set_PartActive(true);
		//m_pPlayer->Set_SuccessBackStab(false);

		m_pPlayer->Set_PlayCinematic(true);
		m_pStateMachine->Change_State(Player::SPECIALSUCK);
	}

}

void Client::Player_BackStab::Exit_State()
{
}

Player_BackStab* Client::Player_BackStab::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_BackStab* pInstance = new Player_BackStab();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_BackStab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_BackStab::Free()
{
	__super::Free();
}
