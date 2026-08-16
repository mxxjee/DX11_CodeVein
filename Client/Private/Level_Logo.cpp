#include "Client_Define.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Load.h"
#include "Level_UIDev.h"
#include "UIObject.h"

#include "Parser_UITool.h"
#include "UIObj_FadeScreen.h"
#include "MinimapManager.h"
#include "UIObj_Cursor.h"

#include "InteractionManager.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Logo::Level_Logo()
{
}

Client::Level_Logo::Level_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

Client::Level_Logo::~Level_Logo()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Logo::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	////버튼 눌렀을때 씬이동 [이벤트구독]
	//iEventHandle = m_pGameInstance->Subscribe<LevelChangeEvent>([this](const LevelChangeEvent& e)
	//	{
	//		
	//		m_pGameInstance->Clear_Scene_UI();
	//			


	//		m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, e.eNextLevel));
	//		m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));


	//	});
	m_pGameInstance->Set_UIManager_Active(true);
	//m_pParsing_UI = Parser_UITool::Create();

	Parser_UITool::Set_LoadLevel((LEVEL)m_iLevel);

	CHECK_FAILED(Ready_UI(), E_FAIL);
	CHECK_FAILED(Ready_Values(), E_FAIL);

	MinimapManager::GetInstance()->Initialize(m_pDevice, m_pContext, (_uint)WINCX, (_uint)WINCY);


	m_SoundAlarm.Limit = 0.05f;
	m_SoundAlarm.m_AlarmFunc = [this]()
		{
			SOUND_EVENT soundEvent;
			soundEvent.ePhase = ANIM_FRAMEPHASE::START;
			soundEvent.SoundName = "Opening";
			soundEvent.fVolume = 0.f;
			soundEvent.bLoop = true;
			m_pGameInstance->Publish(soundEvent);
			m_SoundAlarm.Off();
		};

	m_SoundAlarm.On();

	

	/*마우스 기본값 = lock*/
	MOUSELOCK_EVENT mouseEvent;
	mouseEvent.bLock = false;
	m_pGameInstance->Publish(mouseEvent);

	//커서끄기
	UIObj_Cursor::CursorEvent cursorEvent;
	cursorEvent.bEnable = true;
	m_pGameInstance->Publish(cursorEvent);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// UI 준비 ////////////////////////////////////////////////////////
HRESULT Client::Level_Logo::Ready_UI()
{
	_wstring layer = Layer_UIs;

	//타이틀씬 세트
	LOADEVENT event;
	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/Title.json";
	event.m_bPersistent = false;

	m_pGameInstance->Publish(event);


	////Select는 따로불러서 persistent등록
	//LOADEVENT Persistentevent;
	//Persistentevent.bLoadJson = true;
	//Persistentevent.eToolType = TOOLTYPE::UI_TOOL;
	//Persistentevent.m_Path = "../../DataFiles/Level_UIDev/Selector.json";
	//Persistentevent.m_bPersistent = true;

	//m_pGameInstance->Publish(Persistentevent);



	return S_OK;
}

HRESULT Client::Level_Logo::Ready_Values()
{

	/*UIObject* pShortCutMenu = m_pGameInstance->Find_UI_ByName(L"ShortCut_Menu");
	if (pShortCutMenu)
		pShortCutMenu->Set_Active(false);

	UIObject* pInteraction_UI = m_pGameInstance->Find_UI_ByName(L"PopUp_Interaction");
	if (pInteraction_UI)
		pInteraction_UI->Set_Active(false);




	UIObject* pPlayerHud = m_pGameInstance->Find_UI_ByName(L"PlayerHud_Left");
	if (pPlayerHud)
		pPlayerHud->Set_Active(true, true);

	UIObject* pPlayerMenu = m_pGameInstance->Find_UI_ByName(L"Window_PlayerMenu");
	if (pPlayerMenu)
		pPlayerMenu->Set_Active(false);

	UIObject* pWindow_Storage = m_pGameInstance->Find_UI_ByName(L"Window_Storage");
	if (pWindow_Storage)
		pWindow_Storage->Set_Active(false);


	UIObject* pBloodcode = m_pGameInstance->Find_UI_ByName(L"Window_Bloodcode");
	if (pBloodcode)
		pBloodcode->Set_Active(false);*/


	////비활성화 이벤트 전송
	////미니맵 비활성화
	//UI_MasterEvent MasterEvent;
	//MasterEvent.m_ActionName = "ActiveUI";
	//MasterEvent.m_Text = L"Minimap_Center";
	//MasterEvent.m_bFlag = false;
	//m_pGameInstance->Publish(MasterEvent);


	return S_OK;
}
/******************************************************* UI 준비 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Logo::Update_Priority(const _float fTimeDelta)
{
	if (m_bKeyPressed)
		goto UpdatePriority;

	if (m_pGameInstance->KeyDown(DIK_SPACE))
	{
		////페이드인 ui 활성화이벤트
		//UIObj_FadeScreen::FadeScreenEvent Event;
		//Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
		//m_pGameInstance->Publish(Event);


		//m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::MAIN));
		//m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));

		return 0;
	}
	else if (m_pGameInstance->KeyDown(DIK_F11))
	{
		//페이드인 ui 활성화이벤트
		InteractionManager::GetInstance()->Set_LastSavePointInfo(LEVEL::SAMPLE, 0);


		UIObj_FadeScreen::FadeScreenEvent Event;
		Event.m_iNextLevel = LEVEL::SAMPLE;
		Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
		m_pGameInstance->Publish(Event);

		m_bKeyPressed = true;
	
		return 0;
	}

	else if (m_pGameInstance->KeyDown(DIK_F6))
	{
		//페이드인 ui 활성화이벤트
		InteractionManager::GetInstance()->Set_LastSavePointInfo(LEVEL::BASE, 0);

		UIObj_FadeScreen::FadeScreenEvent Event;
		Event.m_iNextLevel = LEVEL::BASE;
		Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
		m_pGameInstance->Publish(Event);

		m_bKeyPressed = true;
		return 0;
	}
	else if (m_pGameInstance->KeyDown(DIK_F7))
	{
		//페이드인 ui 활성화이벤트
		InteractionManager::GetInstance()->Set_LastSavePointInfo(LEVEL::CHURCH, 0);

		UIObj_FadeScreen::FadeScreenEvent Event;
		Event.m_iNextLevel = LEVEL::CHURCH;
		Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
		m_pGameInstance->Publish(Event);
		m_bKeyPressed = true;
		return 0;
	}
	else if (m_pGameInstance->KeyDown(DIK_F1))
	{
		m_pGameInstance->Clear_Scene_UI();

		m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::UIDEV));
		m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));
		m_bKeyPressed = true;
		return 0;
	}
	else if (m_pGameInstance->KeyDown(DIK_F2))
	{
		UIObj_FadeScreen::FadeScreenEvent Event;
		Event.m_iNextLevel = LEVEL::CUSTOMIZE;
		Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
		m_pGameInstance->Publish(Event);

		m_bKeyPressed = true;

		return 0;
	}

	//TEXT_DESC desc;
	//desc.eSetting = TEXTDRAW_SETTING::DIR8;
	//m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{WINCX * 0.5f - 50.f, WINCY * 0.5f}, .vSize{3.f, 3.f}, .wstrDrawText{L"F1=UI로딩씬.\nSpace=Main씬."}, .wstrFontName{Font_M10}});


UpdatePriority:

	m_pGameInstance->Update_Priority(fTimeDelta);

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Logo::Update(const _float fTimeDelta)
{
	m_SoundAlarm.Update(fTimeDelta);
	m_pGameInstance->Update(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Logo::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Logo::Render(const _float fTimeDelta)
{


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Logo* Client::Level_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Logo* pInstance = new Level_Logo(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Logo 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Logo::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Logo 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Logo::Free()
{
	SOUND_EVENT soundEvent;
	soundEvent.ePhase = ANIM_FRAMEPHASE::END;
	soundEvent.SoundName = "Opening";
	soundEvent.bLoop = true;

	m_pGameInstance->Publish(soundEvent);

	m_pGameInstance->UnsubScribe(iEventHandle);

	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
