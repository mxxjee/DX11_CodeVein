#include "Client_Define.h"
#include "Player_CheckPoint.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "Player_DamageBlow.h"
#include "Player_BloodWeapon.h"
#include "InteractionManager.h"
#include "UIObj_FadeScreen.h"
#include "IInteractable.h"
#include "Player_Stat.h"

Client::Player_CheckPoint::Player_CheckPoint()
{
}

HRESULT Client::Player_CheckPoint::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	m_pPlayerStatCom = m_pPlayer->Get_PlayerStatCom();
	Set_HasAnimEvents(false);
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_CheckPoint::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	auto* ActiveBloodWeapon = m_pPlayer->Get_ActivePlayerBloodWeapon();
	if (ActiveBloodWeapon == nullptr)
		return;
	else
	{
		_uint iAnimIndex = {};

		switch (m_eCheckPointType)
		{
		case Client::Player_CheckPoint::START:
			m_pPlayerMasterRig->Set_Animation(230, false);
			break;
		case Client::Player_CheckPoint::LOOP:
			m_pPlayerMasterRig->Set_Animation(231, true);
			break;
		case Client::Player_CheckPoint::END:
			m_pPlayerMasterRig->Set_Animation(232, false);
			break;
		}



	}

}

void Client::Player_CheckPoint::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	m_pPlayer->Set_ApplyTranslation(false); //체크포인트 움직임 불가.

	if (m_eCheckPointType == CHECKPOINTTYPE::START)
	{
		if (m_pPlayerMasterRig->Is_AnimFinished())
		{
			m_eCheckPointType = CHECKPOINTTYPE::LOOP;

			LEVEL iLevel = m_pPlayer->Get_SavePointLevel();
			_uint iIndex = m_pPlayer->Get_SavePointIndex();

			LEVEL CurrentLevel = LEVEL(m_pGameInstance->Get_Current_LevelID());

			if (CurrentLevel != iLevel)
			{
				UIObj_FadeScreen::FadeScreenEvent Event;
				Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
				Event.m_iNextLevel = iLevel;
				Event.m_bSavePoint = true;
				m_pGameInstance->Publish(Event);
			}
			else
			{
				UIObj_FadeScreen::FadeScreenEvent Event;
				Event.eType = UIObj_FadeScreen::FadeScreenEventType::AUTO_FADE;
				Event.m_iNextLevel = iLevel;
				Event.m_fSecond = 2.f;
				Event.m_bSavePoint = true;
				Event.m_EndFunc = [CurrentLevel, iLevel, iIndex, this]()
					{

						GameObject* pPlayer = m_pGameInstance->Get_Player();
						Character* pCharacter = dynamic_cast<Character*>(pPlayer);
						if (pCharacter)
						{
							SAVE_POINT_INFO* SavepointInfo = InteractionManager::GetInstance()->Get_SavePointInfo(iLevel, iIndex);
							CHECK_JUST_NULL(SavepointInfo);

							pCharacter->Teleport(SavepointInfo->spawnPosition, SavepointInfo->spawnRotation);
							m_pGameInstance->Close_All_Menu();

							YAKUMO_ACTIVE_EVENT YakumoDesc; //야쿠모 활성화 이벤트
							YakumoDesc.bSuccess = true;
							m_pGameInstance->Publish(YakumoDesc);

							INPUT_LOCK_EVENT LockEvent;
							LockEvent.bLock = false;
							m_pGameInstance->Publish(LockEvent);

							IInteractable* pObj = InteractionManager::GetInstance()->Get_Current_Interaction_Target();
							if (pObj)
								pObj->Exit_Interaction(pPlayer);

							//각씬에 맞도록 창 되돌리기
							switch (CurrentLevel)
							{
							case LEVEL::BASE:
								m_pGameInstance->Change_UIMode(UI_MODE::BASE);
								break;

							default:
								m_pGameInstance->Change_UIMode(UI_MODE::DEFAULT);
								break;
							}
						}

					};
				m_pGameInstance->Publish(Event);
			}

			//그냥 겨우살이 interaction 시 바로 lastpoint설정
			//InteractionManager::GetInstance()->Set_LastSavePointInfo(iLevel, iIndex);

			return;
		}

	}
	else if (m_eCheckPointType == CHECKPOINTTYPE::END)
	{
		if (!m_bHealStat)
		{
			m_pPlayerStatCom->Heal_Hp(m_pPlayerStatCom->Get_MaxHp());
			m_pPlayerStatCom->FullRestore_Stamina();
			m_pPlayerStatCom->Apply_Myeonghyeol(m_pPlayerStatCom->Get_MaxMyeonghyeol());
			m_bHealStat = true;




		}


		if (m_pPlayerMasterRig->Is_AnimFinished())
		{
			m_pStateMachine->Change_State(Player::IDLE);
			m_bHealStat = false;
			return;
		}
				
	}

	
}

void Client::Player_CheckPoint::Exit_State()
{
}

Player_CheckPoint* Client::Player_CheckPoint::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_CheckPoint* pInstance = new Player_CheckPoint();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_CheckPoint");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_CheckPoint::Free()
{
	__super::Free();
}
