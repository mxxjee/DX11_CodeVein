#include "Client_Define.h"
#include "Level_Church.h"


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
#include "UISoundUtil.h"
#include "UI_WorldComponent.h"



#pragma region Effect

#include "ParticleSystem.h"
#include "VFX_Parsing.h"

#pragma endregion

#include "Player.h"
#include "Player_Stat.h"

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
#include "Cinematic_EventShape.h"


#include "Monster.h"
#include "UIObject.h"
#include "UIObj_Ending.h"
#pragma endregion

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Church::Level_Church()
{
}

Client::Level_Church::Level_Church(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level_Client(pDevice, pContext)
{
}

Client::Level_Church::~Level_Church()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Church::Initialize(LEVEL _level)
{
	m_iLevel = _UINT(_level);
	m_pInventoryManager = InventoryManager::GetInstance();
	
	m_LevelTitle = L"AREA F-15  Catheral Of The Sacred Blood";
	__super::Initialize(_level);


	m_pGameInstance->Build_OcTree(_float3{ 0.f, 0.f, 0.f }, 300.f, 4);

	ItemManager::GetInstance()->Initialize(m_pDevice, m_pContext);
	MinimapManager::GetInstance()->Initialize(m_pDevice, m_pContext, (_uint)WINCX, (_uint)WINCY);
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

	INPUT_LOCK_EVENT InputEvent;
	InputEvent.bLock = false;
	m_pGameInstance->Publish(InputEvent);


	//다시플레이어입력활성화!!
	GameObject* pObj = m_pGameInstance->Get_Player();
	if (pObj)
		static_cast<Player*>(pObj)->Set_CanTurn(true);

	InventoryManager::GetInstance()->Set_Default_Inventory();
	m_pGameInstance->Change_UIMode(UI_MODE::DEFAULT);
	InteractionManager::GetInstance()->Set_Player(m_pGameInstance->Get_Player());
	MinimapManager::GetInstance()->Set_Current_Level(LEVEL::CHURCH);

	//InteractionManager::GetInstance()->Set_LastSavePointInfo(LEVEL::CHURCH, 1);
	Teleport_With_SavePoint();//세이브포인트를 통해 타고들어왔는지 확인하고, 위치설정한다.
	m_pGameInstance->Play_Sound("DuomoAmb", 0.f, true);

#pragma region Player_Sound
	m_pGameInstance->ClearGroup("Foot_Player");
	m_pGameInstance->LoadSound_AddGroup("Foot_Player (6733536)", "Foot_Player", L"../../Resources/Sounds/Player/Foot_Player/Foot_Player (6733536).wav");

	m_pGameInstance->Get_Companion()->Get_StatComponent()->Set_MaxHP(2200.f);

#pragma endregion

	m_pGameInstance->ChangeChurch();

#ifdef _DEBUG
	//Debug_WhiteDevil();
#endif // _DEBUG

	m_MonsterSpawnPosVec.push_back(_float4{ -187.54f, 31.19f, -62.69f, 1.f });
	m_MonsterSpawnPosVec.push_back(_float4{ -187.43f, 31.17f, -79.46f, 1.f });
	m_MonsterSpawnPosVec.push_back(_float4{ -202.31f, 30.50f, -78.68f, 1.f });
	m_MonsterSpawnPosVec.push_back(_float4{ -202.22f, 30.09f, -63.97f, 1.f });

	m_iItemTriggerHandle = m_pGameInstance->Subscribe<ITEM_TRIGGER_EVENT>([this](const ITEM_TRIGGER_EVENT& e)
		{
			if (e.eTrigger == ITEM_TRIGGER::CHURCH_RAID)
			{
				m_bWaveOnOff = true;

				UIObject* pMosterWaveUI = m_pGameInstance->Find_UI_ByName(L"Monsterwave_UI");
				if (pMosterWaveUI)
					pMosterWaveUI->Set_Active_Delay(3.f);

				m_pGameInstance->Play_Sound("Boss_Title", 0.7f, false);
			}
			else if (e.eTrigger == ITEM_TRIGGER::CHURCH_ENDING)
			{

				UIObject* pWorldUI = m_pGameInstance->Find_UI_ByName(L"Ending");
				if (pWorldUI)
				{
					UIObj_Ending* pEndingUI = dynamic_cast<UIObj_Ending*>(pWorldUI);
					if (pEndingUI)
					{
						//엔딩상자 위치(하드코딩ㅋ)
						pEndingUI->Set_InitPos(_float3(-56.f, -14.9f, -64.06f));
							
						pEndingUI->Set_Active(true);
					}
					

				}
				m_pGameInstance->Play_Sound("Boss_Title", 0.7f, false);
			}
		});

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Church::Update_Priority(const _float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(DIK_F7))
	{
		m_bToggleCamera = !m_bToggleCamera;
		if (m_bToggleCamera)
		{
			m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));
			Camera* pCam = m_pGameInstance->Get_Camera(_UINT(CAMERA::FREE));
			if (pCam)
			{
				//플레이어위치로 카메라
				GameObject* pPlayer = m_pGameInstance->Get_Player();
				if (pPlayer)
				{
					pCam->Set_State(DIRECTION::POSITION, pPlayer->Get_Position());
				}

			}
		}
		else
			m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_01));
	}

	m_pGameInstance->Update_Priority(fTimeDelta);

	//Test_PlayerStatus(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{50.f, 100.f}, .vSize{1.5f, 1.5f}, .wstrDrawText{L"shift+w:스테미나사용/클릭:hp사용/Space:Hit게이지"}, .wstrFontName{Font_M10} });
	m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{50.f, 120.f}, .vSize{1.5f, 1.5f}, .wstrDrawText{L"NumPad1~9 : 아이템획득(인벤토리확인하기)"}, .wstrFontName{Font_M10} });
	m_pGameInstance->Draw_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{50.f, 120.f}, .vSize{1.5f, 1.5f}, .wstrDrawText{L"사다리 앞에서 ㄹ입력시 등반"}, .wstrFontName{Font_M10} });
#endif // _DEBUG

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Church::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update_Parallel(fTimeDelta);

	m_pGameInstance->Update_PhysX(fTimeDelta);


	m_LevelEnterAlarm.Update(fTimeDelta);


	m_pGameInstance->Update(fTimeDelta);
	m_PlayerStatus.Update(fTimeDelta);

	ChangeChurchBoss();
	MonsterWave();
	ChangeEnd();

	if (m_pGameInstance->KeyPress(DIK_9))
	{
		UIObject* pWorldUI = m_pGameInstance->Find_UI_ByName(L"Ending");
		if (pWorldUI)
			pWorldUI->Set_Active(true);

	}
	return 0;
}

void Client::Level_Church::ChangeChurchBoss()
{
	float fPosStart = -66.f;
	float fPosEnd = -65.f;

	_float3 vPosf = m_pGameInstance->Get_PlayerPos_Float3();
	if (vPosf.x < fPosStart || vPosf.x > fPosEnd)
		return;

	GameObject* Sun = m_pGameInstance->Get_GameObject(_UINT(LEVEL::CHURCH), Layer_Sky, L"GodRaySun_0");
	Sun->Get_Transform()->Set_State(DIRECTION::POSITION, _float4{ -56.23, 27.98f, -16.22f, 1.f });
	m_pGameInstance->ChangeChurchBoss();
}

void Client::Level_Church::ChangeEnd()
{
	float fXPos = -62.42f;
	float fYPos = 5.f;

	_float3 vPosf = m_pGameInstance->Get_PlayerPos_Float3();
	if (vPosf.x < fXPos || vPosf.y > fYPos)
		return;

	m_pGameInstance->ChangeEnd();
}
void Client::Level_Church::MonsterWave()
{
#ifdef _DEBUG
	if(m_pGameInstance->KeyDown(DIK_N))
	{
		m_bWaveOnOff = !m_bWaveOnOff;
		UIObject* pMosterWaveUI = m_pGameInstance->Find_UI_ByName(L"Monsterwave_UI");
		if (pMosterWaveUI)
			pMosterWaveUI->Set_Active_Delay(3.f);

		m_pGameInstance->Play_Sound("Boss_Title", 0.7f, false);

	}
#endif
	if (!m_bWaveOnOff)
		return;

	Layer* MonsterLayer = m_pGameInstance->Get_Layer(Layer_Monsters);
	vector<GameObject*>& temp = MonsterLayer->Get_GameObjects_Vector();
	for (auto monster : temp)
	{
		static_cast<Monster*>(monster)->Set_WaveRimOn(true);
	}

	_uint EliteMonsterCnt = {};
	_uint NormalMonsterCnt = {};
	Monster::MONSTER_DESC desc{};

	EliteMonsterCnt = m_pPoolingManager->Get_ActiveCount(POOL_ID::MONSTER_GIANT_VAMPIRE) + m_pPoolingManager->Get_ActiveCount(POOL_ID::MONSTER_GHOSTKNIGHT);
	NormalMonsterCnt = m_pPoolingManager->Get_ActiveCount(POOL_ID::MONSTER_MONKEY_DEVIL) + m_pPoolingManager->Get_ActiveCount(POOL_ID::MONSTER_SLAVE_VAMPIRE);

	//몬스터 다 잡으면 꺼지게설정
	if (EliteMonsterCnt + NormalMonsterCnt == 0 && m_iMaxElite > 3)
	{
		m_bWaveOnOff = false;
		for (auto monster : temp)
		{
			static_cast<Monster*>(monster)->Set_WaveRimOn(false);
		}

		//끝 UI
		UIObject* pMosterWaveEndUI = m_pGameInstance->Find_UI_ByName(L"MonsterWaveEnd_UI");
			if (pMosterWaveEndUI)
				pMosterWaveEndUI->Set_Active_Delay(3.f);
			m_pGameInstance->Play_Sound("Tagui_Disappear", UIVolume, false);
		return;
	}
	//돼지3번 잡으면 돼지 + 엘리트
	if (EliteMonsterCnt == 0) //1돼지
	{
		if (m_iMaxElite > 3)
			return;
		_float RandomPos = m_pGameInstance->RandomValue_int(0, 3);
		desc.vPosition = m_MonsterSpawnPosVec[RandomPos];
		if(m_iMaxElite == 0) //1페 돼지
			m_pPoolingManager->Acquire(POOL_ID::MONSTER_GIANT_VAMPIRE, &desc);
		else if (m_iMaxElite == 1) //2페 할버드
			m_pPoolingManager->Acquire(POOL_ID::MONSTER_GHOSTKNIGHT, &desc);
		else if (m_iMaxElite == 2)
		{
			for (int i = 0; i < 2; i++)
			{
				RandomPos = m_pGameInstance->RandomValue_int(0, 3);
				desc.vPosition = m_MonsterSpawnPosVec[RandomPos];
				if(i == 0)
					m_pPoolingManager->Acquire(POOL_ID::MONSTER_GHOSTKNIGHT, &desc);
				else
					m_pPoolingManager->Acquire(POOL_ID::MONSTER_GIANT_VAMPIRE, &desc);
			}
		}
		else if (m_iMaxElite == 3)
		{
			for (int i = 0; i < 3; i++)
			{
				RandomPos = m_pGameInstance->RandomValue_int(0, 3);
				desc.vPosition = m_MonsterSpawnPosVec[RandomPos];
				if (i == 0)
					m_pPoolingManager->Acquire(POOL_ID::MONSTER_GHOSTKNIGHT, &desc);
				else
					m_pPoolingManager->Acquire(POOL_ID::MONSTER_GIANT_VAMPIRE, &desc);
			}
		}
		m_iMaxElite++;
	}

	//포지션 랜덤, 뽑히는거 랜덤
	if (NormalMonsterCnt == 0)
	{
		for (int i = 0; i < 2; i++)
		{
			_float RandomPos = m_pGameInstance->RandomValue_int(0, 3);
			_float RandomMon = m_pGameInstance->RandomValue_int(0, 1);
			desc.vPosition = m_MonsterSpawnPosVec[RandomPos];
			m_pPoolingManager->Acquire(RandomMon == 0 ? POOL_ID::MONSTER_MONKEY_DEVIL : POOL_ID::MONSTER_SLAVE_VAMPIRE, &desc);
		}
	}
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Church::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

#ifdef _DEBUG
	Test_Inventory(fTimeDelta);
	Test_SavePoint(fTimeDelta);
#endif // _DEBUG
	Test_WorldEnding();

	
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Church::Render(const _float fTimeDelta)
{
	if (m_bMapCreate)
	{
		m_pGameInstance->Set_Capture(true);
		m_bMapCreate = false;
	}
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/

HRESULT Client::Level_Church::Ready_Player()
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
	if (pPlayer)
	{
		//스폰포인트_지하도시
		//_vector vPosition = XMVectorSet(-55.f, 10.f, -124.f, 1.f);
		_vector vPosition = XMVectorSet(-55.f, 14.f, -130.f, 1.f);
		pPlayer->Set_Position_ByController(vPosition);
		pPlayer->Set_Active(true);
	}
	else
	{
		return S_OK;
	}

	return S_OK;
}

HRESULT Client::Level_Church::Ready_Camera()
{
	CCamera_Object* pFollowCam = nullptr;
	Camera* playerCam = nullptr;

	Camera::CAMERA_DESC desc;
	desc.fFov = 700.f;

	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	//m_pGameInstance->Add_Camera(m_iLevel, L"Prototype_GameObject_Camera_Object", m_iLevel, _UINT(CAMERA::PLAYER_FOCUS_00), RCAST(GameObject**)(&pFollowCam));		// Player 포커싱 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));							// 자유시점 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Player, m_iLevel, _UINT(CAMERA::PLAYER_FOCUS_01), RCAST(GameObject**)(&playerCam), &desc);							// 플레이어 카메라
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

HRESULT Client::Level_Church::Ready_Monster()
{
	m_pPoolingManager->Register(POOL_ID::MONSTER_GIANT_WHITEDEVIL, 1, m_iLevel, Proto_GameObject(L"Giant_WhiteDevil"), m_iLevel, Layer_Monsters);

	Monster::MONSTER_DESC desc{};
	desc.vPosition = _float4(-203.539f, 15.6024f, -6.70267, 1.f);
	desc.vRotation = _float3(0.f, 180.f, 0.f);

	desc.fRotationSpeed = XMConvertToRadians(90.f);
	desc.fSpeed = 5.f;

	desc.tControllerDesc.vPosition = { desc.vPosition.x, desc.vPosition.y, desc.vPosition.z };
	desc.tControllerDesc.pOwner = nullptr;

	m_pPoolingManager->Acquire(POOL_ID::MONSTER_GIANT_WHITEDEVIL, &desc);

	m_pPoolingManager->Register(POOL_ID::MONSTER_WOLFGHOST, 1, m_iLevel, Proto_GameObject(L"WolfGhost"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_WOLFGHOST_ATTACK, 3, m_iLevel, Proto_GameObject(L"WolfGhost_Attack"), m_iLevel, Layer_Projectile);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_MONSTER_DIRECT, 50, m_iLevel, Proto_GameObject(L"Projectile_Direct"), m_iLevel, Layer_Projectile);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_MONSTER_HOMING, 10, m_iLevel, Proto_GameObject(L"Projectile_Homing"), m_iLevel, Layer_Projectile);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_WOLFGHOST_ICE, 3, m_iLevel, Proto_GameObject(L"Projectile_WolfHomingIce"), m_iLevel, Layer_Projectile);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_WOLFGHOST_EFFECT, 4, m_iLevel, Proto_GameObject(L"Projectile_Effect"), m_iLevel, Layer_Projectile);



	m_pPoolingManager->Register(POOL_ID::MONSTER_GHOSTKNIGHT, 5, m_iLevel, Proto_GameObject(L"GhostKnight_Halberd"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_GIANT_VAMPIRE, 5, m_iLevel, Proto_GameObject(L"Giant_Vampire"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_MONKEY_DEVIL, 5, m_iLevel, Proto_GameObject(L"Monkey_Devil"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_SLAVE_VAMPIRE, 5, m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, Layer_Monsters);
	return S_OK;
}

HRESULT Client::Level_Church::Ready_Light()
{
	LIGHT_DESC* desc = m_pGameInstance->Get_LightDesc(0);
	desc->vDirection = { 1.f, -0.7f, 0.15f, 0.f };
	m_pGameInstance->Set_LightDesc(0, *desc);

	//약하게 바닥 밝히는 빛추가
	LIGHT_DESC DescLight;
	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.fRange = 95.4f;
	DescLight.vPosition = _float4(-56.272f, 28.35f, -28.211f, 1.f);
	DescLight.vDiffuse = _float4(1.f, 0.9f, 0.8f, 1.f);
	DescLight.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	DescLight.vDirection = _float4(0.f, -0.69f, -1.f, 1.f);
	DescLight.fInnerCone = cos(XMConvertToRadians(1.0f));
	DescLight.fOuterCone = cos(XMConvertToRadians(29.5f));
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.vPosition = _float4(-56.272f, 28.35f, -96.081f, 1.f);
	DescLight.vDirection = _float4(0.03f, -1.f, 1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	//성당조명추가
	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.fRange = 24.1f;
	DescLight.vPosition = _float4(-68.42f, 31.33f, -29.355f, 1.f);
	DescLight.vDiffuse = _float4(20.f, 18.f, 16.f, 1.f);
	DescLight.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	DescLight.vDirection = _float4(0.1f, -0.02f, -0.2f, 1.f);
	DescLight.fInnerCone = cos(XMConvertToRadians(16.5f));
	DescLight.fOuterCone = cos(XMConvertToRadians(25.5f));
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.vPosition = _float4(-68.42f, 23.71f, -29.355f, 1.f);
	DescLight.vDirection = _float4(0.1f, -0.02f, -0.2f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.vPosition = _float4(-44.380f, 31.33f, -29.355f, 1.f);
	DescLight.vDirection = _float4(-0.39f, -0.1f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.vPosition = _float4(-44.380f, 23.71f, -29.355f, 1.f);
	DescLight.vDirection = _float4(-0.39f, -0.1f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.vPosition = _float4(-56.18f, 25.24f, -27.685f, 1.f);
	DescLight.vDirection = _float4(0.f, -0.4f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.vPosition = _float4(-56.18f, 32.94f, -27.685f, 1.f);
	DescLight.vDirection = _float4(0.f, -0.4f, -1.f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	//엔딩조명
	DescLight.eType = LIGHT::SPOTLIGHT;
	DescLight.fRange = 56.2f;
	DescLight.vPosition = _float4(-55.996f, 6.19f, -72.593f, 1.f);
	DescLight.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
	DescLight.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	DescLight.vDirection = _float4(0.f, -1.f, 0.f, 1.f);
	DescLight.fInnerCone = cos(XMConvertToRadians(5.5f));
	DescLight.fOuterCone = cos(XMConvertToRadians(25.5f));
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::POINT;
	DescLight.fRange = 2.7f;
	DescLight.vDiffuse = _float4(1000.f, 900.f, 800.f, 1.f);
	DescLight.vPosition = _float4(-55.996f, 6.19f, -72.593f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	DescLight.eType = LIGHT::POINT;
	DescLight.fRange = 5.2f;
	DescLight.vDiffuse = _float4(10.f, 9.f, 8.f, 1.f);
	DescLight.vPosition = _float4(-55.996f, -12.41f, -69.013f, 1.f);
	m_pGameInstance->Add_Light(m_pGameInstance->Get_LightCnt(), DescLight);

	//그림자
	SHADOW_DESC		ShadowDesc{};
	ShadowDesc.vPosition = _float4{ -20.f, 14.f, 0.f, 1.f };
	ShadowDesc.vAt = _float4{ -75.92f, 13.01f, -85.25f, 1.f };
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
	ShadowDesc1.vAddPos = _float4{ -20.f, 20.f, 0.f, 1.f };

	const _int NumCascade = 4;
	_float bias[NumCascade] = { 0.001f, 0.002f, 0.003f, 0.005f };
	if (FAILED(m_pGameInstance->Add_ShadowCascadeLight(NumCascade, ShadowDesc1, bias)))
		return E_FAIL;

	GameObject::GAMEOBJECT_DESC Desc;
	_wstring layer = Layer_GameObject;
#pragma region 1번창가(입구낮은거)
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(53.5f) };
	Desc.vScale = _float3{ 3.76f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -83.9f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-17.5f), XMConvertToRadians(15.f), XMConvertToRadians(50.5f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(20.f), XMConvertToRadians(-15.f), XMConvertToRadians(52.5f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -89.2f, 24.1f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 2번창가(입구높은거)
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(55.5f) };
	Desc.vScale = _float3{ 4.65f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-19.5f), XMConvertToRadians(15.f), XMConvertToRadians(53.f) };
	Desc.vScale = _float3{ 3.55f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(20.f), XMConvertToRadians(-15.f), XMConvertToRadians(53.f) };
	Desc.vScale = _float3{ 4.89f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -83.7f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 3번창가(안쪽낮은거)
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(53.5f) };
	Desc.vScale = _float3{ 3.76f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-54.5f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-50.5f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 3.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 24.1f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 4번창가(안쪽높은거)
	Desc.vRotation = _float3{ XMConvertToRadians(-1.f), XMConvertToRadians(0.5f), XMConvertToRadians(55.5f) };
	Desc.vScale = _float3{ 4.09f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-56.5f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 5.45f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(-54.f), XMConvertToRadians(90.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 5.22f, 1.f, 1.f };
	Desc.vPosition = _float4{ -87.2f, 32.6f, -69.f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

#pragma region 엔딩쪽
	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 15.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -56.f, 24.1f, -72.6f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(60.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 15.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -56.f, 24.1f, -72.6f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);

	Desc.vRotation = _float3{ XMConvertToRadians(0.f), XMConvertToRadians(120.f), XMConvertToRadians(0.f) };
	Desc.vScale = _float3{ 15.f, 1.f, 1.f };
	Desc.vPosition = _float4{ -56.f, 24.1f, -72.6f, 1.f };
	Desc.wstrName = L"GodRayMesh";
	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, TEXT("Prototype_GameObject_GodRayMesh"), m_iLevel, layer, nullptr, &Desc);
#pragma endregion

	return S_OK;
}

HRESULT Client::Level_Church::Ready_Collider()
{
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::INTERACTION);
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::EVENT_POINT);
	CAST(Player*)(m_pGameInstance->Get_Player())->Register_Collider_Player();	// 시간 없어서 하드코딩 편법좀 썼습니ㅏㄷ

	Cinematic_EventShape::CINEMATICEVENTSHAPE_DESC desc;
	desc.fRadius = 2.f;
	desc.vPosition = _float4(-69.750f, 5.282f, -130.583f, 1.f);

	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"Collider_Cinematic"), m_iLevel, L"Layer_Collider", nullptr, &desc);

	return S_OK;
}

HRESULT Client::Level_Church::Ready_Map()
{
	COUT("메인 맵 세팅중");

	_string strMapFolder = "../../DataFiles/Level_Church";
	_string strFileName = "MapData";

	m_pGameInstance->Spawn_LevelData(strMapFolder, m_iLevel, true);

	Spawn_SavePoint(LEVEL::CHURCH);

	if (FAILED(Spawn_MonsterTrigger(LEVEL::CHURCH, strMapFolder, strFileName)))
		return E_FAIL;

	m_bMapCreate = true;
	COUT("맵 세팅 완료");
	m_bMapCreate = true;

	return S_OK;
}

//////////////////////////////////////////////////////// UI 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Church::Ready_UIObject()
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

	event.bLoadJson = true;
	event.eToolType = TOOLTYPE::UI_TOOL;
	event.m_Path = "../../DataFiles/Level_UIDev/EndingUI.json";
	m_pGameInstance->Publish(event);

	return S_OK;

}
/******************************************************* UI 준비 함수 *******************************************************/

HRESULT Client::Level_Church::Ready_Values()
{
	Player_Stat* pPlayerStat = dynamic_cast<Player_Stat*>(m_pGameInstance->Get_Player()->Get_Component_FromName(Com_Stat));
	if (pPlayerStat == nullptr)
		return E_FAIL;

	//가드바 값연결
	UIObject* pGuardBar = m_pGameInstance->Find_UI_ByName(L"GuardBar");
	if (pGuardBar)
	{
		UIObj_GuardBar* ppGuardBar = dynamic_cast<UIObj_GuardBar*>(pGuardBar);
		if (ppGuardBar)
		{
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
		}

	}

	__super::Ready_Values();

	return S_OK;
}

HRESULT Client::Level_Church::Ready_Items()
{
	m_pPoolingManager->Register(POOL_ID::ITEM_GROUND, 10, m_iLevel, Proto_GameObject(L"Item"), m_iLevel, Layer_Items, nullptr, 0, POOL_POLICY::GROW);
	m_pPoolingManager->Register(POOL_ID::ITEM_DROP, 5, m_iLevel, Proto_GameObject(L"Item"), m_iLevel, Layer_Items);

	if (FAILED(Spawn_Item(LEVEL::CHURCH, "../../DataFiles/Level_Church", "MapData")))
		return E_FAIL;
	return S_OK;
}

HRESULT Client::Level_Church::Ready_MainEffects()
{
	//피 폭발
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_PLATE, m_pGameInstance->Find_Effect(L"Blood_Plate"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_PILLAR, m_pGameInstance->Find_Effect(L"Blood_Pillar"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_HAND, m_pGameInstance->Find_Effect(L"BloodBoom_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	//화염방사
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_ATTACK, m_pGameInstance->Find_Effect(L"FireBlast"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_HAND, m_pGameInstance->Find_Effect(L"FireBlast_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_PLATE, m_pGameInstance->Find_Effect(L"FireBlast_Plate"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_MAIN, m_pGameInstance->Find_Effect(L"FireBlast_Alpha"), 10,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	//번개 공용
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_HAND_CIRCLE, m_pGameInstance->Find_Effect(L"Thunder_Hand_Circle"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_HAND_AURA, m_pGameInstance->Find_Effect(L"Thunder_Hand_Smoke"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	//직선 얼음 공격
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_ICE_HAND, m_pGameInstance->Find_Effect(L"Ice_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_ICE_BULLET, m_pGameInstance->Find_Effect(L"Ice_Blast"), 50,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	//유도 얼음 공격
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_ICE_HOMING_BULLET, m_pGameInstance->Find_Effect(L"Ice_Blast_Chase"), 6,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_ICE_HOMING_HIT, m_pGameInstance->Find_Effect(L"Ice_Blast_Hit"), 6,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	
	//번개 유도탄
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE, m_pGameInstance->Find_Effect(L"Thunder_Gate_Smoke"), 4,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE_BLINK, m_pGameInstance->Find_Effect(L"Thunder_Gate_Blink"), 4,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_BULLET, m_pGameInstance->Find_Effect(L"Thunder_Bullet"), 20,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_BULLET_HIT, m_pGameInstance->Find_Effect(L"Thunder_Bullet_Hit"), 4,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	//직선 화염 공격
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET, m_pGameInstance->Find_Effect(L"HellFireBolt"), 10,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HIT, m_pGameInstance->Find_Effect(L"HellFireBolt_Hit"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HAND, m_pGameInstance->Find_Effect(L"HellFireBolt_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HAND_SHOKE, m_pGameInstance->Find_Effect(L"HellFireBolt_Hand_ShokeWave"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	//번개 광선
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_VOIDLASER, m_pGameInstance->Find_Effect(L"Wolf_VoidLaser"), 3,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_VOIDLASER_SMOKE, m_pGameInstance->Find_Effect(L"Wolf_VoidLaser_Smoke"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_SOLARLASER, m_pGameInstance->Find_Effect(L"Wolf_SolarLaser"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_SOLARLASER_PARTICLE, m_pGameInstance->Find_Effect(L"Wold_SolarLaser_Particle"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);	

	
	//번개
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_PLATE, m_pGameInstance->Find_Effect(L"Thunder_Plate"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_PILLAR, m_pGameInstance->Find_Effect(L"Thunder_Pillar"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE_END, m_pGameInstance->Find_Effect(L"Thunder_Gate_Smoke_End"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	
	// 텔레포트
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_TP_BOTTOM, m_pGameInstance->Find_Effect(L"Wolf_TP_Bottom"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_TP_START, m_pGameInstance->Find_Effect(L"Wolf_TP_Start"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_TP_END, m_pGameInstance->Find_Effect(L"Wolf_TP_End"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_TP_FLAME, m_pGameInstance->Find_Effect(L"Wolf_TP_FollowFlame"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_ENEMY_SHIELD_RED, m_pGameInstance->Find_Effect(L"Giant_Vampire_Shield"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// WHITE DEVIL
	POOLING->Register_Effect(POOL_ID::EFFECT_WHITEDEVIL_TRANSITION, m_pGameInstance->Find_Effect(L"WD_Transition"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_WHITEDEVIL_AURA, m_pGameInstance->Find_Effect(L"WD_Phase2_Aura"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_WHITEDEVIL_SLASH, m_pGameInstance->Find_Effect(L"Phase2_Slash"), 3,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	// GHOST KNIGHT
	POOLING->Register_Effect(POOL_ID::EFFECT_GHOSTKNIGHT_PARTICLE, m_pGameInstance->Find_Effect(L"GK_Special_ATK_Particle"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);
	POOLING->Register_Effect(POOL_ID::EFFECT_GHOSTKNIGHT_ATK, m_pGameInstance->Find_Effect(L"GK_Special_ATK"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	
	return S_OK;
}

#ifdef _DEBUG
HRESULT Client::Level_Church::Debug_WhiteDevil()
{
	/*중간보스 디버깅용-그냥바로생성하는 코드추가*/
	Monster::MONSTER_DESC desc{};
	desc.vPosition = _float4(-203.539f,15.6024f,-6.70267, 1.f);
	desc.vRotation = _float3(0.f, 180.f, 0.f);

	desc.fRotationSpeed = XMConvertToRadians(90.f);
	desc.fSpeed = 5.f;

	desc.tControllerDesc.vPosition = { desc.vPosition.x, desc.vPosition.y, desc.vPosition.z };
	desc.tControllerDesc.pOwner = nullptr;

	m_pPoolingManager->Acquire(POOL_ID::MONSTER_GIANT_WHITEDEVIL, &desc);

	Player* pPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
	if (pPlayer)
	{
		//스폰포인트_중보 앞
		_vector vPosition = XMVectorSet(-188.871f, 17.0f, -23.5f, 1.f);
		pPlayer->Set_Position_ByController(vPosition);
		pPlayer->Set_Active(true);
	}
	else
	{
		return S_OK;
	}
	return S_OK;
}
#endif // _DEBUG



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Church* Client::Level_Church::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Church* pInstance = new Level_Church(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Church 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Church::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Church 원본 생성 실패", L"경고!!!", );
}
void Client::Level_Church::Test_WorldEnding()
{
	
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Church::Free()
{
	m_pGameInstance->Clear_Level_Prototype(_UINT(LEVEL::CHURCH));
	m_pGameInstance->Clear_Level_GameObject(_UINT(LEVEL::CHURCH));
	m_pGameInstance->UnsubScribe(m_iItemTriggerHandle);
	//m_pPoolingManager->Clear_Range(POOL_ID::MONSTER_STAGE01_BEGIN, POOL_ID::MONSTER_STAGE01_END);

	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
