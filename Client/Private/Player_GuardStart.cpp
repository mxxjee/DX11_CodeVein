#include "Client_Define.h"
#include "Player_GuardStart.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_GuardStart::Player_GuardStart()
{
}

HRESULT Client::Player_GuardStart::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false);
	if (m_pPlayerTransformCom == nullptr || m_pUpperStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_GuardStart::Enter_State()
{
	//해당 Start 이벤트에는 가드 판정 on 이 심어져있따. 몇초에 가드 판정 On이 시작 되는가. _bool bGuardDefense = true 와 같이 설정해주면 될듯

	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
	if (ActiveWeapon == nullptr)
		return;
	else
	{
		Player::INPUT_DESC Desc = m_pPlayer->Get_PlayerInputDesc();
		ANIMREQUEST_DESC AnimDesc = {};


		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::GUARD_START;

		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_AnimationUpper(iAnimIndex, true,1.0f,false,0.1f);
	}
}

void Client::Player_GuardStart::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	if (m_pPlayerMasterRig->Is_UpperAnimFinished() == true)
	{
		//m_pPlayerMasterRig->Request_ClearAnimationUpper(0.2f);
		m_pUpperStateMachine->Change_State(Player::GUARDLOOP);
		return;
	}
	//else if(Desc.bAltPressed && !XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //끝나지 않았어도 Press이 true고 이동중이었을때
	//{
	//	m_pUpperStateMachine->Change_State(Player::GUARDLOOP);
	//	return;
	//}
}

void Client::Player_GuardStart::Exit_State()
{
	//m_pUpperStateMachine->Set_CurStateType(0); //이거 초기화 안하면 이전게 계속 남아있어서 체인지가 제대로 안됨.
}

Player_GuardStart* Client::Player_GuardStart::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_GuardStart* pInstance = new Player_GuardStart();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_GuardStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_GuardStart::Free()
{
	__super::Free();
}
