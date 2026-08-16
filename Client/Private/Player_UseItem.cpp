#include "Client_Define.h"
#include "Player_UseItem.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "InventoryManager.h"
#include "Player_Injection.h"

Client::Player_UseItem::Player_UseItem()
{
}

HRESULT Client::Player_UseItem::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

void Client::Player_UseItem::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	//원래 게임도 등록되어 있는 아이템이 0개거나 없으면 return 
	ItemInfo* pItemInfo = InventoryManager::GetInstance()->Get_ShorcutFocusItem();
	if (pItemInfo == nullptr || pItemInfo->itemCount <= 0)
	{
		if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
		{
			m_pStateMachine->Change_State(Player::RUN);
			return;
		}
		else
		{
			m_pStateMachine->Change_State(Player::IDLE);
			return;
		}
	}
	else //
	{
		if (pItemInfo->ItemID == 5001) //HP재생물약
		{
			m_pPlayerMasterRig->Set_AnimationUpper(86, true);

			if (XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //움직임이없다면 전신 + 상체를 동시에 재생 
			{
				m_pPlayerMasterRig->Set_Animation(86, false);

			}
			else if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //진입 시점에 움직임이 있다면 
			{
				m_pStateMachine->Change_State(Player::WALK); //하체(전신)은 걷기 모션으로
			}
		}
		else if (pItemInfo->ItemID == 5000 || pItemInfo->ItemID == 5010 || pItemInfo->ItemID == 5011) //명혈 회복약 , 상실의 조각
		{
			m_pPlayerMasterRig->Set_AnimationUpper(85, true);

			if (XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //움직임이없다면 전신 + 상체를 동시에 재생 
			{
				m_pPlayerMasterRig->Set_Animation(85, false);

			}
			else if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //진입 시점에 움직임이 있다면 
			{

				m_pStateMachine->Change_State(Player::WALK); //하체(전신)은 걷기 모션으로
			}
		}
		else if (pItemInfo->ItemID == 5002 || pItemInfo->ItemID == 5009) //재생 유도약 , 학장인자 ? 
		{
			m_pPlayerMasterRig->Set_AnimationUpper(83, true);

			if (XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //움직임이없다면 전신 + 상체를 동시에 재생 
			{
				m_pPlayerMasterRig->Set_Animation(83, false);

			}
			else if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //진입 시점에 움직임이 있다면 
			{
				m_pStateMachine->Change_State(Player::WALK); //하체(전신)은 걷기 모션으로
			}
		}

	}

}

void Client::Player_UseItem::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();


	if (EventDesc.bCanInjectionVisible())
		m_pPlayer->Get_PlayerInjection()->Set_PartActive(true);
	else
		m_pPlayer->Get_PlayerInjection()->Set_PartActive(false);


	if (EventDesc.bCanCombo() && Desc.bShiftPressed && (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB || Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pStateMachine->Change_State(Player::SPECIALATTACK);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.bCDown && !Desc.bZeroStamina)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pStateMachine->Change_State(Player::ATTACKPARRY);
		return;
	}
	else if (EventDesc.bCanCombo() && m_pPlayer->CanUseSkill())
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pStateMachine->Change_State(Player::ATTACKNORMAL);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pStateMachine->Change_State(Player::ATTACKSTRONGSTART);
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pStateMachine->Change_State(Player::BACKSTEP); //백스텝  
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if (EventDesc.bCanMove() && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pStateMachine->Change_State(Player::RUN);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bAltDown)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::GUARDSTART);
		m_pStateMachine->Change_State(Player::GUARDWALK);
		return;
	}
	else if (m_pPlayerMasterRig->Is_UpperAnimFinished() == true) //재생 끝났으면
	{
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.1f); //상체 0.1초 블렌드하면서 사라지게
		m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화


		if (m_pStateMachine->Get_CurStateType() == Player::IDLE || m_pStateMachine->Get_CurStateType() == Player::RUN)
		{
			m_pStateMachine->Get_CurrentState()->Enter_State();
		}
		else
		{
			if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
				m_pStateMachine->Change_State(Player::RUN);
			else if (XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
				m_pStateMachine->Change_State(Player::IDLE);
		}
	}
	else if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //업데이트 시점에 움직임이 있다면 
	{
		m_pStateMachine->Change_State(Player::WALK); //하체(전신)은 걷기 모션으로
		return;
	}



	//후반에는 키입력가능하게
}

void Client::Player_UseItem::Exit_State()
{
	if(m_pPlayer->Get_PlayerInjection()->Get_IsPartActive()==true)
		m_pPlayer->Get_PlayerInjection()->Set_PartActive(false);
}

Player_UseItem* Client::Player_UseItem::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_UseItem* pInstance = new Player_UseItem();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_UseItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_UseItem::Free()
{
	__super::Free();
}
