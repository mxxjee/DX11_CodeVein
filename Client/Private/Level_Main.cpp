#include "Client_Define.h"
#include "Level_Main.h"


#include "GameInstance.h"
#include "PoolingManager.h"
#include "Parser_UITool.h"
#include "Level_UIDev.h"
#include "UIObj_GuardBar.h"
#include "UIObj_Stamina.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_Cursor.h"
#include "UIObj_HpBar.h"

#include "InventoryManager.h"
#include "Layer.h"

#pragma region Effect

#include "ParticleSystem.h"
#include "VFX_Parsing.h"

#pragma endregion

#include "Player.h"
#include "Player_Stat.h"
#include "Yakumo.h"
#include "Cinematic_EventShape.h"

#pragma region PhysXTest
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_MasterRig.h"

#include "Camera_Object.h"
#include "Camera_Free.h"
#include "Mesh.h"
#include "MT_Utils.h"
#include "ItemManager.h"
#include "MinimapManager.h"
#include "InteractionManager.h"

#include "Monster.h"
#include "CustomizingManager.h"
#pragma endregion

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Main::Level_Main()
{
}

Client::Level_Main::Level_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level_Client(pDevice, pContext)
{
}

Client::Level_Main::~Level_Main()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Main::Initialize(LEVEL _level)
{
	CustomizingManager::GetInstance()->DestroyInstance();

	m_LevelTitle = L"AREA D-12  Ruined City UnderGround";
	__super::Initialize(_level);

	m_iLevel = _UINT(_level);
	m_pInventoryManager = InventoryManager::GetInstance();


	m_pGameInstance->Build_OcTree(_float3{-156.154f, 8.90f, 99.1936 }, 300.f, 4);

	ItemManager::GetInstance()->Initialize(m_pDevice, m_pContext);
	//	MinimapManager::GetInstance()->Initialize(m_pDevice, m_pContext, (_uint)WINCX, (_uint)WINCY);
	m_pPoolingManager = PoolingManager::Get_Instance();
	Safe_AddRef(m_pPoolingManager);

	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Camera(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);
	CHECK_FAILED(Ready_UIObject(), E_FAIL);
	CHECK_FAILED(Ready_MainEffects(), E_FAIL);

	CHECK_FAILED(Ready_Map(), E_FAIL);
	CHECK_FAILED(Ready_Sky(), E_FAIL);
	CHECK_FAILED(Ready_Collider(), E_FAIL);
	CHECK_FAILED(Ready_Monster(), E_FAIL);
	CHECK_FAILED(Ready_Items(), E_FAIL);
	CHECK_FAILED(Ready_Values(), E_FAIL);
	CHECK_FAILED(Ready_GodRay(), E_FAIL);

	/////////>>>>슬라임씨를 위한 맵로딩 끝 판단 추가<<<<<<<
	SYSTEM_EVENT Event;
	Event.eType = SYSTEM_EVENT_TYPE::END_LOAD;
	m_pGameInstance->Publish(Event);


	/*마우스 기본값 = lock*/
	MOUSELOCK_EVENT mouseEvent;
	mouseEvent.bLock = true;
	m_pGameInstance->Publish(mouseEvent);


	//커서끄기
	UIObj_Cursor::CursorEvent cursorEvent;
	cursorEvent.bEnable = false;
	m_pGameInstance->Publish(cursorEvent);


	

	InventoryManager::GetInstance()->Set_Default_Inventory();
	InventoryManager::GetInstance()->Set_Haze(0);
	InventoryManager::GetInstance()->Publish_Haze();


	m_pGameInstance->Close_All_Menu();
	m_pGameInstance->Change_UIMode(UI_MODE::DEFAULT);
	InteractionManager::GetInstance()->Set_Player(m_pGameInstance->Get_Player());
	MinimapManager::GetInstance()->Set_Current_Level(LEVEL::MAIN);
	
	//다시플레이어입력활성화!!
	GameObject* pObj = m_pGameInstance->Get_Player();
	if (pObj)
		static_cast<Player*>(pObj)->Set_CanTurn(true);


	INPUT_LOCK_EVENT InputEvent;
	InputEvent.bLock = false;
	m_pGameInstance->Publish(InputEvent);


	//잠시 주석좀 .. 
	InteractionManager::GetInstance()->Set_LastSavePointInfo(LEVEL::MAIN, 1);
	Teleport_With_SavePoint();//세이브포인트를 통해 타고들어왔는지 확인하고, 위치설정한다.
	m_pGameInstance->Play_Sound("UnderPassAmb", 0.f, true);

#pragma region Player_Sound
	m_pGameInstance->LoadSound_AddGroup("Foot_Player (633253298)", "Foot_Player", L"../../Resources/Sounds/Player/Foot_Player/Foot_Player (633253298).wav");
	m_pGameInstance->LoadSound_AddGroup("Foot_Player (726181377)", "Foot_Player", L"../../Resources/Sounds/Player/Foot_Player/Foot_Player (726181377).wav");
	m_pGameInstance->LoadSound_AddGroup("Foot_Player (791771586)", "Foot_Player", L"../../Resources/Sounds/Player/Foot_Player/Foot_Player (791771586).wav");

#pragma endregion

	ChangeColor();

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Main::Update_Priority(const _float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(DIK_F7))
	{
		if (m_bToggleCamera)
		{
			m_bToggleCamera = false;
			g_bFreeCam = false;
			m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_01));
			m_pGameInstance->Set_Visible_All_UI(true);
		}
		else
		{
			m_bToggleCamera = true;
			g_bFreeCam = true;
			m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));
			
			m_pGameInstance->Set_Visible_All_UI(false);
		}

	}

	m_pGameInstance->Update_Priority(fTimeDelta);

	//Test_PlayerStatus(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{50.f, 100.f}, .vSize{1.5f, 1.5f}, .wstrDrawText{L"shift+w:스테미나사용/클릭:hp사용/Space:Hit게이지"}, .wstrFontName{Font_M10} });
	m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{50.f, 120.f}, .vSize{1.5f, 1.5f}, .wstrDrawText{L"NumPad1~9 : 아이템획득(인벤토리확인하기)"}, .wstrFontName{Font_M10} });
	m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{50.f, 120.f}, .vSize{1.5f, 1.5f}, .wstrDrawText{L"사다리 앞에서 ㄹ입력시 등반"}, .wstrFontName{Font_M10} });
#endif // _DEBUG

	Change_ColorSurface();
	Change_ColorCave();
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Main::Update(const _float fTimeDelta)
{
	
	m_pGameInstance->Update_Parallel(fTimeDelta);

	//// 피직스 씬 업데이트
	//if (m_pPxScene)
	//{
	//	m_pPxScene->simulate(fTimeDelta);
	//	m_pPxScene->fetchResults(true);
	//}
	m_pGameInstance->Update_PhysX(fTimeDelta);

	m_LevelEnterAlarm.Update(fTimeDelta);

	m_pGameInstance->Update(fTimeDelta);
	m_PlayerStatus.Update(fTimeDelta);
	


	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Main::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

#ifdef _DEBUG
	//Test_Inventory(fTimeDelta);
//Test_SavePoint(fTimeDelta);

//if (m_pGameInstance->KeyDown(DIK_P))
//{
//	int iNewValue = (rand() % 5 + 1) * 1000;
//	InventoryManager::GetInstance()->Add_Haze(iNewValue);
//}

//if (m_pGameInstance->KeyDown(DIK_O))
//{
//	int iNewValue = (rand() % 5 + 1) * 1000;
//	InventoryManager::GetInstance()->Reduce_Haze(iNewValue);
//}
#endif // _DEBUG


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Main::Render(const _float fTimeDelta)
{
	if (m_bMapCreate)
	{
		m_pGameInstance->Set_Capture(true);
		m_bMapCreate = false;
	}
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/

HRESULT Client::Level_Main::Ready_Player()
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
	if (pPlayer)
	{
		// ??
		//XMVectorSet(-271.426f, -21.52375f, 95.8762f, 1.f);
		
		//스폰포인트 -214.975    Y : -19.6577    Z : 25.9624
		_vector vPosition = XMVectorSet(-214.975f, -17.2085f, 25.9624f, 1.f);
		

		//시호 사다리용
		//_vector vPosition = XMVectorSet(-278.f, -27.2f, 59.5f, 1.f);

		//지상_지하도시
		//_vector vPosition = XMVectorSet(-165.f, 10.f, 100.f, 1.f);

		pPlayer->Set_Position_ByController(vPosition);
		pPlayer->Set_Active(true);
		pPlayer->Enter_MainScene();

		//YAKUMO_ACTIVE_EVENT YakumoEvnet = {};
		//YakumoEvnet.bSuccess = true;
		//m_pGameInstance->Publish(YakumoEvnet);
	}
	else
	{
		return S_OK;
	}


	return S_OK;
}

HRESULT Client::Level_Main::Ready_Camera()
{
	CCamera_Object* pFollowCam = nullptr;
	Camera* playerCam = nullptr;

	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	m_pGameInstance->Add_Camera(m_iLevel, L"Prototype_GameObject_Camera_Object", m_iLevel, _UINT(CAMERA::PLAYER_FOCUS_00), RCAST(GameObject**)(&pFollowCam));		// Player 포커싱 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));							// 자유시점 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Player, m_iLevel, _UINT(CAMERA::PLAYER_FOCUS_01), RCAST(GameObject**)(&playerCam));							// 플레이어 카메라
	m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_01));


	if (pFollowCam)
	{
		//GameObject* pPlayer = m_pGameInstance->Get_GameObject(m_iLevel, L"Layer_Player", L"Player_0");
		//pFollowCam->Set_Target(pPlayer);

		////미니맵매니저에도 연결해주기
		//MinimapManager::GetInstance()->Set_RotateCamera(pFollowCam);
	}
	if (playerCam)
	{
		//미니맵매니저에도 연결해주기
		MinimapManager::GetInstance()->Set_RotateCamera(playerCam);
	}

	return S_OK;
}

HRESULT Client::Level_Main::Ready_Monster()
{
	//Monster::MONSTER_DESC Desc{};
	//PHYSX_CONTROLLER_DESC ControllerDesc; 

	//ControllerDesc.fRadius = 0.5f;
	//ControllerDesc.fHeight = 1.0f;
	//ControllerDesc.pOwner = nullptr;
	//ControllerDesc.eActorType = PX_ACTOR_TYPE::MONSTER;
	//ControllerDesc.fSlopeLimit = 0.2f;
	//ControllerDesc.fStepOffset = 0.5f;

	// ====================================================================================================================
	//	1구역: 지하동굴
	// ====================================================================================================================

	//Desc.vPosition = _float4(-258.25f, -21.63f, 20.84f, 1.f);
	//Desc.fSpeed = 2.0f;
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, L"Layer_Monster", nullptr, &Desc);

	//Desc.vPosition = _float4(-266.97f, -24.62f, 32.34f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, L"Layer_Monster", nullptr, &Desc);

	//Desc.vPosition = _float4(-268.72f, -26.98f, 30.88f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Devil"), m_iLevel, L"Layer_Monster", nullptr, &Desc);
	

	 //====================================================================================================================
		//1.5구역: 복도
	 ///====================================================================================================================

	//Desc.vPosition = _float4(-245.425f, -21.105f, 56.683f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"St01_Slime"), m_iLevel, L"Layer_Monster", nullptr, &Desc);
	//	



	//Desc.vPosition = _float4(-246.13f, -19.71f, 54.22f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, L"Layer_Monster", nullptr, &Desc);



	// ====================================================================================================================
	//	2구역: 주차장
	 //====================================================================================================================

	//Desc.vPosition = _float4(-222.42f, -4.59f, 46.21f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, L"Layer_Monster", nullptr, &Desc);

	//Desc.vPosition = _float4(-225.19f, -6.44f, 47.43f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Devil"), m_iLevel, L"Layer_Monster", nullptr, &Desc);

	//Desc.vPosition = _float4(-245.425f, -21.105f, 56.683f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"St01_Slime"), m_iLevel, L"Layer_Monster", nullptr, &Desc);

	//Desc.vPosition = _float4(-225.25f, -7.27f, 77.48f, 1.f);
	//ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	//Desc.tControllerDesc = ControllerDesc;
	//m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, L"Layer_Monster", nullptr, &Desc);

	////잠만올리버좀끌게요..
	//Monster::MONSTER_DESC desc{};
	//desc.vPosition = _float4(-158.68f, 9.f + 1.0f, 102.f, 1.f);
	//desc.fSpeed = 8.f;
	//desc.fRotationSpeed = XMConvertToRadians(90.f);

	//ControllerDesc.vPosition = { desc.vPosition.x, desc.vPosition.y, desc.vPosition.z };
	//ControllerDesc.pOwner = nullptr;

	//desc.tControllerDesc = ControllerDesc;

	//MSG_FAIL(FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"St01_BossOliver"), m_iLevel, L"Layer_Monster", nullptr, &desc)), L"몬스터 생성 실패", L"실패", E_FAIL);


	m_pPoolingManager->Register(POOL_ID::MONSTER_OLIVER, 1, m_iLevel, Proto_GameObject(L"St01_BossOliver"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_SLAVE_VAMPIRE, 3, m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_SLAVE_DEVIL, 1, m_iLevel, Proto_GameObject(L"Slave_Devil"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_SLIME, 5, m_iLevel, Proto_GameObject(L"St01_Slime"), m_iLevel, Layer_Monsters);

	return S_OK;
}

HRESULT Client::Level_Main::Ready_Light()
{
	//LIGHT_DESC* desc = m_pGameInstance->Get_LightDesc(0);
	//desc->vDirection = { 1.f, -0.7f, 0.15f, 0.f };
	//m_pGameInstance->Set_LightDesc(0, *desc);
	//Desc.eType = LIGHT::POINT;
	//Desc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	//Desc.fRange = 10.f;
	//Desc.vDiffuse = _float4(1.f, 0.3f, 0.3f, 1.f);
	//Desc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
	//Desc.vSpecular = _float4{ 1.f, 1.f, 1.f, 1.f };
	LIGHT_DESC* desc = m_pGameInstance->Get_LightDesc(0);
	desc->vDirection = { 0.f, -0.6f, 1.f, 0.f };
	m_pGameInstance->Set_LightDesc(0, *desc);
	//m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), Desc);

	//Desc.eType = LIGHT::POINT;
	//Desc.vPosition = _float4(10.f, 5.f, 10.f, 1.f);
	//Desc.fRange = 10.f;
	//Desc.vDiffuse = _float4(1.f, 0.3f, 0.3f, 1.f);
	//Desc.vAmbient = _float4(0.3f, 0.1f, 0.1f, 1.f);
	//Desc.vSpecular = Desc.vDiffuse;

	//m_pGameInstance->Add_Light(2, Desc);
	//GameObject* temp = m_pGameInstance->Get_GameObject(_UINT(LEVEL::LOGO), L"Layer_GameObject", L"Monster");

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

	const _int NumCascade = 4;
	_float bias[NumCascade] = { 0.001f, 0.002f, 0.003f, 0.005f };
	if (FAILED(m_pGameInstance->Add_ShadowCascadeLight(NumCascade, ShadowDesc1, bias)))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::Level_Main::Ready_Collider()
{
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::INTERACTION);
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::EVENT_POINT);
	CAST(Player*)(m_pGameInstance->Get_Player())->Register_Collider_Player();	// 시간 없어서 하드코딩 편법좀 썼습니ㅏㄷ

	Cinematic_EventShape::CINEMATICEVENTSHAPE_DESC desc;
	desc.fRadius = 2.f;
	desc.vPosition = _float4(-203.054f, 1.543f, 101.296f, 1.f);

	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Collider_Cinematic"), m_iLevel, L"Layer_Collider", nullptr, &desc);

	return S_OK;
}

HRESULT Client::Level_Main::Ready_Map()
{
	COUT("메인 맵 세팅중");

	_string strMapFolder = "../../DataFiles/Level_Main";
	_string strFileName = "MapData";

	m_pGameInstance->Spawn_LevelData(strMapFolder, m_iLevel, true);

	Spawn_SavePoint(LEVEL::MAIN);

	if (FAILED(Spawn_MonsterTrigger(LEVEL::MAIN, strMapFolder, strFileName)))
		return E_FAIL;

	m_bMapCreate = true;
	COUT("맵 세팅 완료");

	return S_OK;
}

HRESULT Client::Level_Main::Ready_MainEffects()
{
	POOLING->Register_Effect(POOL_ID::EFFECT_FIRE, m_pGameInstance->Find_Effect(L"Drum_Bonfire"), 25,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	return S_OK;
}

HRESULT Client::Level_Main::Ready_GodRay()
{
	_wstring layer = Layer_GameObject;

	GameObject::GAMEOBJECT_DESC Desc;

	Desc.vScale = _float3{ 7.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -281.6f, -0.8f, 64.5f, 1.f };
	Desc.wstrName = L"GodRayMesh";

	Desc.vRotation = _float3{ XMConvertToRadians(-6.5f), XMConvertToRadians(0.f), XMConvertToRadians(11.5f) };
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-13.f), XMConvertToRadians(60.f), XMConvertToRadians(0.5f) };
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-6.5f), XMConvertToRadians(120.f), XMConvertToRadians(-11.f) };
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	return S_OK;
}

//////////////////////////////////////////////////////// UI 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Main::Ready_UIObject()
{
	__super::Ready_UIObject();

	//여기 씬에서만사용하는 UI 로드 /BossNameUI
	LOADEVENT event;
	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/BossName.json";

	m_pGameInstance->Publish(event);



	//BossHpBar
	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/Hpbar_Boss.json";

	m_pGameInstance->Publish(event);

	//BossDeadUI
	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/Boss_End.json";
	m_pGameInstance->Publish(event);


	return S_OK;

}
/******************************************************* UI 준비 함수 *******************************************************/

HRESULT Client::Level_Main::Ready_Values()
{

	Player_Stat* pPlayerStat = dynamic_cast<Player_Stat*>(m_pGameInstance->Get_Player()->Get_Component_FromName(Com_Stat));

	if (pPlayerStat == nullptr)
		return E_FAIL;
	//명혈 ui갱신
	ManaUIEvent Event;
	Event.eType = ManaUIEventType::INTIALIZE;
	Event.fMaxValue = (_float)pPlayerStat->Get_MaxMyeonghyeol();
	Event.fValue = (_float)pPlayerStat->Get_Myeonghyeol();
	m_pGameInstance->Publish(Event);


	//가드바 값연결
	UIObject* pGuardBar = m_pGameInstance->Find_UI_ByName(L"GuardBar");
	if (pGuardBar)
	{
		UIObj_GuardBar* ppGuardBar = dynamic_cast<UIObj_GuardBar*>(pGuardBar);
		if (ppGuardBar)
		{
			ppGuardBar->Set_OwnerObjectID(m_pGameInstance->Get_Player()->Get_ObjectID());
			ppGuardBar->Set_TargetValue(pPlayerStat->Get_FocusGaugePtr());
			ppGuardBar->Set_MaxValue(pPlayerStat->Get_MaxFocusGaugePtr());

		}
	}


	//스테미너 값연결
	UIObject* pStaminaBar = m_pGameInstance->Find_UI_ByName(L"Stamina");
	if (pStaminaBar)
	{
		UIObj_Stamina* ppStamina = dynamic_cast<UIObj_Stamina*>(pStaminaBar);
		if (ppStamina)
		{
			//요기 나중에 const로 바꿔줘야할거같기도
			ppStamina->Set_TargetValue(pPlayerStat->Get_StaminaPtr());
			ppStamina->Set_MaxValue(pPlayerStat->Get_MaxStaminaPtr());

		}
	}



	//HP바연결
	UIObject* pHPBar = m_pGameInstance->Find_PersistentUI_ByName(L"HPBar");
	if (pHPBar)
	{
		UIObj_ProgressBar* ppHpBar = dynamic_cast<UIObj_ProgressBar*>(pHPBar);
		if (ppHpBar)
		{
			ppHpBar->Set_CurrentFloat(pPlayerStat->Get_HpPtr());
			ppHpBar->Set_MaxFloat(pPlayerStat->Get_MaxHpPtr());

			ppHpBar->Init_Ratio(true, 1.f);
			ppHpBar->Init_Ratio(false, 1.f);

		}
	}

	//Text연결
	UIObject* pHPText = m_pGameInstance->Find_PersistentUI_ByName(L"CurrentHP_Text");
	if (pHPText)
	{
		UIObj_BitmapFont* ppHpText = dynamic_cast<UIObj_BitmapFont*>(pHPText);
		if (ppHpText)
		{
			ppHpText->Set_ValueFloat(pPlayerStat->Get_HpPtr());

		}
	}

	UIObject* pHPTotalText = m_pGameInstance->Find_PersistentUI_ByName(L"TotalHP_Text");
	if (pHPTotalText)
	{
		UIObj_BitmapFont* ppTotalHpText = dynamic_cast<UIObj_BitmapFont*>(pHPTotalText);
		if (ppTotalHpText)
		{
			ppTotalHpText->Set_ValueFloat(pPlayerStat->Get_MaxHpPtr());

		}
	}


	UIObject* pBossHpBar = m_pGameInstance->Find_UI_ByName(L"Boss_HPBar");
	if (pBossHpBar)
	{
		pBossHpBar->Set_Active(false);
	}

	Player_Stat* pYakumoStat = dynamic_cast<Player_Stat*>(m_pGameInstance->Get_Companion()->Get_Component_FromName(Com_Stat));

	/*동행자 ui연결방법*/
	UIObject* pObj = m_pGameInstance->Find_UI_ByName(L"Buddy_Statusbar");
	if (pObj)
	{
		UIObj_HpBar* pHpbar = dynamic_cast<UIObj_HpBar*>(pObj->Get_Child(L"HPBar"));
		if (pHpbar)
		{


			pHpbar->Set_Owner_ObjectID(m_pGameInstance->Get_Companion()->Get_ObjectID());
			pHpbar->Set_CurrentFloat(pYakumoStat->Get_HpPtr());
			pHpbar->Set_MaxFloat(pYakumoStat->Get_MaxHpPtr());
			pHpbar->Init_Ratio(true, (pYakumoStat->Get_Hp() / pYakumoStat->Get_MaxHp()));

			//그리고 동행자 스폰시에 SEt_active()시켜야함 (동행자는 사라질떄도 있으니ㅏㄲ 스폰할때마다 함수만들어서 이거호출해야함)
			//UI_MasterEvent Event;
			//Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
			//Event.m_bFlag = true;		//끄고싶으면 false값 넘겨주기
			//Event.m_bPersistent = true;
			//Event.m_ActionName = "ActiveUI";
			//Event.m_Text = L"Buddy_Statusbar";
			//m_pGameInstance->Publish(Event);

		}

	}



	__super::Ready_Values();


	return S_OK;
}

void Client::Level_Main::ChangeColor()
{
	m_pGameInstance->ChangeMainMap();
}

HRESULT Client::Level_Main::Ready_Items()
{
	m_pPoolingManager->Register(POOL_ID::ITEM_GROUND, 10, m_iLevel, Proto_GameObject(L"Item"), m_iLevel, Layer_Items, nullptr, 0, POOL_POLICY::GROW);
	m_pPoolingManager->Register(POOL_ID::ITEM_DROP, 5, m_iLevel, Proto_GameObject(L"Item"), m_iLevel, Layer_Items);

	if (FAILED(Spawn_Item(LEVEL::MAIN, "../../DataFiles/Level_Main", "MapData")))
		return E_FAIL;
	return S_OK;
}

void Client::Level_Main::Change_ColorSurface()
{
	_float fPosStart = -5.2f;
	_float fPosEnd = 7.0f;
	
	_float3 vPosf = m_pGameInstance->Get_PlayerPos_Float3();
	if (vPosf.y < fPosStart || vPosf.y > fPosEnd)
		return;
	
	float t = clamp((vPosf.y - fPosStart) / (fPosEnd - fPosStart), 0.f, 1.f);
	m_pGameInstance->ChangeMap(t);

	LIGHT_DESC* temp = m_pGameInstance->Get_LightDesc(2);
	_vector tempVector = XMLoadFloat4(&temp->vDiffuse);
	
	_vector FinalVector = XMVectorSet(5.f, 5.f, 5.f, 1.f) * (1.f - t) + XMVectorSet(0.f, 0.f, 0.f, 1.f) * t;
	_float4 tempFloat;
	XMStoreFloat4(&tempFloat, FinalVector);
	temp->vDiffuse = tempFloat;
	m_pGameInstance->Set_LightDesc(2, *temp);

	return;
}

void Client::Level_Main::Change_ColorCave()
{ 
	//만드는중
	_float fPosStart = 40.f;
	_float fPosEnd = 45.f;

	_float3 vPosf = m_pGameInstance->Get_PlayerPos_Float3();
	if (vPosf.y > -20)
		return;
	if (vPosf.z < fPosStart || vPosf.z > fPosEnd)
		return;

	float t = clamp((vPosf.z - fPosStart) / (fPosEnd - fPosStart), 0.f, 1.f);
	m_pGameInstance->ChangeCaveMap(t);
}





//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Main* Client::Level_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Main* pInstance = new Level_Main(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Main 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Main::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Main 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Main::Free()
{
	m_pGameInstance->StopSound("UnderPassAmb");
	m_pGameInstance->Clear_Level_Prototype(_UINT(LEVEL::MAIN));
	m_pGameInstance->Clear_Level_GameObject(_UINT(LEVEL::MAIN));
	m_pPoolingManager->Clear_Range(POOL_ID::MONSTER_STAGE01_BEGIN, POOL_ID::MONSTER_STAGE01_END);
	m_pPoolingManager->Clear_Range(POOL_ID::EFFECT_OLIVER_SLASH, POOL_ID::EFFECT_OLIVER_SCREAM);
	m_pPoolingManager->Clear_Range(POOL_ID::ITEM_BEGIN, POOL_ID::ITEM_END);
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
