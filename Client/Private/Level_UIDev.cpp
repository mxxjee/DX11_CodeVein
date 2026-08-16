#include "Client_Define.h"
#include "Level_UIDev.h"

#include "GameInstance.h"
#include "Parser_UITool.h"
#include "Level_UIDev.h"
#include "UIObj_GuardBar.h"
#include "UIObj_Stamina.h"
#include "UIObj_BitmapFont.h"

#include "InventoryManager.h"
#include "ItemManager.h"
#include "UIObj_BossName.h"
#include "UIObj_HpBar.h"

#include "Player.h" //플레이어 스탯 가져오기
#include "Player_Stat.h"
#include "UIObj_PopUp_MapTitle.h"

#include "CustomizingManager.h"
#include "UIObj_Cursor.h"


Client::Level_UIDev::Level_UIDev()
{
}

Client::Level_UIDev::Level_UIDev(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

Client::Level_UIDev::~Level_UIDev()
{
}


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_UIDev::Initialize(LEVEL _level)
{
	/* LEVEL::SAMPLE */
	m_iLevel = _UINT(_level);

	//Parser로드 시작
	CHECK_FAILED(Ready_Load(), E_FAIL);
	CHECK_FAILED(Ready_UIObjects(), E_FAIL);

	//UI값연결
	CHECK_FAILED(Ready_Values(), E_FAIL);

#ifdef _DEBUG
	UIComponent::Set_DrawDebug(m_bDrawDebug);

#endif // DEBUG


#pragma region PlayerHUD관련 active
	///*UIObject* pPlayerHud = m_pGameInstance->Find_UI_ByName(L"PlayerHud_Left");
	//if (pPlayerHud)
	//	pPlayerHud->Set_Active(true,true);

	//UIObject* pPlayerMenu = m_pGameInstance->Find_UI_ByName(L"Window_PlayerMenu");
	//if (pPlayerMenu)
	//	pPlayerMenu->Set_Active(false);

	//UIObject* pBloodcode = m_pGameInstance->Find_UI_ByName(L"Window_Bloodcode");
	//if (pBloodcode)
	//	pBloodcode->Set_Active(false);

	//UIObject* pShortCutMenu = m_pGameInstance->Find_UI_ByName(L"ShortCut_Menu");
	//if (pShortCutMenu)
	//	pShortCutMenu->Set_Active(false);*/

	////UI_MasterEvent Event;
	////Event.m_ActionName = "OpenWindow";
	////Event.m_Text = L"Window_PlayerMenu";
	////Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	////Event.m_ActionStrHash = hash<string>{}(Event.m_ActionName);
	////
	////m_pGameInstance->Publish(Event);

	//UIObject* pWindow_Storage = m_pGameInstance->Find_UI_ByName(L"Window_Storage");
	//if (pWindow_Storage)
	//	pWindow_Storage->Set_Active(false);


	//UIObject* pInteraction_UI = m_pGameInstance->Find_UI_ByName(L"PopUp_Interaction");
	//if (pInteraction_UI)
	//	pInteraction_UI->Set_Active(false);


	////m_pGameInstance->Change_UIMode(UI_MODE::DEFAULT);

#pragma endregion
	//InventoryManager::GetInstance()->Initialize(m_pDevice,m_pContext);
	//ItemManager::GetInstance()->Initialize(m_pDevice, m_pContext);

	//InventoryManager::GetInstance()->Set_Default_Inventory();

	//세팅
	//CustomizingManager::GetInstance()->Set_FocusType(CUSTOMIZING_TYPE::SKIN);

	UIObj_Cursor::CursorEvent cursorEvent;
	cursorEvent.bEnable = true;
	m_pGameInstance->Publish(cursorEvent);

	/*플레이어 생성이후 Initialize()*/
	CustomizingManager::GetInstance()->Initialize();


	return S_OK;
}
HRESULT Client::Level_UIDev::Ready_Load()
{
	// 카메라 매니저를 리셋해줌
	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	// 원하는 카메라들을 모두 추가(enum class는 레벨마다 달라도 됨)
	//m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));
	// 레벨에서 첫 번째로 쓸 카메라로 변경
	//m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));

	m_pGameInstance->Set_UIManager_Active(true);

	//Parser
	m_pParsing_UI->Set_LoadLevel(LEVEL::UIDEV);



	return S_OK;
}
HRESULT Client::Level_UIDev::Ready_UIObjects()
{
	LOADEVENT event;
	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/Customizing_UIData.json";
	event.m_bPersistent = false;
	event.ProtoLevel = LEVEL::CUSTOMIZE;
	m_pGameInstance->Publish(event);

	{
		//가이드사진 불러오기
		UIObject::UIOBJECT_DESC UIDesc;
		UIDesc.fCX = WINCX;
		UIDesc.fCY = WINCY;
		UIDesc.fX = WINCX / 2.f;
		UIDesc.fY = WINCY / 2.f;
		UIDesc.iShaderNumber = 4;
		UIDesc.wstrName = L"Back";
		UIDesc.fAlpha = 0.3f;



		GameObject* pOut = nullptr;

		_wstring layer = Layer_UIs;
		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(0, L"Prototype_UIObject", m_iLevel, layer, &pOut, &UIDesc)))
			return E_FAIL;

		if (pOut)
		{
			pGuideUI = dynamic_cast<UIObject*>(pOut);
			UI_Image::UIIMAGEDESC Desc;
			Desc.pOwner = pGuideUI;
			Desc.TextureKey = "Guide/CustomizingGuide";
			pGuideUI->Set_Interatable(false);

			if (FAILED(pGuideUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &Desc)))
				return E_FAIL;

		}
	}

	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/ColorPalette.json";
	event.m_bPersistent = false;
	m_pGameInstance->Publish(event);
	


	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/ItemGrid.json";
	event.m_bPersistent = false;
	m_pGameInstance->Publish(event);




	return S_OK;
}
HRESULT Client::Level_UIDev::Ready_Values()
{
	//메뉴들 켜주기->최종본에서는 하나의 window로 관리

	wstring WindowNames[] = { L"Custom_Menu",L"Window_Info",L"Scene_Name",L"Default_Camera_UI" };

	for (int i = 0; i < sizeof(WindowNames) / sizeof(WindowNames[0]); ++i)
	{
		UI_MasterEvent Event;
		Event.m_ActionName = "ActiveUI";
		Event.m_Text = WindowNames[i];
		Event.m_bFlag = true;
		Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
		m_pGameInstance->Publish(Event);
	}





	return S_OK;
}
void Client::Level_UIDev::Free()
{
	__super::Free();
	//InventoryManager::GetInstance()->DestroyInstance();
	Safe_Release(m_pParsing_UI);
}
/******************************************************* 객체 준비 함수 *******************************************************/
//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_UIDev::Update_Priority(const _float fTimeDelta)
{
	m_pGameInstance->Update_Priority(fTimeDelta);
	if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_D))
	{
		m_bDrawDebug = !m_bDrawDebug;

#ifdef _DEBUG
		UIComponent::Set_DrawDebug(m_bDrawDebug);
#endif // DEBUG


	}

	/*가드 이벤트 실행*/
	//피격시..실행할것들(가드 게이지 갱신 / hp 줄어들기)
	if (m_pGameInstance->KeyDown(DIK_SPACE))
	{

		//BossNameTest
		UIObj_BossName::BossNameUIEVent BossNameEvent;
		BossNameEvent.bActive = true;
		BossNameEvent.BossName_Korean = L"올리버 콜린스";
		BossNameEvent.BossName_English = L"Oliver Collins";
		
		m_pGameInstance->Publish(BossNameEvent);

		//가드값 줄어들고있는 와중엔 처리X
		CHECK_TRUE_RESULT(m_PlayerStatus.m_bFocusState,0);
		//m_PlayerStatus.fCurrentHp -= m_pGameInstance->RandomValue(3.f, 10.f);


		//집중상태 값 처리!!
		m_PlayerStatus.fCurrentGuard += m_pGameInstance->RandomValue(3.f, 7.f);
		m_PlayerStatus.fCurrentGuard = clamp<float>(m_PlayerStatus.fCurrentGuard, 0.f, m_PlayerStatus.fMaxGuard);

		UI_MasterEvent	Event;
		Event.m_ActionName = "OnGuardEvent";
		Event.m_EventTarget = UI_EVENT_TARGET::UIOBJECT;
		Event.m_bFlag = true;
		
		Event.m_fValue = 0;	//처리할 guard바의 objectID보내기

		Event.m_ActionStrHash = hash<string>{}(Event.m_ActionName);
		m_pGameInstance->Publish(Event);

	
	};

	//회피 시 갱신할데이터 - 스테미너
	if (m_pGameInstance->KeyPress(DIK_W) && m_pGameInstance->KeyPress(DIK_LSHIFT))
	{
		m_PlayerStatus.Walk(fTimeDelta);
		
		 


	}

	else
		m_PlayerStatus.m_bUseStamina = false;

	//if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
	//{
	//	/*m_PlayerStatus.fCurrentHp -= m_pGameInstance->RandomValue(5.f, 14.f);
	//	m_PlayerStatus.fCurrentHp = clamp<float>(m_PlayerStatus.fCurrentHp, 0.f, m_PlayerStatus.fMaxHp);*/

	//	UIObject* pInteraction_UI = m_pGameInstance->Find_UI_ByName(L"PopUp_Interaction");
	//	if (pInteraction_UI)
	//		pInteraction_UI->Set_Active(true);


	//}

	if (m_pGameInstance->KeyDown(DIK_P))
	{
		UI_MasterEvent Event;
		Event.m_ActionName = "ActiveUI";
		Event.m_Text = L"PopUp_SavePoint";
		Event.m_bFlag = true;
		Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;

		m_pGameInstance->Publish(Event);

	}
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_UIDev::Update(const _float fTimeDelta)
{

	m_pGameInstance->Update_Parallel(fTimeDelta);
	m_pGameInstance->Update(fTimeDelta);
	m_PlayerStatus.Update(fTimeDelta);

	if (m_pGameInstance->KeyDown(DIK_C))
	{
		bActive = !bActive;

		if (pGuideUI)
			pGuideUI->Set_Active(bActive);


	}
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_UIDev::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

#pragma region 아이템 들어오는거 확인용
	////ItemTest
	/*if (m_pGameInstance->KeyDown(DIK_NUMPAD1))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"블러드 샷");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD2))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"블레이즈 로어");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD3))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"기프트힐");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}


	if (m_pGameInstance->KeyDown(DIK_NUMPAD4))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"헤비 액스");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD5))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"체력 증가");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD6))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"명혈 농축약");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD7))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"여왕 토벌대 아장/가시형");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}


	if (m_pGameInstance->KeyDown(DIK_NUMPAD8))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"재생력");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD9))
	{
		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"캐스터");
		InventoryManager::GetInstance()->Add_Item(pItemInfo);

		Safe_Delete(pItemInfo);
	}*/
	if (m_pGameInstance->KeyDown(DIK_DOWNARROW))
	{
		wstring Texts[] = { L"AREA D-12  Ruined City UnderGround",
		L"Home Base" };

		UIObj_PopUp_MapTitle::MapTitleUIEvent Event;
		Event.m_Text = Texts[idx];
		m_pGameInstance->Publish(Event);

		//현재 포커스아이템사용하기(아이템)
		InventoryManager::GetInstance()->Use_FocusItem();

		++idx;
		if (idx > 1)
			idx = 0;

	}


#pragma endregion
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_UIDev::Render(const _float fTimeDelta)
{

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_UIDev* Client::Level_UIDev::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_UIDev* pInstance = new Level_UIDev(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_UIDev 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_UIDev::Initialize_Level()
{
	Initialize(CAST(LEVEL)(m_iLevel));
}
/******************************************************* 생성자 호출 함수 *******************************************************/

