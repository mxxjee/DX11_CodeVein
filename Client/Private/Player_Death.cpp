#include "Client_Define.h"
#include "Player_Death.h"
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


Client::Player_Death::Player_Death()
{
}

HRESULT Client::Player_Death::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

    CloseUIAlarm.Limit = 2.5f;
    CloseUIAlarm.m_AlarmFunc = [this]()
        {

            UI_MasterEvent Event;
            Event.m_bFlag = false;
            Event.m_ActionName = "ActiveUI";
            Event.m_Text = L"PlayerDeadUI";
            Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;

            m_pGameInstance->Publish(Event);

            CloseUIAlarm.Off();
        };
    return S_OK;
}

void Client::Player_Death::Enter_State()
{
    if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
        return;

    Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

    m_pPlayer->Enter_DeadState();

    auto* ActiveBloodWeapon = m_pPlayer->Get_ActivePlayerBloodWeapon();
    if (ActiveBloodWeapon == nullptr)
        return;
    else
    {
        switch (m_eDeathType)
        {
        case DEATHIDLE:
            m_pPlayerMasterRig->Set_Animation(126, false);
            break;
        case DOWNDEATH_S:
            m_pPlayerMasterRig->Set_Animation(120, false);
            break;
        case DOWNDEATH_P:
            m_pPlayerMasterRig->Set_Animation(121, false);
            break;
        default:
            break;
        }
    }

	m_bFirst = false;

    /*죽었을때 연출ui추가*/
    UI_MasterEvent Event;
    Event.m_bFlag = true;
    Event.m_ActionName = "ActiveUI";
    Event.m_Text = L"PlayerDeadUI";
    Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;

    m_pGameInstance->Publish(Event);
    InteractionManager::GetInstance()->Set_Enable(false);

    m_pPlayer->Enter_DeadState();
}

void Client::Player_Death::Update_State(_float fTimeDelta)
{
    CloseUIAlarm.Update(fTimeDelta);

    Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
    Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

    m_pPlayer->Set_ApplyTranslation(false); //죽을때 움직임 불가.

	if (m_pPlayerMasterRig->Is_AnimFinished() && !m_bFirst)
	{
		m_bFirst = true;

		LEVEL CurrentLevel = (LEVEL)m_pGameInstance->Get_Current_LevelID();
		//여기서 이제 재생 끝나면 체크포인트때처럼 동일하게 페이드 시작 / 끝나면 자동으로 플레이어한테 이벤트 발송은 되니까

		{
			UIObj_FadeScreen::FadeScreenEvent Event;
			Event.eType = UIObj_FadeScreen::FadeScreenEventType::AUTO_FADE;

            Event.m_fSecond = 2.f;
            Event.m_bSavePoint = true;
            Event.m_EndFunc = [CurrentLevel, this]()
            {

                GameObject* pPlayer = m_pGameInstance->Get_Player();
                Character* pCharacter = dynamic_cast<Character*>(pPlayer);
                if (pCharacter)
                {
                    SAVE_POINT_INFO* SavepointInfo = InteractionManager::GetInstance()->Get_LastSavePointInfo();

                    pCharacter->Teleport(SavepointInfo->spawnPosition, SavepointInfo->spawnRotation);
                    m_pGameInstance->Close_All_Menu();

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

        CloseUIAlarm.On();

        return;
    }



}

void Client::Player_Death::Exit_State()
{

    InteractionManager::GetInstance()->Set_Enable(true);
}

Player_Death* Client::Player_Death::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
    Player_Death* pInstance = new Player_Death();

    if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
    {
        MSG_BOX("Failed to Created : Player_Death");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Client::Player_Death::Free()
{
    __super::Free();
}
