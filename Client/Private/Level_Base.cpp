#include "Client_Define.h"
#include "Level_Base.h"

#include "GameInstance.h"
#include "Parser_UITool.h"
#include "Level_UIDev.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_Cursor.h"

#include "ItemManager.h"
#include "InventoryManager.h"
#include "Layer.h"

#include "ParticleSystem.h"
#include "VFX_Parsing.h"

#include "Player.h"
#include "Player_Stat.h"
#include "Yakumo.h"

#include "UIObj_GuardBar.h"
#include "UIObj_Stamina.h"

#include "Player_MasterRig.h"
#include "Camera_Object.h"
#include "Camera_Free.h"

#include "InteractionManager.h"
#include "MinimapManager.h"
#include "NPC_Murasame.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Base::Level_Base()
{
}

Client::Level_Base::Level_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level_Client(pDevice, pContext)
{
}

Client::Level_Base::~Level_Base()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Base::Initialize(LEVEL _level)
{
	m_LevelTitle = L"Home Base";
	__super::Initialize(_level);

	m_iLevel = _UINT(_level);
	m_pInventoryManager = InventoryManager::GetInstance();

	m_pGameInstance->Build_OcTree(_float3{ 0.f, 0.f, 0.f }, 300.f, 4);

	ItemManager::GetInstance()->Initialize(m_pDevice, m_pContext);

	CHECK_FAILED(Ready_Player(), E_FAIL);
	CHECK_FAILED(Ready_Camera(), E_FAIL);
	CHECK_FAILED(Ready_Light(), E_FAIL);
	CHECK_FAILED(Ready_UIObject(), E_FAIL);
	CHECK_FAILED(Ready_NPC(), E_FAIL);
	CHECK_FAILED(Ready_Values(), E_FAIL);
	//CHECK_FAILED(Ready_Effects(), E_FAIL);
	CHECK_FAILED(Ready_Map(), E_FAIL);
	CHECK_FAILED(Ready_Sky(), E_FAIL);
	CHECK_FAILED(Ready_Collider(), E_FAIL);
	CHECK_FAILED(Ready_Items(), E_FAIL);

	SYSTEM_EVENT Event;
	Event.eType = SYSTEM_EVENT_TYPE::END_LOAD;
	m_pGameInstance->Publish(Event);

	MOUSELOCK_EVENT mouseEvent;
	mouseEvent.bLock = true;
	m_pGameInstance->Publish(mouseEvent);

	UIObj_Cursor::CursorEvent cursorEvent;
	cursorEvent.bEnable = false;
	m_pGameInstance->Publish(cursorEvent);

	InteractionManager::GetInstance()->Set_Player(m_pGameInstance->Get_Player());
	MinimapManager::GetInstance()->Set_Current_Level(LEVEL::BASE);
	InventoryManager::GetInstance()->Set_Default_Inventory();
	m_pGameInstance->Close_All_Menu();
	m_pGameInstance->Change_UIMode(UI_MODE::BASE);

	//다시플레이어입력활성화!!
	GameObject* pObj = m_pGameInstance->Get_Player();
	if (pObj)
		static_cast<Player*>(pObj)->Set_CanTurn(true);
	m_pGameInstance->Play_Sound("Play_musium", 0.f, true);
	m_pGameInstance->Play_Sound("BaseAmb", 0.f, true);
	INPUT_LOCK_EVENT LockEvent;
	LockEvent.bLock = false;
	m_pGameInstance->Publish(LockEvent);

	//맵에 환경변화
	Change_Color();
	//Teleport_With_SavePoint();//세이브포인트를 통해 타고들어왔는지 확인하고, 위치설정한다.


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Base::Update_Priority(const _float fTimeDelta)
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
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Base::Update(const _float fTimeDelta)
{
	
	m_pGameInstance->Update_Parallel(fTimeDelta);

	m_pGameInstance->Update_PhysX(fTimeDelta);

	m_LevelEnterAlarm.Update(fTimeDelta);

	m_pGameInstance->Update(fTimeDelta);
	m_PlayerStatus.Update(fTimeDelta);

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Level_Base::Update_Late(const _float fTimeDelta)
{
	m_pGameInstance->Update_Late(fTimeDelta);

#ifdef _DEBUG
	Test_Haze();

#endif // _DEBUG


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Base::Render(const _float fTimeDelta)
{
	if (m_bMapCreate)
	{
		m_pGameInstance->Set_Capture(true);
		m_bMapCreate = false;
	}

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/

HRESULT Client::Level_Base::Ready_Player()
{
	Player* pPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
	if (pPlayer)
	{
		//스폰포인트_지하도시
		_vector vPosition = XMVectorSet(0.f, 10.f, -25.f, 1.f);
		pPlayer->Set_Position_ByController(vPosition);
		pPlayer->Set_Active(true);
	}
	else
	{
		return S_OK;
	}

	Yakumo* pYakumo = dynamic_cast<Yakumo*>(m_pGameInstance->Get_Companion());
	if (pYakumo)
	{
		pYakumo->Set_Active(false);

		UI_MasterEvent Event;
		Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
		Event.m_bFlag = false;		//끄고싶으면 false값 넘겨주기
		Event.m_bPersistent = true;
		Event.m_ActionName = "ActiveUI";
		Event.m_Text = L"Buddy_Statusbar";

		m_pGameInstance->Publish(Event);
	}
	

	return S_OK;
}

HRESULT Client::Level_Base::Ready_Camera()
{
	Camera* playerCam = nullptr;

	m_pGameInstance->CameraManager_Reset(_UINT(CAMERA::END));
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Free, m_iLevel, _UINT(CAMERA::FREE));							// 자유시점 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_Player, m_iLevel, _UINT(CAMERA::PLAYER_FOCUS_01), RCAST(GameObject**)(&playerCam));							// 플레이어 카메라
	m_pGameInstance->Add_Camera(m_iLevel, Proto_GameObject_Camera_NPC, m_iLevel, _UINT(CAMERA::NPC_SHOP));
	m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_01));

	if (playerCam)	//미니맵매니저에도 연결해주기
		MinimapManager::GetInstance()->Set_RotateCamera(playerCam);

	return S_OK;
}

HRESULT Client::Level_Base::Ready_NPC()
{
	NPC::NPC_DESC Desc{};
	PHYSX_CONTROLLER_DESC ControllerDesc; 

	ControllerDesc.pOwner = nullptr;
	ControllerDesc.eActorType = PX_ACTOR_TYPE::MONSTER;

	Desc.vPosition = _float4(4.512f, 4.f, -10.124f, 1.f);
	ControllerDesc.vPosition = { Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z };
	Desc.tControllerDesc = ControllerDesc;

	m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, Proto_GameObject(L"NPC_Murasame"), m_iLevel, L"Layer_NPC", nullptr, &Desc);

	return S_OK;
}

HRESULT Client::Level_Base::Ready_Light()
{
	//LIGHT_DESC Desc;
	//Desc.eType = LIGHT::DIRECTIONAL;
	//Desc.vDiffuse = { 1.f, 1.f, 1.f, 1.f };
	//Desc.vAmbient = { 0.1f, 0.1f, 0.1f, 1.f };
	//Desc.vSpecular = { 0.8f, 0.85f, 0.9f, 1.f }; //0.3으로 해놨었음 
	//Desc.vDirection = { 1.f, -0.7f, 0.15f, 0.f };

	LIGHT_DESC* desc = m_pGameInstance->Get_LightDesc(0);
	desc->vDirection = { 1.f, -0.7f, 0.15f, 0.f };
	m_pGameInstance->Set_LightDesc(0, *desc);
	//임시라서 그냥 자체 생산?
	//m_pGameInstance->Add_	Light(0, *desc);

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
	ShadowDesc.vPosition = _float4{ -20.f, 14.f, 0.f, 1.f };
	ShadowDesc.vAt = _float4{ -33.61f, 15.20f, -2.39f, 1.f };
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

	return S_OK;
}

HRESULT Client::Level_Base::Ready_Collider()
{
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::INTERACTION);
	m_pGameInstance->Add_CollisionPair(COLLISION_GROUP::PLAYER, COLLISION_GROUP::EVENT_POINT);

	return S_OK;
}

HRESULT Client::Level_Base::Ready_Map()
{
	COUT("거점 : 맵 세팅중");
	m_pGameInstance->Spawn_LevelData("../../DataFiles/Level_Base", m_iLevel, true);
	Spawn_SavePoint(LEVEL::BASE);
	COUT("거점 : 맵 세팅 완료");

	m_bMapCreate = true;

	return S_OK;
}

HRESULT Client::Level_Base::Ready_Values()
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


	__super::Ready_Values();


	return S_OK;
}

HRESULT Client::Level_Base::Ready_Items()
{

	return S_OK;
}

//여긴오면 한번만 바뀌게
void  Client::Level_Base::Change_Color()
{
	m_pGameInstance->ChangeBaseMap();
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Base* Client::Level_Base::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Level_Base* pInstance = new Level_Base(pDevice, pContext);

	pInstance->Set_Level(_UINT(_level));
	//MSG_FAIL(pInstance->Initialize(_level), L"Level_Base 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::Level_Base::Initialize_Level()
{
	MSG_FAIL(Initialize(CAST(LEVEL)(m_iLevel)), L"Level_Base 원본 생성 실패", L"경고!!!", );
}
/******************************************************* 생성자 호출 함수 *******************************************************/

//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Base::Free()
{
	
	m_pGameInstance->StopSound("BaseAmb");
	m_pGameInstance->StopSound("Play_musium");
	m_pGameInstance->Clear_Level_Prototype(_UINT(LEVEL::BASE));
	m_pGameInstance->Clear_Level_GameObject(_UINT(LEVEL::BASE));

	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
