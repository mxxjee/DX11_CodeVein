#include "Client_Define.h"
#include "MainApp.h"
#include "GameInstance.h"

#include "Loader.h"
#include "Level_Load.h"
#include "Mouse.h"

#include "UIObj_Cursor.h"
#include "UIObj_FadeScreen.h"
#include "UIObj_LoadingScreen.h"
#include "UIObj_LoadingCube.h"
#include "UIObj_Text.h"

#include "PhysXManager.h"
#include "PoolingManager.h"
#include "InventoryManager.h"
#include "ItemManager.h"
#include "MinimapManager.h"
#include "InteractionManager.h"
#include "DialogueManager.h"
#include "SkillManager.h"
#include "ShopManager.h"
#include "CustomizingManager.h"



#include "Parser_UITool.h"
#include "UIObj_LockOn.h"
#include "UIObj_FocusMenu.h"


#include "Camera_Object.h"
#include "Player.h"
#include "GameClock.h"
#include "PartObject.h"

#include "UISoundUtil.h"



#pragma region IMGUI
#include "ImguiManager.h"
#include "IMGUI_Main.h"
#include "IMGUI_GameObject.h"
#include "ImguiRTV.h"
#include "IMGUI_TransitionTable.h"
#include "IMGUI_MonsterEditor.h"

#pragma endregion


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
MainApp::MainApp()
{
}

MainApp::~MainApp()
{
}
//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MainApp::Initialize()
{
	srand((unsigned)time(NULL));

	MSG_FAIL(CoInitializeEx(nullptr, COINIT_MULTITHREADED), L"CoInit실패", L"망했음ㅋㅋ", E_FAIL);

#ifdef _DEBUG
	g_fFPSRate = FPS_60;
	g_bIMGUI_UIOn = true;
	// F10 눌러서 렌더타겟 켜고끄기
	g_bDrawRTV = false;
	// g_toolType = TOOLTYPE::SHADER_TOOL;
	g_bOmniPVDRecording = true;
	g_bDrawDebugCollider = true;
	g_bPhysXDebug = true;
	g_bClient = true;
#else
	g_fFPSRate = FPS_60;
	g_bIMGUI_UIOn = true;
	// F10 눌러서 렌더타겟 켜고끄기
	g_bDrawRTV = false;
	// g_toolType = TOOLTYPE::SHADER_TOOL;
	g_bOmniPVDRecording = false;
	g_bDrawDebugCollider = false;
	g_bPhysXDebug = false;
	g_bClient = true;
#endif // _DEBUG

	g_vHDRColor = _float3{ 0.f, 0.f, 0.f };



	ENGINE_DESC tdesc{};

	tdesc.iNumLevels = _uint(LEVEL::END);
	tdesc.hWnd = g_hWnd;
	tdesc.hInstance = g_hInstance;

	m_pGameInstance = GameInstance::GetInstance();
	m_pGameInstance->Set_EngineMode(ENGINEMODE::CLIENT);
	m_pGameInstance->Initialize_Device(g_hWnd, WINMODE::WIN, WINCX, WINCY, &m_pDevice, &m_pContext);
	m_pGameInstance->Initialize(tdesc);
	Safe_AddRef(m_pGameInstance);


	m_pInventoryManager = InventoryManager::GetInstance();
	m_pInventoryManager->Initialize(m_pDevice, m_pContext);
	m_pMinimapManager = MinimapManager::GetInstance();


	ItemManager::GetInstance()->Initialize(m_pDevice, m_pContext);
	PoolingManager::Get_Instance()->Initialize();
	DialogueManager::GetInstance()->Initialize();
	SkillManager::GetInstance()->Initialize();
	

	m_vecWolfSceneKeyFrames = CinematicPreset::Load("../../DataFiles/CinematicPreset/WolfGhost.json");

	m_pShopManager = ShopManager::GetInstance();
	m_pShopManager->Initialize();


	m_pInteractionManager = InteractionManager::GetInstance();
	m_pInteractionManager->Initialize();


	/* 사운드 로딩 및 재생 방법 */

	//m_pGameInstance->LoadSoundFolder(L"../../Resources/Sounds/Monster/Attack_Bark");
	//m_pGameInstance->LoadSoundFolder_Recursive(L"../../Resources/Sounds/Monster");
	//
	//m_pGameInstance->Play_Sound_RandomInGroup("Attack_Bark");





	// 로딩순서 : fadescreen등 로딩씬에 필요한것들 먼저 로드 -> 페이드스크린 띄우기 ->이후 진짜 static로드
	CHECK_FAILED(Ready_Loading(), E_FAIL);
	CHECK_FAILED(Ready_Persistent(), E_FAIL);

	CHECK_FAILED(Ready_Static(), E_FAIL);
	CHECK_FAILED(Register_HotKey(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);

#ifdef _DEBUG
	CHECK_FAILED(Ready_IMGUI(), E_FAIL);
#else
	if (g_bIMGUI_UIOn)
		CHECK_FAILED(Ready_IMGUI(), E_FAIL);
#endif // _DEBUG


	CHECK_FAILED(Start_Level(), E_FAIL);

	/*플레이어 생성이후 Initialize()*/
	//m_pCustomizingManager->Initialize();

	/*게임커서만들기(딱히 레이어에넣지않음)*/
	UIObject::UIOBJECT_DESC Desc;
	Desc.fCX = 50.f;
	Desc.fCY = 50.f;
	Desc.iZOrder = 99;
	Desc.wstrName = L"UI_Cursor";
	m_pCursor = UIObj_Cursor::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC), &Desc);

	//일단 커서숨겨!
	UIObj_Cursor::CursorEvent Event;
	Event.bEnable = false;
	m_pGameInstance->Publish(Event);

	LockHandle = m_pGameInstance->Subscribe< ESCLOCKEVENT>([this](const ESCLOCKEVENT& e)
		{
			m_bLockEsc = e.m_bLock;
		});

	//쉐이더 디버깅용 나중에 지우기
	//ID3D11Debug* debug;
	//m_pDevice->QueryInterface(IID_PPV_ARGS(&debug));

	//ID3D11InfoQueue* infoQueue;
	//debug->QueryInterface(IID_PPV_ARGS(&infoQueue));
	//infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
	return S_OK;
}

HRESULT Client::MainApp::Ready_Loading()
{
	m_pParsing_UI = Parser_UITool::Create();



	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Bookk_Bold, L"../../Resources/Font/Bookk-Bold.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Bookk_Bold_Eng, L"../../Resources/Font/BokkMyungjo_Eng.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Shine_Bold, L"../../Resources/Font/Shine_Bold.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_A2Z_5_Medium, L"../../Resources/Font/A2Z5Medium.spritefont"), E_FAIL);



#pragma region POSTEX_SHADER
	SHADERENTRY postexentry[] = {
	{"VS_MAIN", "PS_MAIN"},
	{"VS_MAIN", "PS_MAIN_COMMNET"},
	{"VS_MAIN", "PS_MAIN_MASKING"},
	{"VS_MAIN", "PS_MAIN_COLOR_MUL"},
	{"VS_MAIN", "PS_NO_DISCARD" },

	{"VS_MAIN", "PS_PROGRESS_HORIZONTAL" },
	{"VS_MAIN", "PS_PROGRESS_HORIZONTAL_RE" },
	{"VS_MAIN", "PS_PROGRESS_VERTICAL" },
	{"VS_MAIN", "PS_PROGRESS_VERTICAL_RE" },
	{"VS_MAIN", "PS_PROGRESS_RADIAL" },
	{"VS_MAIN","PS_PROGRESS_CENTER"},
	{"VS_MAIN","PS_NOISE"},
	{"VS_MAIN","PS_SMOKE_TEST"},
	{"VS_MAIN","PS_SPRITE"},
	{"VS_MAIN","PS_HPBarInner"},
	{"VS_MAIN","PS_WorldDissolve"},
	{"VS_MAIN","PS_BlurUI"},
	{"VS_MAIN","PS_MinimapUI"},
	{"VS_MAIN","PS_CENTERED_MINIMAP"},
	{"VS_MAIN","PS_Distortion"},
	{ "VS_MAIN","PS_WorldUI_Blur" },
	{ "VS_MAIN","PS_PaletteArea" },
		 {"VS_MAIN","PS_TITLE"},
	  {"VS_MAIN","PS_LIGHT"}


	};
	SHADERENTRIES entries;
	entries.pEntries = postexentry;
	entries.iNumpass = sizeof(postexentry) / sizeof(SHADERENTRY);
	/* For.Prototype_Component_Shader_VTXPosTex */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXPosTex,
		Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries)), E_FAIL);
#pragma endregion

#pragma region PosColor
	SHADERENTRY poscolorentry[2] = {
   {"VS_MAIN", "PS_MAIN"},
	{"VS_MAIN", "PS_SELECT"}
	};
	SHADERENTRIES poscolorentries;
	poscolorentries.pEntries = poscolorentry;
	poscolorentries.iNumpass = sizeof(poscolorentry) / sizeof(SHADERENTRY);
	/* For.Prototype_Component_Shader_VTXPoscolor */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXPosCorlor,
		Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosColor.hlsl", VTXPOSCOR::Elements, VTXPOSCOR::iNumElements, poscolorentries)), E_FAIL);

#pragma endregion

	//////////전역오브젝트////////////////////

/////////////전역컴포넌트(커서&페이드스크린 땜애 필요함)///////////////
		/* For.Prototype_Component_VIBuffer_Rect */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_VIRect,
		VIBuffer_Rect::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_Texture_UIResource */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UITexture(L"UIResource"),
		NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/UI/Textures/")), E_FAIL);

	/* For.Prototype_Component_UIRender */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIRender,
		UI_Render::Create(m_pDevice, m_pContext)), E_FAIL);


	/* For.Prototype_Component_UI_Image */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIImage,
		UI_Image::Create(m_pDevice, m_pContext)), E_FAIL);


	/* For.Prototype_Component_UI_Progress */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIAnimation,
		UI_Animation::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIText,
		UI_Text::Create(m_pDevice, m_pContext)), E_FAIL);

	//uibase 클래스
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_UIObject",
		UIObject::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC))), E_FAIL);

	////////Persistent
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_FadeScreen,
		UIObj_FadeScreen::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_LoadingScreen,
		UIObj_LoadingScreen::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_LoadingCube,
		UIObj_LoadingCube::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_TEXT,
		UIObj_Text::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC))), E_FAIL);

	//PErsistent
	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_LockOn,
		UIObj_LockOn::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC))), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_UIObject_FocusMenu,
		UIObj_FocusMenu::Create(m_pDevice, m_pContext, _UINT(LEVEL::STATIC))), E_FAIL);




	return S_OK;
}

HRESULT Client::MainApp::Ready_Static()
{
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_Default, L"../../Resources/Font/Terraria20.spritefont"), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Load_Font(Font_M10, L"../../Resources/Font/Gyungi_M10.spritefont"), E_FAIL);


	m_pLoader_Effect = Loader::Create(m_pDevice, m_pContext, LEVEL::STATIC);
	m_pLoader = Loader::Create(m_pDevice, m_pContext, LEVEL::STATIC);

	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_SquadaOne, L"../../Resources/Font/SquadaOne-Regular.spritefont"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Bold, L"../../Resources/Font/Rajdhani Bold.spritefont"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Light, L"../../Resources/Font/Rajdhani-Light.spritefont"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Medium, L"../../Resources/Font/Rajdhani-Medium.spritefont"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_SemiBold, L"../../Resources/Font/Rajdhani-SemiBold.spritefont"), E_FAIL);


	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon310, L"../../Resources/Font/Yoon310.spritefont"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon320, L"../../Resources/Font/Yoon320.spritefont"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon330, L"../../Resources/Font/Yoon330.spritefont"), E_FAIL);


	//CHECK_FAILED(m_pGameInstance->Load_Font(Font_A2Z_5_Medium, L"../../Resources/Font/A2Z5Medium.spritefont"), E_FAIL);






	/* For.Prototype_Component_Texture_BitmapFont */
	//CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"BitmapFont"),
	//    NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Font/Bitmapfont/")), E_FAIL);

	////Bitmapfont
	//CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Rajdhani_Medium, L"../../Resources/Font/BitmapFont/Rajdhani/Rajdhani.fnt", "Rajdhani/Rajdhani_0"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Rajdhani_LightGray, L"../../Resources/Font/BitmapFont/Rajdhani_LightGrayOutline/Rajdhani_LightGrayOutline.fnt", "Rajdhani_LightGrayOutline/Rajdhani_LightGrayOutline"), E_FAIL);
	//CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Squada_One, L"../../Resources/Font/BitmapFont/Squada One/Squada One.fnt", "Squada One/Squada One"), E_FAIL);

	m_pMouse = Engine::Mouse::GetInstance();

	return S_OK;
}

HRESULT Client::MainApp::Ready_IMGUI()
{
	m_pImguiManager = ImguiManager::GetInstance();
	m_pImguiManager->Init(g_hWnd, m_pDevice, m_pContext);

	m_pImguiManager->RegisterWindow(IMGUI_Main::Create(m_pDevice, m_pContext));
	m_pImguiManager->RegisterWindow(IMGUI_GameObject::Create(m_pDevice, m_pContext));

	//ImguiManager에게 추가한다.
	m_pImguiManager->RegisterWindow(ImguiRTV::Create(m_pDevice, m_pContext));
	m_pImguiManager->RegisterWindow(IMGUI_TransitionTable::Create(m_pDevice, m_pContext));
	m_pImguiManager->RegisterWindow(IMGUI_MonsterEditor::Create(m_pDevice, m_pContext));


	
	return S_OK;
}

HRESULT Client::MainApp::Ready_Light()
{
	LIGHT_DESC desc{};

	desc.eType = LIGHT::DIRECTIONAL;
	desc.vDiffuse = _float4{ 1.f, 1.f, 1.f, 1.f };
	desc.vAmbient = _float4{ 0.5f, 0.5f, 0.5f, 1.f };
	desc.vSpecular = _float4{ 0.8f, 0.85f, 0.9f, 1.f };
	desc.vDirection = _float4(0.f, -0.6f, 1.f, 0.f);
	//임시라서 그냥 자체 생산?
	m_pGameInstance->Add_Light(0, desc);

	desc = {};
	desc.eType = LIGHT::SPOTLIGHT;
	desc.fRange = 100.f;
	desc.vPosition = _float4(-279.969f, -6.72f, 66.114f, 1.f);
	desc.vDiffuse = _float4(20.f, 18.f, 16.f, 1.f);
	desc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	desc.vDirection = _float4(0.18f, -1.f, 0.06f, 1.f);
	desc.fInnerCone = cos(XMConvertToRadians(5.5f));
	desc.fOuterCone = cos(XMConvertToRadians(25.5f));
	m_pGameInstance->Add_Light(1, desc);

	desc.eType = LIGHT::SPOTLIGHT; //2번은 카메라에 붙이고 directional 계속 계산해주기
	desc.fRange = 10.f;
	desc.vPosition = _float4(-279.969f, -6.72f, 66.114f, 1.f);
	desc.vDiffuse = _float4(5.f, 5.f, 5.f, 1.f);
	desc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	desc.vDirection = _float4(0.18f, -1.f, 0.06f, 1.f);
	desc.fInnerCone = cos(XMConvertToRadians(5.f));
	desc.fOuterCone = cos(XMConvertToRadians(26.f));
	m_pGameInstance->Add_Light(2, desc);

	return S_OK;
}

HRESULT Client::MainApp::Ready_Persistent()
{
	m_pGameInstance->Set_UIManager_Active(true);

	//모든 씬에서사용할  persistent UI등록
	LOADEVENT event;
	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_bPersistent = true;
	event.m_Path = "../../DataFiles/Level_UIDev/Loading_Set.json";

	m_pGameInstance->Publish(event);

	//게임에서 쓸 persistent는 언제로드할까...
	event.m_Path = "../../DataFiles/Level_UIDev/LockOn.json";
	m_pGameInstance->Publish(event);



	return S_OK;
}

HRESULT Client::MainApp::Start_Level()
{
	//페이드인 ui 활성화이벤트
	UIObj_FadeScreen::FadeScreenEvent Event;
	Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
	Event.m_iNextLevel = LEVEL::LOGO;
	Event.m_bForceLoad = true;
	m_pGameInstance->Publish(Event);

	// m_pParsing_UI->UnSubcribe_Evnets();//다음 씬에서도사용하므로 이벤트 구독해제 -> 바로삭제

	m_pGameInstance->Add_Level(CAST(_uint)(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::LOGO));
	m_pGameInstance->Change_Level(CAST(_uint)(LEVEL::LOADING));


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
void Client::MainApp::Update(const _float fTimeDelta)
{
	static _bool open = true;

	//Show_FPS(fTimeDelta);
#ifdef _DEBUG

	// 렌더타겟, 디버그 콜라이더 켜고 끄기
	if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_F10))
	{
		g_bDrawDebugCollider = !g_bDrawDebugCollider;
	}
	else if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_F11))
	{
		m_pGameInstance->Toggle_DebugVisualization();
	}
	else if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_1))
	{
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(1.f);
	}
	else if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_2))
	{
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(2.f);
	}
	else if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_3))
	{
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(3.f);
	}
	else if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_4))
	{
		m_pGameInstance->Get_Clock(L"Clock_Default")->Set_TimeScale(4.f);
	}
	else if (m_pGameInstance->KeyDown(DIK_F10))
	{
		g_bDrawRTV = !g_bDrawRTV;
	}


#endif
	// IMGUI창 켜고 끄기
	if (m_pGameInstance->KeyDown(DIK_F1))
	{
		open = !open;
		m_pImguiManager->Set_Open_All_Window(open);
	}

	//if (m_pGameInstance->KeyDown(DIK_F2))
	//{
	//    CameraEvent shakeevent;
	//    shakeevent.eCameraAction = CAMERA_ACTION::SHAKE;
	//    shakeevent.tShake.bShakeStart = true;
	//    shakeevent.tShake.fIntensity = 0.12f;
	//    shakeevent.tShake.fDuration = 0.1f;
	//    shakeevent.tShake.fKickStrength = 0.0f;
	//    shakeevent.tShake.vKickDirection = _float4(0.f, 0.f, 0.f, 0.f);
	//    shakeevent.tShake.ePriority = SHAKE_PRIORITY::NORMAL_ATTACK;
	//
	//    m_pGameInstance->Publish(shakeevent);
	//}

	//if (m_pGameInstance->KeyDown(DIK_F3))
	//{
	//    CameraEvent shakeevent;
	//    shakeevent.eCameraAction = CAMERA_ACTION::SHAKE;
	//    shakeevent.tShake.bShakeStart = true;
	//    shakeevent.tShake.fIntensity = 0.4f;
	//    shakeevent.tShake.fDuration = 1.f;
	//    shakeevent.tShake.fKickStrength = 0.0f;
	//    shakeevent.tShake.vKickDirection = _float4(0.f, 0.f, 0.f, 0.f);
	//    shakeevent.tShake.ePriority = SHAKE_PRIORITY::EARTH_QUAKE;

	//    m_pGameInstance->Publish(shakeevent);
	//}

	//커서 켜고끄기
	if (m_pGameInstance->KeyDown(DIK_F9))
	{
		g_bShowCursor = !g_bShowCursor;
		ShowCursor(g_bShowCursor);
	}



	if (m_pLoader != nullptr && m_pLoader->Is_LoadComplete())
		Safe_Release(m_pLoader);

	if (m_pLoader_Effect != nullptr && m_pLoader_Effect->Is_LoadComplete())
		Safe_Release(m_pLoader_Effect);

	/*인벤토리매니저 업데이트(데이터)*/
	m_pInventoryManager->Update(fTimeDelta);

	/*샵 매니저 업데이트(타이머)*/
	m_pShopManager->Update(fTimeDelta);

	// 플레이어 레벨로 이동.. m_pInteractionManager->Update(fTimeDelta);

	m_pGameInstance->Update_Level(fTimeDelta);

	if (m_pCursor)
		m_pCursor->Update(fTimeDelta);

	if (m_pCursor)
		m_pCursor->Update_Late(fTimeDelta);

	/*ESC->UI관련, 카메라/플레이어 이벤트는 임시*/
	if (m_pGameInstance->KeyDown(DIK_ESCAPE) && !m_bLockEsc)
	{
		LEVEL CurrentLevel = (LEVEL)m_pGameInstance->Get_Current_LevelID();
		CHECK_TRUE(CurrentLevel == LEVEL::CUSTOMIZE);
		UI_Inventory_OnOff();
	}

	if (m_pGameInstance->KeyPress(DIK_F) && m_pGameInstance->KeyPress(DIK_LSHIFT))
	{
		CameraEvent event;
		event.eCameraState = CAMERA_STATE::CINEMATIC;
		event.bCinematicAutoReturn = true;
		event.vecKeyframes = m_vecWolfSceneKeyFrames;  // 복사만 발생, IO 없음
		m_pGameInstance->Publish(event);
	}


	// 플레이어 등록
	Find_Player(fTimeDelta);
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
void Client::MainApp::Render(const _float fTimeDelta)
{
	static _float4 clearcolor = _float4(1.f, 1.f, 1.f, 1.f);

	if (g_bIMGUI_UIOn)
	{
		m_pImguiManager->Update_Priority(fTimeDelta);
		m_pImguiManager->Update(fTimeDelta);
	}

	m_pGameInstance->Draw_Begin(&clearcolor);
	//오브젝트들 렌더전에 미니맵RTV 굽기

	MinimapManager::GetInstance()->Render_To_Texture(fTimeDelta);

	//플레이어 쉐이더 정보 한번에 던지기
	Bind_Player_Shader();

	m_pGameInstance->Draw(fTimeDelta);

	if (g_bIMGUI_UIOn)
	{
		m_pImguiManager->Render();
	}

	m_pGameInstance->Draw_Text_Begin();
	m_pGameInstance->Render_Texts_Stacked();
	m_pGameInstance->Draw_Text_End();

	if (m_pCursor)
		m_pCursor->Render(fTimeDelta);

	m_pMinimapManager->Clear_Icons_On_UI();
	m_pGameInstance->Draw_End();

}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// FPS출력 함수 ////////////////////////////////////////////////////////
void Client::MainApp::Show_FPS(const _float fTimeDelta)
{
	++m_iCurrentFPS;
	static _float Timer = {};
	Timer += fTimeDelta;

	if (Timer >= 1.f)
	{
		m_iFPS = m_iCurrentFPS;

		m_wstrFPS = L"FPS : " + to_wstring(m_iFPS);

		m_iCurrentFPS = 0;

		Timer -= 1.f;
	}

	m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::NORMAL}, .vPosition{100.f, 0.f}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{Font_Default} });

#ifdef _DEBUG
	m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{50.f, 50.f}, .vSize{1.5f, 1.5f}, .wstrDrawText{L"F9:커서토글/Alt+f4종료/ESC:인벤토리"}, .wstrFontName{Font_M10} });
#endif // _DEBUG



	// m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{Font_Default} });

	 //FONT_DESC desc;
	 //desc.wstrDrawText = m_wstrFPS;
	 //desc.wstrFontName = Font_Default;
	 //desc.vOutlineColor = { 1.f, 0.f, 1.f, 1.f };
	 //m_pGameInstance->Add_Text_Stack(desc);

	 //desc.vPosition = _float2(0.f, 50.f);
	 //desc.eSetting = TEXTDRAW_SETTING::DIR4;
	 //m_pGameInstance->Add_Text_Stack(desc);

	 //desc.vPosition = _float2(0.f, 100.f);
	 //desc.eSetting = TEXTDRAW_SETTING::DIR8;
	 //m_pGameInstance->Add_Text_Stack(desc);

}

HRESULT Client::MainApp::Register_HotKey()
{
	//m_pGameInstance->Register_HotKey(DIK_C, true, true, true, []()
	//    {
	//        MSG_BOX("DIKC + ctrl+shift+Alt");
	//    });
	return S_OK;
}
/******************************************************* FPS출력 함수 *******************************************************/



//////////////////////////////////////////////////////// UI 켜고 끄기 ////////////////////////////////////////////////////////
void Client::MainApp::UI_Inventory_OnOff()
{
	// UI매니저 비활성화 밴
	if (!m_pGameInstance->Get_UIManager_Active())
		return;

	// 뭐라도 켜져있으면 
	if (m_pGameInstance->Get_CurrentWindow())
	{

		// 닫아
		m_pGameInstance->Close_Window();
		PlayCancleSound();


		m_pInteractionManager->Finishi_Interaction();

		// 윈도우스택이 머지 아무튼 비어있지 않으면(채워져 있으면) 리턴
		if (!m_pGameInstance->Is_Empty_WindowStack())
		{
			return;
		}

		INPUT_LOCK_EVENT Event;
		Event.bLock = false;
		m_pGameInstance->Publish(Event);

		// 플레이어블 레벨일때
		if (Is_Playable_Level(m_pGameInstance->Get_Current_LevelID()))
		{
			// 캐릭터가 있으면 회전 가능
			GameObject* pObj = m_pGameInstance->Get_Player();
			if (pObj)
				static_cast<Player*>(pObj)->Set_CanTurn(true);
		}

		MOUSELOCK_EVENT mouseEvent;
		mouseEvent.bLock = true;
		m_pGameInstance->Publish(mouseEvent);


		if (m_pGameInstance->Get_Current_LevelID() != _UINT(LEVEL::BASE))
			m_pGameInstance->Change_UIMode(UI_MODE::DEFAULT);

		else
			m_pGameInstance->Change_UIMode(UI_MODE::BASE);

	}
	else
	{
		// 게임 레벨이었다면
		if (Is_Playable_Level(m_pGameInstance->Get_Current_LevelID()))
		{
			// 오브젝트 회전 불가능하게 해
			GameObject* pObj = m_pGameInstance->Get_Player();
			if (pObj)
				static_cast<Player*>(pObj)->Set_CanTurn(false);
		}

		//아무것도없을땐 확정적으로 윈도우열기
		UI_MasterEvent Event;
		Event.m_ActionName = "OpenWindow";
		Event.m_Text = L"Window_PlayerMenu";
		Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
		Event.m_bPersistent = true;
		Event.m_ActionStrHash = hash<string>{}(Event.m_ActionName);
		m_pGameInstance->Publish(Event);

		PlayOpenSound();

		////커서보이기
		m_pCursor->Set_Active(true);

		//UIManager->Mode변경(나머지 활성화비활성화 처리)
		m_pGameInstance->Change_UIMode(UI_MODE::INVENTORY);

	}
}
/******************************************************* UI 켜고 끄기 *******************************************************/



//////////////////////////////////////////////////////// 플레이어 쉐이더한번에 던지기 함수 ////////////////////////////////////////////////////////
void Client::MainApp::Bind_Player_Shader()
{
	if (m_pPlayerPart == nullptr)
		return;

	// 무성이형 구조체는 헝가리안 표기법으로 t야
	// 멤버변수에 m_ 붙는거야 체크 꼼꼼히해주셈
	// PLAYER_SHADER_DESC m_vPlayerShaderDesc = m_pPlayer->Get_PlayerShaderDesc();
	PLAYER_SHADER_DESC& playerShaderDesc = m_pPlayer->Get_PlayerShaderDesc();
	
	m_pShaderCom->Bind_EntireBuffer_BySlot(3, &playerShaderDesc, sizeof(PLAYER_SHADER_DESC));

}
void Client::MainApp::Find_Player(const _float fTimeDelta)
{
	// 파트 있으면 그냥 리턴
	if (m_pPlayerPart)
		return;

	// 플레이어 없을때 작동
	if (m_pPlayer == nullptr)
	{
		static _float timer = 0.f;
		timer += fTimeDelta;
		// 10초에 한 번씩 검색
		if (timer >= 10.f)
		{
			timer = 0.f;
			m_pPlayer = DCAST(Player*)(m_pGameInstance->Get_Player());
			if (m_pPlayer == nullptr) return;
		}
		return;
	}
	// 플레이어 없으면 밑으로 안 감
	

	// 파트오브젝트 없으면 넣어
	if (m_pPlayerPart == nullptr)
	{
		m_pPlayerPart = m_pPlayer->Find_PartObject(TEXT("Part_Body"));
		if (m_pPlayerPart == nullptr) return;
		m_pShaderCom = m_pPlayerPart->Get_Shader();
	}
}
/******************************************************* 플레이어 쉐이더한번에 던지기 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MainApp* Client::MainApp::Create()
{
	MainApp* pInstance = new MainApp;

	MSG_FAIL(pInstance->Initialize(), L"MainApp 생성에 실패했습니다!", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MainApp::Free()
{
	__super::Free();

	m_pGameInstance->UnsubScribe(LockHandle);

	m_pGameInstance->Delete_Light(0);
	PoolingManager::Get_Instance()->Clear_All();
	Safe_Release(m_pCursor);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pParsing_UI);
	Safe_Release(m_pLoader);
	Safe_Release(m_pLoader_Effect);
	Safe_Release(m_pMouse);

	InventoryManager::GetInstance()->DestroyInstance();
	ItemManager::GetInstance()->DestroyInstance();
	MinimapManager::GetInstance()->DestroyInstance();
	DialogueManager::GetInstance()->DestroyInstance();
	SkillManager::GetInstance()->DestroyInstance();


	m_pImguiManager->DestroyInstance();

	CoUninitialize();

	_uint refcnt = m_pGameInstance->Release_Engine();
	InteractionManager::GetInstance()->DestroyInstance();
	PoolingManager::DestroyInstance();
	ShopManager::DestroyInstance();

	COUT("[레퍼런스] : " << refcnt);
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

