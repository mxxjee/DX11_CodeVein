#include "Client_Define.h"
#include "Level_Sample.h"

#include "GameInstance.h"
#include "Parser_UITool.h"
#include "Level_UIDev.h"
#include "UIObj_GuardBar.h"
#include "UIObj_Stamina.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_MonsterStatus.h"
#include "PoolingManager.h"

#include "InventoryManager.h"
#include "InteractionManager.h"

#include "Layer.h"
#include "Level_Load.h"
#include "UIObj_Cursor.h"

#pragma region Effect

#include "ParticleSystem.h"
#include "VFX_Parsing.h"

#pragma endregion

#pragma region PhysXTest
#include "Player.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_MasterRig.h"
#include "Player_Stat.h"

#include "Camera_Object.h"
#include "Camera_Free.h"
#include "Mesh.h"
#include "MT_Utils.h"
#include "ItemManager.h"
#include "MinimapManager.h"

#include "Monster.h"
#pragma endregion

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Sample::Level_Sample()
{
}

Client::Level_Sample::Level_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level_Client(pDevice, pContext)
{
}

Client::Level_Sample::~Level_Sample()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Sample::Initialize(LEVEL _level)
{
	m_LevelTitle = L"Djenarhk Qlcdl rhdwhsgksms Toavmf Fpqpf";
	__super::Initialize(_level);

	/* LEVEL::SAMPLE */
	m_iLevel = _UINT(_level);

	m_pGameInstance->Build_OcTree(_float3{ 0.f, 0.f, 0.f }, 500.f, 5);

	ItemManager::GetInstance()->Initialize(m_pDevice, m_pContext);
	MinimapManager::GetInstance()->Initialize(m_pDevice, m_pContext, (_uint)WINCX, (_uint)WINCY);
	m_pPoolingManager = PoolingManager::Get_Instance();
	Safe_AddRef(m_pPoolingManager);

	CHECK_FAILED(Ready_Camera(), E_FAIL);
	CHECK_FAILED(Ready_Monsters(), E_FAIL);
	CHECK_FAILED(Ready_StaticObject(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);
	CHECK_FAILED(Ready_UIObject(), E_FAIL);
	CHECK_FAILED(Ready_MainEffects(), E_FAIL);
	CHECK_FAILED(Ready_Values(), E_FAIL);
	CHECK_FAILED(Ready_Sky(), E_FAIL);
	CHECK_FAILED(Ready_Player(), E_FAIL);

	IMGUI_EVENT event;
	event.eType = IMGUI_EVENT_TYPE::UPDATE_PROTOTYPE;

	m_pGameInstance->Publish<IMGUI_EVENT>(event);



	/*마우스 기본값 = lock*/
	MOUSELOCK_EVENT mouseEvent;
	mouseEvent.bLock = true;
	m_pGameInstance->Publish(mouseEvent);


	//커서끄기
	UIObj_Cursor::CursorEvent cursorEvent;
	cursorEvent.bEnable = false;
	m_pGameInstance->Publish(cursorEvent);


	InventoryManager::GetInstance()->Set_Default_Inventory();
	InventoryManager::GetInstance()->Set_Haze(9999);

#pragma region 스킬영상용
	InventoryManager::GetInstance()->Set_Skill_Debug();
#pragma endregion

	
	ItemInfo* pItem=ItemManager::GetInstance()->Get_ItemInfo(5009);
	InventoryManager::GetInstance()->Add_Item(pItem);
	Safe_Delete(pItem);


	m_pGameInstance->Close_All_Menu();
	m_pGameInstance->Change_UIMode(UI_MODE::DEFAULT);


	InteractionManager::GetInstance()->Set_Player(m_pGameInstance->Get_Player());
	MinimapManager::GetInstance()->Set_Current_Level(LEVEL::SAMPLE);
	//Teleport_With_SavePoint();//세이브포인트를 통해 타고들어왔는지 확인하고, 위치설정한다.


	

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/

//////////////////////////////////////////////////////// UI 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Sample::Ready_UIObject()
{
	__super::Ready_UIObject();

	//이 씬에서만사용하는 UI로드
	//BossNameUI
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


//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Sample::Ready_Player()
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
	if (pPlayer)
	{
		//스폰포인트_지하도시
		_vector vPosition = XMVectorSet(6.f, -25.f, 1.f, 1.f);
		pPlayer->Set_Position_ByController(vPosition);
		pPlayer->Set_Active(true);
	}
	else
	{
		BREAK;
		return S_OK;
	}

	return S_OK;
}

HRESULT Client::Level_Sample::Ready_Camera()
{
	Camera* playerCam = nullptr;

	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));							// 자유시점 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Player, m_iLevel, _UINT(CAMERA::PLAYER_FOCUS_01), RCAST(GameObject**)(&playerCam));							// 플레이어 카메라
	m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_01));


	if (playerCam)
	{
		//미니맵매니저에도 연결해주기
		MinimapManager::GetInstance()->Set_RotateCamera(playerCam);
	}



	return S_OK;
}

HRESULT Client::Level_Sample::Ready_Monsters()
{
	m_pPoolingManager->Register(POOL_ID::MONSTER_OLIVER, 1, m_iLevel, Proto_GameObject(L"St01_BossOliver"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_SLAVE_VAMPIRE, 1, m_iLevel, Proto_GameObject(L"Slave_Vampire"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_SLAVE_DEVIL, 1, m_iLevel, Proto_GameObject(L"Slave_Devil"), m_iLevel, Layer_Monsters);
	//m_pPoolingManager->Register(POOL_ID::MONSTER_SLIME, 1, m_iLevel, Proto_GameObject(L"St01_Slime"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_WOLFGHOST, 1, m_iLevel, Proto_GameObject(L"WolfGhost"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_WOLFGHOST_ATTACK, 3, m_iLevel, Proto_GameObject(L"WolfGhost_Attack"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_MONSTER_DIRECT, 50, m_iLevel, Proto_GameObject(L"Projectile_Direct"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_MONSTER_HOMING, 10, m_iLevel, Proto_GameObject(L"Projectile_Homing"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_WOLFGHOST_ICE, 3, m_iLevel, Proto_GameObject(L"Projectile_WolfHomingIce"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::PROJECTILE_WOLFGHOST_EFFECT, 4, m_iLevel, Proto_GameObject(L"Projectile_Effect"), m_iLevel, Layer_Monsters);
	
	m_pPoolingManager->Register(POOL_ID::MONSTER_GIANT_VAMPIRE, 1, m_iLevel, Proto_GameObject(L"Giant_Vampire"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_MONKEY_DEVIL, 1, m_iLevel, Proto_GameObject(L"Monkey_Devil"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_GIANT_WHITEDEVIL, 1, m_iLevel, Proto_GameObject(L"Giant_WhiteDevil"), m_iLevel, Layer_Monsters);
	m_pPoolingManager->Register(POOL_ID::MONSTER_GHOSTKNIGHT, 1, m_iLevel, Proto_GameObject(L"GhostKnight_Halberd"), m_iLevel, Layer_Monsters);

	return S_OK;
}

HRESULT Client::Level_Sample::Ready_StaticObject()
{
	COUT("튜토리얼 맵 세팅중");

	m_pGameInstance->Spawn_LevelData("../../DataFiles/Level_Main/Tutorial", m_iLevel, true);

	COUT("맵 세팅 완료");

	return S_OK;
}

HRESULT Client::Level_Sample::Ready_Light()
{
	LIGHT_DESC Desc;
	Desc.eType = LIGHT::DIRECTIONAL;
	Desc.vDiffuse = { 1.f, 1.f, 1.f, 1.f };
	Desc.vAmbient = { 1.f, 1.f, 1.f, 1.f };
	Desc.vSpecular = { 0.8f, 0.85f, 0.9f, 1.f }; //0.3으로 해놨었음 
	Desc.vDirection = { 0.f, -0.6f, 1.f, 0.f };

	//임시라서 그냥 자체 생산?
	m_pGameInstance->Add_Light(0, Desc);

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
	ShadowDesc1.vOrthSize = 40.f;
	ShadowDesc1.fNearZ = 0.1f;
	ShadowDesc1.fFarZ = 500.f;
	ShadowDesc1.fDepth = 200.f;

	const _int NumCascade = 4;
	_float bias[NumCascade] = { 0.001f, 0.002f, 0.003f, 0.005f };
	if (FAILED(m_pGameInstance->Add_ShadowCascadeLight(NumCascade, ShadowDesc1, bias)))
		return E_FAIL;

	return S_OK;
}

HRESULT Client::Level_Sample::Ready_Values()
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


	__super::Ready_Values();

	return S_OK;
}


HRESULT Client::Level_Sample::Ready_MainEffects()
{
	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_PLATE, m_pGameInstance->Find_Effect(L"Blood_Plate"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_PILLAR, m_pGameInstance->Find_Effect(L"Blood_Pillar"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_BLOODBOOM_HAND, m_pGameInstance->Find_Effect(L"BloodBoom_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_ATTACK, m_pGameInstance->Find_Effect(L"FireBlast"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_HAND, m_pGameInstance->Find_Effect(L"FireBlast_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIREBLAST_PLATE, m_pGameInstance->Find_Effect(L"FireBlast_Plate"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_HAND_CIRCLE, m_pGameInstance->Find_Effect(L"Thunder_Hand_Circle"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_HAND_AURA, m_pGameInstance->Find_Effect(L"Thunder_Hand_Smoke"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_ICE_HAND, m_pGameInstance->Find_Effect(L"Ice_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_ICE_BULLET, m_pGameInstance->Find_Effect(L"Ice_Blast"), 50,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE, m_pGameInstance->Find_Effect(L"Thunder_Gate_Smoke"), 4,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_GATE_BLINK, m_pGameInstance->Find_Effect(L"Thunder_Gate_Blink"), 4,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_BULLET, m_pGameInstance->Find_Effect(L"Thunder_Bullet"), 4,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_THUNDER_BULLET_HIT, m_pGameInstance->Find_Effect(L"Thunder_Bullet_Hit"), 4,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET, m_pGameInstance->Find_Effect(L"HellFireBolt"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HIT, m_pGameInstance->Find_Effect(L"HellFireBolt_Hit"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HAND, m_pGameInstance->Find_Effect(L"HellFireBolt_Hand"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_FIRE_HELLFIREBULLET_HAND_SHOKE, m_pGameInstance->Find_Effect(L"HellFireBolt_Hand_ShokeWave"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_VOIDLASER, m_pGameInstance->Find_Effect(L"Wolf_VoidLaser"), 3,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_VOIDLASER_SMOKE, m_pGameInstance->Find_Effect(L"Wolf_VoidLaser_Smoke"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_SOLARLASER, m_pGameInstance->Find_Effect(L"Wolf_SolarLaser"), 1,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_WOLF_SOLARLASER_PARTICLE, m_pGameInstance->Find_Effect(L"Wold_SolarLaser_Particle"), 2,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

	POOLING->Register_Effect(POOL_ID::EFFECT_ENEMY_SHIELD_RED, m_pGameInstance->Find_Effect(L"Giant_Vampire_Shield"), 5,
		m_iLevel, Layer_Effect, POOL_POLICY::RECYCLE_OLDEST);

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

/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Sample::Update_Priority(const _float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(DIK_F7))
	{
		if (m_bCamera)
		{
			m_bCamera = false;
			g_bFreeCam = false;
			m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_01));

			m_pGameInstance->Set_Visible_All_UI(true);
		}

		else
		{
			m_bCamera = true;
			g_bFreeCam = true;
			m_pGameInstance->Camera_Change(_UINT(CAMERA::FREE));

			
			m_pGameInstance->Set_Visible_All_UI(false);
		}
	}

	m_pGameInstance->Update_Priority(fTimeDelta);
	
	//// 메인 레벨로 이동
	//if (m_pGameInstance->KeyDown(DIK_F8))
	//{
	//	Publish_ExitEvent(true, false);

	//	m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::MAIN));
	//	m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));
	//	return 0;
	//}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Sample::Update(const _float fTimeDelta)
{
	m_pGameInstance->Update_Parallel(fTimeDelta);
	m_pGameInstance->Update_PhysX(fTimeDelta);

#ifdef _DEBUG
	Test_WeaponChange();

#endif // _DEBUG


	m_pGameInstance->Update(fTimeDelta);


	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Sample::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

#ifdef _DEBUG
	Test_SavePoint(fTimeDelta);
#endif // _DEBUG

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Sample::Render(const _float fTimeDelta)
{
	MinimapManager::GetInstance()->Clear_Icons_On_UI();

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Sample* Client::Level_Sample::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Sample* pInstance = new Level_Sample(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Sample 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Sample::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Sample 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Sample::Free()
{
	m_pGameInstance->Clear_Level_Prototype(_UINT(LEVEL::SAMPLE));
	m_pGameInstance->Clear_Level_GameObject(_UINT(LEVEL::SAMPLE));
	PoolingManager::Get_Instance()->Clear_Range(POOL_ID::MONSTER_STAGE01_BEGIN, POOL_ID::MONSTER_STAGE01_END);
	POOLING->Clear_Range(POOL_ID::PROJECTILE_WOLFGHOST_ATTACK, POOL_ID::PROJECTILE_WOLFGHOST_ICE);

	__super::Free();

	Safe_Release(m_pParsing_Shader);
}
/******************************************************* 객체 반환 함수 *******************************************************/
