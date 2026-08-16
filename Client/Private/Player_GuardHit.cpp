#include "Client_Define.h"
#include "Player_GuardHit.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_GuardHit::Player_GuardHit()
{
}

HRESULT Client::Player_GuardHit::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(true); 
	if (m_pPlayerTransformCom == nullptr || m_pUpperStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_GuardHit::Enter_State()
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


		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::GUARD_HIT;

		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_Animation(iAnimIndex, false, 0.2f);
	}

}

void Client::Player_GuardHit::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	//가드 히트상태는 회피기만 가능 , 이동은 막아주고 (루트모션)
	m_pPlayer->Set_ApplyTranslation(false);

	if (m_pGameInstance->KeyUp(DIK_LALT)) //이 Hit 상태일때 뗐었으면 true로 저장
	{
		m_bUpAlt = true;
	}


	//if (Desc.bZeroStamina) //히트당하는 상태에서 스태미나가 0이라면 Guard_Break 진입 -> Player가 체크하도록 바꿈
	//{
	//	m_pPlayer->Set_ApplyTranslation(true);
	//	m_pStateMachine->Change_State(Player::GUARD_BREAK);
	//	return;
	//}
	if (EventDesc.bCanEscape() && m_pGameInstance->KeyDown(DIK_SPACE) && XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::BACKSTEP); //백스텝  
		m_bUpAlt = false;
		return;
	}
	else if (EventDesc.bCanEscape() && m_pGameInstance->KeyDown(DIK_SPACE) && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ROLL);
		m_bUpAlt = false;
		return;
	}
	else if (m_bUpAlt && m_pPlayerMasterRig->Is_AnimFinished()) //알트 키를 뗐으면 End로 진입
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pUpperStateMachine->Change_State(Player::GUARDEND);
		m_pStateMachine->Change_State(Player::IDLE);
		m_bUpAlt = false;
		return;
	}
	else if (m_pGameInstance->KeyPress(DIK_LALT) && m_pPlayerMasterRig->Is_AnimFinished())
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::GUARDWALK);
		m_bUpAlt = false;
		return;
	}

}

void Client::Player_GuardHit::Exit_State()
{

}

Player_GuardHit* Client::Player_GuardHit::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_GuardHit* pInstance = new Player_GuardHit();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_GuardHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_GuardHit::Free()
{
	__super::Free();
}
