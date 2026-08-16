#include "Client_Define.h"
#include "Player_GuardLoop.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_GuardLoop::Player_GuardLoop()
{
}

HRESULT Client::Player_GuardLoop::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

void Client::Player_GuardLoop::Enter_State()
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


		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::GUARD_LOOP;

		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_AnimationUpper(iAnimIndex, true, 1.0f, true);
			
	}
}

void Client::Player_GuardLoop::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	if(Desc.bAltUp) //알트 키를 뗐으면 End로 진입
	{
		//m_pPlayerMasterRig->Request_ClearAnimationUpper(0.1f);
		m_pStateMachine->Change_State(Player::IDLE);
		m_pUpperStateMachine->Change_State(Player::GUARDEND);
		return;
	}


	//루프에서는 이전 키방향과 현재 키 방향을 비교해서 바뀔때만 다시 Enter_State를 들어가도록 수정해줘야할거같고
	//즉 이전 InputDir과 현재 InputDir을 비교해서 들어가도록 

}

void Client::Player_GuardLoop::Exit_State() //애니메이션이 바뀔때 호출되므로 같은 애니메이션에서는 return 해주니까 이때 비활성화
{

}

Player_GuardLoop* Client::Player_GuardLoop::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_GuardLoop* pInstance = new Player_GuardLoop();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_GuardLoop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_GuardLoop::Free()
{
	__super::Free();
}
