#include "Client_Define.h"
#include "Level_Customize.h"

#include "GameInstance.h"
#include "Player.h"
#include "Level_Load.h"
#include "PoolingManager.h"

#include "CustomizingManager.h"
#include "UIObj_Cursor.h"
#include "Parser_UITool.h"

#include "UIObj_FadeScreen.h"
#include "UISoundUtil.h"



//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Customize::Level_Customize()
{
}

Client::Level_Customize::Level_Customize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level_Client(pDevice, pContext)
{
}

Client::Level_Customize::~Level_Customize()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Customize::Initialize(LEVEL _level)
{
	m_LevelTitle = L"Customize";
	m_pCustomizingManager = CustomizingManager::GetInstance();
	__super::Initialize(_level);

	/* LEVEL::SAMPLE */
	m_iLevel = _UINT(_level);

	CHECK_FAILED(Ready_Load(), E_FAIL);
	CHECK_FAILED(Ready_UIObjects(), E_FAIL);

	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);
	CHECK_FAILED(Ready_Camera(), E_FAIL);
	CHECK_FAILED(Ready_UIObject(), E_FAIL);
	CHECK_FAILED(Ready_Sky_Customize(), E_FAIL);
	CHECK_FAILED(Ready_Values(), E_FAIL);
	g_bDrawDebugCollider = false;



	INPUT_LOCK_EVENT InputEvent;
	InputEvent.bLock = false;
	m_pGameInstance->Publish(InputEvent);

	UIObj_Cursor::CursorEvent cursorEvent;
	cursorEvent.bEnable = true;
	m_pGameInstance->Publish(cursorEvent);

	MouseLockEvent LockEvent;
	LockEvent.bLock = false;
	m_pGameInstance->Publish(LockEvent);


	/*플레이어 생성이후 Initialize()*/
	CustomizingManager::GetInstance()->Initialize();
	m_pGameInstance->Play_Sound("AMB_CHARACTER_CUSTOMIZE_000_Play", 0.f, true);

	ChangeColor();

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// UI 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Customize::Ready_UIObject()
{
	__super::Ready_UIObject();

	//이 씬에서만사용하는 UI로드

	return S_OK;
}
/******************************************************* UI 준비 함수 *******************************************************/


//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Customize::Ready_Player()
{
	Player* pPlayer = DCAST(Player*)(m_pGameInstance->Get_Player());
	if (pPlayer)
	{
		// 0, 0, 0 고정
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		pPlayer->Set_Position_ByController(vPosition);
		pPlayer->Set_Active(true);
		pPlayer->Set_Gravity(false);
		pPlayer->Enter_CustomScene();
	}
	else
	{
		BREAK;
		return S_OK;
	}

	m_bMapCreate = true;
	return S_OK;
}

HRESULT Client::Level_Customize::Ready_Camera()
{
	Camera* playerCam = nullptr;

	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));							// 자유시점 카메라
	//m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Player, m_iLevel, _UINT(CAMERA::PLAYER_FOCUS_01), RCAST(GameObject**)(&playerCam));							// 플레이어 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Customize, m_iLevel, _UINT(CAMERA::CUSTOMIZE));				// 커스터마이징 카메라
	m_pGameInstance->Camera_Change(_UINT(CAMERA::CUSTOMIZE));

	//if (playerCam)
	//{
	//	//미니맵매니저에도 연결해주기
	//	MinimapManager::GetInstance()->Set_RotateCamera(playerCam);
	//}

	return S_OK;
}

HRESULT Client::Level_Customize::Ready_Light()
{
	//LIGHT_DESC Desc;
	//Desc.eType = LIGHT::DIRECTIONAL;
	//Desc.vDiffuse = { 1.f, 1.f, 1.f, 1.f };
	//Desc.vAmbient = { 0.5f, 0.5f, 0.5f, 1.f };
	//Desc.vSpecular = { 0.8f, 0.85f, 0.9f, 1.f }; //0.3으로 해놨었음 
	//Desc.vDirection = { 0.f, -0.6f, 1.f, 0.f };

	////임시라서 그냥 자체 생산?
	//m_pGameInstance->Add_Light(0, Desc);

	//Desc.eType = LIGHT::POINT;
	//Desc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	//Desc.fRange = 10.f;
	//Desc.vDiffuse = _float4(1.f, 0.3f, 0.3f, 1.f);
	//Desc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
	//Desc.vSpecular = Desc.vDiffuse;

	//m_pGameInstance->Add_Light(1, Desc);

	//Desc.eType = LIGHT::POINT;
	//Desc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	//Desc.fRange = 10.f;
	//Desc.vDiffuse = _float4(1.f, 0.3f, 0.3f, 1.f);
	//Desc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
	//Desc.vSpecular = Desc.vDiffuse;

	//m_pGameInstance->Add_Light(2, Desc);
	//GameObject* temp = m_pGameInstance->Get_GameObject(_UINT(LEVEL::MAIN), L"Layer_Player", L"Player");

	SHADOW_DESC		ShadowDesc{};
	ShadowDesc.vPosition = _float4{ 0.f, 14.f, -20.f, 1.f };
	ShadowDesc.vAt = _float4{ -155.61f, 32.14f, 65.48f, 1.f };
	ShadowDesc.vOrthSize = 500;//XMConvertToRadians(45.0f);
	ShadowDesc.fNearZ = 0.1f;
	ShadowDesc.fFarZ = 500.f;
	ShadowDesc.fDepth = 500;

	if (FAILED(m_pGameInstance->Add_ShadowLight(ShadowDesc)))
		return E_FAIL;

	SHADOW_DESC		ShadowDesc1{};
	ShadowDesc1.vPosition = _float4{ 0.f, 1.f, -1.f, 1.f }; //높이서 내려다봄
	ShadowDesc1.vAt = _float4{ 0.f, 0.f, 0.f, 1.f };
	ShadowDesc1.vOrthSize = 5.f;
	ShadowDesc1.fNearZ = 0.1f;
	ShadowDesc1.fFarZ = 500.f;
	ShadowDesc1.fDepth = 200.f;
	ShadowDesc1.vAddPos = _float4(0.f, 10.f, 0.f, 1.f);

	const _int NumCascade = 4;
	_float bias[NumCascade] = { 0.001f, 0.002f, 0.003f, 0.005f };
	if (FAILED(m_pGameInstance->Add_ShadowCascadeLight(NumCascade, ShadowDesc1, bias)))
		return E_FAIL;

	return S_OK;
}
HRESULT Client::Level_Customize::Ready_Load()
{
	// 카메라 매니저를 리셋해줌
	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	// 원하는 카메라들을 모두 추가(enum class는 레벨마다 달라도 됨)
	//m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));
	// 레벨에서 첫 번째로 쓸 카메라로 변경
	//m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));

	m_pGameInstance->Set_UIManager_Active(true);


	Parser_UITool::Set_LoadLevel(LEVEL::CUSTOMIZE);

	return S_OK;
}

HRESULT Client::Level_Customize::Ready_UIObjects()
{
	LOADEVENT event;
	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/Customizing_UIData.json";
	event.m_bPersistent = false;
	event.ProtoLevel = LEVEL::CUSTOMIZE;
	m_pGameInstance->Publish(event);

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

HRESULT Client::Level_Customize::Ready_Values()
{
	//메뉴들 켜주기->최종본에서는 하나의 window로 관리

	wstring WindowNames[] = { L"Custom_Menu",L"Window_Info",L"Scene_Name",L"Default_Camera_UI",L"Confirm_Button"};

	for (int i = 0; i < sizeof(WindowNames) / sizeof(WindowNames[0]); ++i)
	{
		UI_MasterEvent Event;
		Event.m_ActionName = "ActiveUI";
		Event.m_Text = WindowNames[i];
		Event.m_bFlag = true;
		Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
		m_pGameInstance->Publish(Event);
	}

	UIObject* pObj = m_pGameInstance->Find_UI_ByName(L"Confirm_Button");
	if (pObj)
	{
		pObj->Bind_OnClickEvent([this]()
			{
				PlayClickSound();

				//씬이동
				UIObj_FadeScreen::FadeScreenEvent Event;
				Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
				Event.m_iNextLevel = LEVEL::MAIN;
				m_pGameInstance->Publish(Event);

			});
	}
	return S_OK;
}

HRESULT Client::Level_Customize::Ready_Sky_Customize()
{
	m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), Proto_GameObject_SkySphere, m_iLevel, Layer_Sky);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
void Client::Level_Customize::ChangeColor()
{
	m_pGameInstance->ChangeCustomize();
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/


//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Customize::Update_Priority(const _float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(DIK_F7))
	{
		if (m_bCamera)
		{
			m_bCamera = false;
			m_pGameInstance->Camera_Change(_UINT(CAMERA::CUSTOMIZE));
		}
		else
		{
			m_bCamera = true;
			m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));
		}
	}

	m_pGameInstance->Update_Priority(fTimeDelta);

	// 메인 레벨로 이동
	//if (m_pGameInstance->KeyDown(DIK_F8))
	//{
	//	Publish_ExitEvent(true, false);
	//
	//	m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::MAIN));
	//	m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));
	//	return 0;
	//}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Customize::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update_Parallel(fTimeDelta);
	// m_pGameInstance->Update_PhysX(fTimeDelta);

	//Test_WeaponChange();

	m_pGameInstance->Update(fTimeDelta);
	m_pCustomizingManager->Update(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Customize::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);
	//Test_SavePoint(fTimeDelta);
	
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Customize::Render(const _float fTimeDelta)
{
	//MinimapManager::GetInstance()->Clear_Icons_On_UI();
	if (m_bMapCreate)
	{
		m_pGameInstance->Set_Capture(true);
		m_bMapCreate = false;
	}
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Customize* Client::Level_Customize::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Customize* pInstance = new Level_Customize(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Customize 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Customize::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Customize 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Customize::Free()
{
	
	m_pGameInstance->StopSound("AMB_CHARACTER_CUSTOMIZE_000_Play");

	m_pGameInstance->Clear_Level_Prototype(_UINT(LEVEL::CUSTOMIZE));
	m_pGameInstance->Clear_Level_GameObject(_UINT(LEVEL::CUSTOMIZE));
	CAST(Player*)(m_pGameInstance->Get_Player())->Set_Gravity(true);

	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
