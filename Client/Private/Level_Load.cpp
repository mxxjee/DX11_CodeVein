#include "Client_Define.h"
#include "Level_Load.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Main.h"
#include "Level_Sample.h"
#include "Level_UIDev.h"
#include "Level_Customize.h"
#include "Level_Base.h"
#include "Level_Church.h"
#include "Level_Player.h"

#include "UIObj_FadeScreen.h"
#include "UIObj_LoadingScreen.h"
#include "UIObj_Cursor.h"

#include "UIObj_HpBar.h"
#include "UIObj_Window_LevelUp.h"

#include "IInteractable.h"
#include "InteractionManager.h"

#include "UISoundUtil.h"



//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Load::Level_Load()
{
}

Client::Level_Load::Level_Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

Client::Level_Load::~Level_Load()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Load::Initialize(LEVEL _level)
{
	m_eCreateLevel = _level;
	m_iCreateLevel = _uint(_level);
	m_pLoader = Loader::Create(m_pDevice, m_pContext, _level);
	CHECK_NULLPTR(m_pLoader);


	UIObject* pObj = m_pGameInstance->Find_PersistentUI_ByName(L"Loading_Cube");
	if (pObj)
		pObj->Set_Active(true);

	//다음씬이 로고씬이 아닐떄만 laodingscreen 처리
	if (_level != LEVEL::LOGO)
	{
		m_fTimer = 0.f;
		m_Alarm_To_Active_LoadingScreen.Elapsed = 0.f;

		m_Alarm_To_Active_LoadingScreen.Limit = 3.f;
		m_Alarm_To_Active_LoadingScreen.m_AlarmFunc = [this]()
			{
				UIObject* pObj = m_pGameInstance->Find_PersistentUI_ByName(L"Loading_Back");
				if (pObj)
				{

					pObj->Set_Active(true);
					pObj->Set_Alpha(0.f);
				}

				m_Alarm_To_Active_LoadingScreen.Off();
			};


		m_Alarm_To_Active_LoadingScreen.On();



		//텍스트변경
		UIObj_LoadingScreen::LoadingScreenEvent LoadingScreenEvent;
		LoadingScreenEvent.eType = UIObj_LoadingScreen::LoadingScreenEventType::UPDATE_TEXTS;
		m_pGameInstance->Publish(LoadingScreenEvent);


	}
	



	//커서끄기
	UIObj_Cursor::CursorEvent cursorEvent;
	cursorEvent.bEnable = false;
	m_pGameInstance->Publish(cursorEvent);



	//소리모두끄기
	m_pGameInstance->StopAllSound();

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Load::Update_Priority(const _float fTimeDelta)
{

	m_pGameInstance->Update_Priority(fTimeDelta);

	

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Load::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update(fTimeDelta);
	m_Alarm_To_Active_LoadingScreen.Update(fTimeDelta);
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Load::Update_Late(const _float fTimeDelta)
{
	m_fTimer += fTimeDelta;
	m_pGameInstance->Update_Late(fTimeDelta);

	switch (m_eCreateLevel)
	{
	case LEVEL::LOGO:
		if (m_pLoader->Is_LoadComplete())
		{
			m_pGameInstance->Add_Level(_UINT(LEVEL::LOGO), Level_Logo::Create(m_pDevice, m_pContext, m_eCreateLevel));
			m_pGameInstance->Add_PlayerLevel(Level_Player::Create(m_pDevice, m_pContext, LEVEL::PLAYER));
			//Load_Persistent_GameUI();

			//씬바뀜이벤트~
			Publish_ExitEvent(true,false);

			m_pGameInstance->Change_Level(_UINT(LEVEL::LOGO));



		}
		break;

	case LEVEL::CUSTOMIZE:
		if (m_pLoader->Is_LoadComplete())
		{
			//Load_Persistent_GameUI();
			COUT("로딩 시간 : " << m_fTimer);
			m_pGameInstance->Add_Level(_UINT(LEVEL::CUSTOMIZE), Level_Customize::Create(m_pDevice, m_pContext, m_eCreateLevel));

			Publish_ExitEvent();

			m_pGameInstance->Change_Level(_UINT(LEVEL::CUSTOMIZE));
		}
		break;

	case LEVEL::MAIN:
		if (m_pLoader->Is_LoadComplete())
		{
			COUT("로딩 시간 : " << m_fTimer);
			Load_Persistent_GameUI();
			m_pGameInstance->Add_Level(_UINT(LEVEL::MAIN), Level_Main::Create(m_pDevice, m_pContext, m_eCreateLevel));
	
			//씬바뀜이벤트~
			Publish_ExitEvent();

			m_pGameInstance->Change_Level(_UINT(LEVEL::MAIN));

		}	
		break;

	case LEVEL::SAMPLE:
		if (m_pLoader->Is_LoadComplete())
		{
			COUT("로딩 시간 : " << m_fTimer);
			Load_Persistent_GameUI();
			m_pGameInstance->Add_Level(_UINT(LEVEL::SAMPLE), Level_Sample::Create(m_pDevice, m_pContext, m_eCreateLevel));

			//씬바뀜이벤트~
			Publish_ExitEvent();

			m_pGameInstance->Change_Level(_UINT(LEVEL::SAMPLE));
		}
		break;

	case LEVEL::BASE:
		if (m_pLoader->Is_LoadComplete())
		{
			Load_Persistent_GameUI();
			COUT("로딩 시간 : " << m_fTimer);
			m_pGameInstance->Add_Level(_UINT(LEVEL::BASE), Level_Base::Create(m_pDevice, m_pContext, m_eCreateLevel));

			//씬바뀜이벤트~
			Publish_ExitEvent();

			m_pGameInstance->Change_Level(_UINT(LEVEL::BASE));
		}
		break;

	case LEVEL::CHURCH:
		if (m_pLoader->Is_LoadComplete())
		{
			Load_Persistent_GameUI();
			COUT("로딩 시간 : " << m_fTimer);
			m_pGameInstance->Add_Level(_UINT(LEVEL::CHURCH), Level_Church::Create(m_pDevice, m_pContext, m_eCreateLevel));

			//씬바뀜이벤트~
			Publish_ExitEvent();

			m_pGameInstance->Change_Level(_UINT(LEVEL::CHURCH));
		}
		break;

	case LEVEL::UIDEV:
		if (m_pLoader->Is_LoadComplete())
		{
			//Load_Persistent_GameUI();
			COUT("로딩 시간 : " << m_fTimer);
			m_pGameInstance->Add_Level(_UINT(LEVEL::UIDEV), Level_UIDev::Create(m_pDevice, m_pContext, m_eCreateLevel));
			m_pGameInstance->Change_Level(_UINT(LEVEL::UIDEV));
		}
		break;
	}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Load::Render(const _float fTimeDelta)
{



	return S_OK;
}
void Client::Level_Load::Load_Persistent_GameUI()
{
	CHECK_TRUE(m_pGameInstance->Get_AllLoadPersistents());
	UIObject* Obj = nullptr;
	
	//game 관련,player hud 풀링객체들
	{
		//PlayerHUD관련 UI세트
		LOADEVENT event;
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/UIData.json";
		event.m_bPersistent = true;
		m_pGameInstance->Publish(event);


		//헤이즈
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Haze.json";
		event.m_bPersistent = true;
		m_pGameInstance->Publish(event);


		//아웃라인 호버 로드
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Hover_Outline.json";
		m_pGameInstance->Publish(event);


		//인벤토리 슬롯눌렀을때 윈도우들 로드
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Window_Storage.json";
		m_pGameInstance->Publish(event);


		//ShortcutMenu로드
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/ShortCutMenu.json";
		m_pGameInstance->Publish(event);


		//인벤로드
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Window_PlayerMenu.json";
		m_pGameInstance->Publish(event);


		//팝업ui
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/PopUp.json";
		m_pGameInstance->Publish(event);


		//Interaction UI
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Popup_Interaction.json";
		m_pGameInstance->Publish(event);

		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/PopUp_SavePoint.json";
		m_pGameInstance->Publish(event);

		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Popup_NPC.json";
		m_pGameInstance->Publish(event);

		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Popup_LadderUp.json";
		m_pGameInstance->Publish(event);


		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/PopUp_MapTitle.json";
		m_pGameInstance->Publish(event);

		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/PopUp_Chest.json";
		m_pGameInstance->Publish(event);

		//BloodCode 윈도우로드
		//인벤토리 슬롯눌렀을때 윈도우들 로드
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Window_BloodCode.json";
		m_pGameInstance->Publish(event);

		//작은미니맵띄우기
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Minimap_Center.json";
		m_pGameInstance->Publish(event);


		//겨우살이 menu
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Mistletoe_Menu.json";
		m_pGameInstance->Publish(event);

		Obj = m_pGameInstance->Find_PersistentUI_ByName(L"Mistletoe_Menu");
		if (Obj)
		{
			Obj->Set_Active(false, false);
			Obj->Bind_On_Active_By_CloseWindow([]()
				{
					//그냥 close로껐을떄.
					IInteractable* pObj = InteractionManager::GetInstance()->Get_Current_Interaction_Target();
					if (pObj && pObj->Get_InteractionType() == INTERACTION_TYPE::SAVEPOINT)
					{
						pObj->Exit_Interaction(nullptr);
						InteractionManager::GetInstance()->Reset_CurrentInteraction();
					}
				});

		}

	
		//scene select menu
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/SceneSelectMenu.json";
		m_pGameInstance->Publish(event);

		Obj = m_pGameInstance->Find_PersistentUI_ByName(L"SceneSelectMenu");
		if (Obj)
			Obj->Set_Active(false, false);

		//NPCDialogue
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/NPC_Dialogue.json";
		m_pGameInstance->Publish(event);


		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/NPC_Menu.json";
		m_pGameInstance->Publish(event);


		////SYSTEM TEXT
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/System_Text.json";
		m_pGameInstance->Publish(event);


		////ShopMenu
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Window_Shop.json";
		m_pGameInstance->Publish(event);

		//Haze warning
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Warning_Haze.json";
		m_pGameInstance->Publish(event);


		//뒤잡 ,패링시 연출
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/BackStabScreen.json";
		m_pGameInstance->Publish(event);

		//동행자 ui
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Buddy_Statusbar.json";
		m_pGameInstance->Publish(event);

		UIObject* pBuddyHpar = m_pGameInstance->Find_UI_ByName(L"Buddy_Statusbar");
		if (pBuddyHpar)
		{
			pBuddyHpar->Set_Visible(false);
			pBuddyHpar->Set_Active(false);

		}
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/PlayerDeadUI.json";
		m_pGameInstance->Publish(event);

		//LevelUpWindow
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Window_Levelup.json";
		m_pGameInstance->Publish(event);


		//Popup
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/LevelUp_PopUp.json";
		m_pGameInstance->Publish(event);

#pragma region levelup _ popup 연결
		/*이벤트연결..*/
		{
			UIObject* pObj = m_pGameInstance->Find_UI_ByName(L"LevelUp_Popup");
			if (pObj)
			{
				UIObject* pButton = pObj->Get_Child(L"Shine_OK");
				if (pButton)
					pButton->Bind_OnClickEvent([]()
						{
							PlayClickSound();

							UI_MasterEvent Event;
							Event.m_bFlag = false;
							Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
							Event.m_ActionName = "ActiveUI";
							Event.m_Text = L"LevelUp_Popup";
							GameInstance::GetInstance()->Publish(Event);

							UIObj_Window_LevelUp::Window_LevelUpUIEvent LevelUpEvent;
							LevelUpEvent.m_bFocus = true;
							GameInstance::GetInstance()->Publish(LevelUpEvent);


							ESCLOCKEVENT LockEvent;
							LockEvent.m_bLock = false;
							GameInstance::GetInstance()->Publish(LockEvent);

						});
			}
		}

#pragma endregion

		//타귀 UI
		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/Monsterwave_UI.json";
		m_pGameInstance->Publish(event);

		event.bLoadJson = true;
		event.eToolType = TOOLTYPE::UI_TOOL;
		event.m_Path = "../../DataFiles/Level_UIDev/MonsterWaveEnd_UI.json";
		m_pGameInstance->Publish(event);




		/////////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//////풀링객체로드///////<<<<<<<<<<<<<<<<<<<</////////
		LOADEVENT_UIPOOLING PoolingEvent;
		PoolingEvent.bLoadJson = true;
		PoolingEvent.eToolType = TOOLTYPE::UI_TOOL;
		PoolingEvent.m_Path = "../../DataFiles/Level_UIDev/Monster_StatusBar.json";
		PoolingEvent.ePoolingType = UI_POOLING_TYPE::MONSTERSTATUSBAR;
		PoolingEvent.m_iSize = 10;
		m_pGameInstance->Publish(PoolingEvent);

	}




	//다했으면 이제 중복로드막아주는  bool true
	m_pGameInstance->Set_AllLoadPersistents(true);

}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Load* Client::Level_Load::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Load* pInstance = new Level_Load(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Load 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Load::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Load 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Load::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}

void Client::Level_Load::Publish_ExitEvent(bool bFadeScreenExit, bool bLoadingExit)
{
	//씬바뀜이벤트~
	if (bFadeScreenExit)
	{
		UIObj_FadeScreen::FadeScreenEvent Event;
		Event.m_fSecond = 4.f;
		Event.eType = UIObj_FadeScreen::FadeScreenEventType::EXIT_SCENE;
		m_pGameInstance->Publish(Event);
	}



	if (bLoadingExit)
	{
		UIObj_LoadingScreen::LoadingScreenEvent LoadingScreenEvent;
		LoadingScreenEvent.eType = UIObj_LoadingScreen::LoadingScreenEventType::DEACTIVE;
		LoadingScreenEvent.m_fSecond = 1.f;

		m_pGameInstance->Publish(LoadingScreenEvent);
	}



	UIObject* pObj = m_pGameInstance->Find_PersistentUI_ByName(L"Loading_Cube");
	if (pObj)
		pObj->Set_Active(false);

}
/******************************************************* 객체 반환 함수 *******************************************************/
