#include "Client_Define.h"
#include "Level_Client.h"


#include "GameInstance.h"
#include "Parser_UITool.h"
#include "Level_UIDev.h"
#include "UIObj_GuardBar.h"
#include "UIObj_Stamina.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_FadeScreen.h"
#include "UIObj_LoadingScreen.h"

#include "InventoryManager.h"
#include "MinimapManager.h"
#include "Layer.h"

#include "ItemManager.h"
#include "PoolingManager.h"
#include "InteractionManager.h"


#include "PlayerStatus.h"
#include "Player_Stat.h"
#include "Player.h"

#include "MapObject.h"
#include "Monster_EventShape.h"
#pragma region Effect

#include "ParticleSystem.h"
#include "VFX_Parsing.h"

#pragma endregion

#include "Level_Load.h"
#include "UIObj_PopUp_MapTitle.h"
#include "GodRaySun.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Level_Client::Level_Client()
{
}

Client::Level_Client::Level_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Level(pDevice, pContext)
{
}

Client::Level_Client::~Level_Client()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Client::Initialize(LEVEL _level)
{
	////버튼 눌렀을때 씬이동 [이벤트구독]
	//m_iLevelChangeHandle = m_pGameInstance->Subscribe<LevelChangeEvent>([this](const LevelChangeEvent& e)
	//	{

	//		m_pGameInstance->Clear_Scene_UI();
	//		MinimapManager::GetInstance()->Clear_Minimap();
	//		InteractionManager::GetInstance()->Clear_InteractionManager();



	//		m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, e.eNextLevel));
	//		m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));


	//	});
	
	m_LevelEnterAlarm.Elapsed = 0.f;
	m_LevelEnterAlarm.Limit = 5.f;
	m_LevelEnterAlarm.m_AlarmFunc = [this]()
		{
			UIObj_PopUp_MapTitle::MapTitleUIEvent Event;
			Event.m_Text = m_LevelTitle;
			m_pGameInstance->Publish(Event);

			m_LevelEnterAlarm.Off();
		};

	m_LevelEnterAlarm.On();
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// UI 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Client::Ready_UIObject()
{

	
	m_pGameInstance->Set_UIManager_Active(true);
	Parser_UITool::Set_LoadLevel((LEVEL)m_iLevel);


#ifdef _DEBUG
	UIComponent::Set_DrawDebug(false);
#endif // DEBUG


	return S_OK;

}
/******************************************************* UI 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 이펙트 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Client::Ready_Effects()
{
	m_pParsing_VFX = VFX_Parsing::Create(m_iLevel);

	// JSON 파일 목록
	static vector<_string> vecEffectFiles;
	static _bool bScan = { false };
	if (bScan == false)
	{
		namespace fs = std::filesystem;
		fs::path effectDir("../../DataFiles/Effects");
		if (fs::exists(effectDir))
		{
			for (auto& effect : fs::directory_iterator(effectDir))
			{
				if (effect.path().extension() == ".json")
					vecEffectFiles.push_back(effect.path().stem().string());	// ABC.json => ABC로 바꿔줌
			}
		}
		bScan = true;
	}

	// 컨테이너에 하나도 없다면 이벤트 발생 안함
	if (vecEffectFiles.size() != 0)
	{
		for (_uint i = 0; i < vecEffectFiles.size(); i++)
		{
			LOADEVENT event;
			event.eToolType = TOOLTYPE::VFX_TOOL;
			event.m_Path = "../../DataFiles/Effects/" + vecEffectFiles[i] + ".json";
			m_pGameInstance->Publish<LOADEVENT>(event);
		}
	}

	// Decal_Blood 풀 준비
	m_pGameInstance->Initialize_Pool(_UINT(LEVEL::STATIC), L"Prototype_GameObject_DecalBlood", m_iLevel, 64);

	return S_OK;
}

HRESULT Client::Level_Client::Spawn_SavePoint(LEVEL _level)
{
	int iTargetMapType = 0;
	switch (_level)
	{
	case LEVEL::BASE:	iTargetMapType = 1; break;
	case LEVEL::MAIN:	iTargetMapType = 2; break;
	case LEVEL::CHURCH:	iTargetMapType = 3; break; 
	default:
		return S_OK;
	}

	std::ifstream file("../../DataFiles/Level_All/MapData_SP.json");
	if (!file.is_open())
		return S_OK;

	nlohmann::json root;
	try { file >> root; }
	catch (...) {
		return E_FAIL;
	}

	if (!root.contains("data") || !root["data"].is_array())
		return S_OK;

	for (auto& objNode : root["data"])
	{
		if (objNode.contains("ExtraData") && objNode["ExtraData"].contains("MyMapType"))
		{
			int iSavedMapType = objNode["ExtraData"]["MyMapType"].get<int>();

			if (iSavedMapType != iTargetMapType)
				continue;
		}
		else
		{
			continue;
		}

		if (!objNode.contains("WorldMatrix")) continue;

		MapObject::MAPOBJECT_DESC desc{};

		auto& matArr = objNode["WorldMatrix"];
		if (matArr.size() == 16)
		{
			_float4x4 matWorld{};
			for (_uint i = 0; i < 16; ++i)
				matWorld.m[i / 4][i % 4] = matArr[i].get<_float>();

			desc.bSetWorldPos = true;
			desc.matWorldPos = matWorld;
		}

		if (objNode.contains("ExtraData"))
		{
			desc.jExtraData = objNode["ExtraData"];
		}

		_wstring wstrProto = L"Prototype_GameObject_SavePoint";
		if (objNode.contains("PrototypeName"))
		{
			std::string strProto = objNode["PrototypeName"].get<std::string>();
			wstrProto = _wstring(strProto.begin(), strProto.end());
		}

		_wstring wstrLayer = L"Layer_SP";
		if (objNode.contains("LayerName"))
		{
			std::string strLayer = objNode["LayerName"].get<std::string>();
			wstrLayer = _wstring(strLayer.begin(), strLayer.end());
		}

		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(_level), wstrProto, _UINT(_level), wstrLayer, nullptr, &desc)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT Client::Level_Client::Spawn_MonsterTrigger(LEVEL _level, const _string& strFolderPath, const _string& strFileName)
{
	_string strFullTarget = strFolderPath + "/" + strFileName + "_Trigger.json";

	std::ifstream file(strFullTarget);
	if (!file.is_open())
	{
		COUT("[Warning] Trigger file not found at: " + strFullTarget);
		return S_OK;
	}

	nlohmann::json root;
	try { file >> root; }
	catch (...) { return E_FAIL; }

	if (!root.contains("data") || !root["data"].is_array())
		return S_OK;

	for (auto& objNode : root["data"])
	{
		if (!objNode.contains("WorldMatrix")) continue;

		// 월드 행렬 복구
		auto& matArr = objNode["WorldMatrix"];
		_float4x4 matWorld{};
		for (_uint i = 0; i < 16; ++i)
			matWorld.m[i / 4][i % 4] = matArr[i].get<_float>();

		_matrix matW = XMLoadFloat4x4(&matWorld);
		_vector vScale, vRot, vPos;
		XMMatrixDecompose(&vScale, &vRot, &vPos, matW);

		// Desc 조립
		Monster_EventShape::MonsterEventShapeDesc desc;
		desc.pOwner = nullptr;
		desc.eColliderType = COLLIDER::AABB;
		XMStoreFloat3(&desc.vOffSet, vPos);

		_float3 fScale;
		XMStoreFloat3(&fScale, vScale);
		desc.Extents = _float3(fScale.x * 0.5f, fScale.y * 0.5f, fScale.z * 0.5f);

		if (objNode.contains("ExtraData"))
			desc.jExtraData = objNode["ExtraData"];

		_wstring wstrProto = Proto_GameObject_Monster_EventShape;
		_wstring wstrLayer = L"Layer_Trigger";

		if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(_level), wstrProto, _UINT(_level), wstrLayer, nullptr, &desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT Client::Level_Client::Spawn_Item(LEVEL _level, const _string& strFolderPath, const _string& strFileName)
{
	_string strFullTarget = strFolderPath + "/" + strFileName + "_Item.json";

	std::ifstream file(strFullTarget);
	if (!file.is_open())
	{
		COUT("[Warning] Item file not found at: " + strFullTarget);
		return S_OK; // 파일이 없어도 크래시 내지 않고 부드럽게 넘김
	}

	nlohmann::json root;
	try { file >> root; }
	catch (...) { return E_FAIL; }

	if (!root.contains("data") || !root["data"].is_array())
		return S_OK;

	for (auto& objNode : root["data"])
	{
		if (!objNode.contains("WorldMatrix")) continue;

		auto& matArr = objNode["WorldMatrix"];
		_float4x4 matWorld{};
		for (_uint i = 0; i < 16; ++i)
			matWorld.m[i / 4][i % 4] = matArr[i].get<_float>();

		MapObject::MAPOBJECT_DESC desc{};
		desc.bSetWorldPos = true;
		desc.matWorldPos = matWorld;

		if (objNode.contains("ExtraData"))
			desc.jExtraData = objNode["ExtraData"];

		_wstring wstrProto = L"Item";
		if (objNode.contains("PrototypeName"))
		{
			std::string strProto = objNode["PrototypeName"].get<std::string>();
			wstrProto = _wstring(strProto.begin(), strProto.end());
		}

		_wstring wstrLayer = L"Layer_Item";

		if (wstrProto == L"Item" || wstrProto == L"Prototype_GameObject_Item")
		{
			PoolingManager::Get_Instance()->Acquire(POOL_ID::ITEM_GROUND, &desc);
		}
		else
		{
			if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(_UINT(_level), wstrProto, _UINT(_level), wstrLayer, nullptr, &desc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

/******************************************************* 이펙트 준비 함수 *******************************************************/

HRESULT Client::Level_Client::Ready_Sky()
{
	m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), Proto_GameObject_SkySphere, m_iLevel, Layer_Sky);

	GodRaySun::GodRayDesc Desc;
	//여기서 레벨별로 태양위치 다르게 생성하기
	if (m_iLevel == _UINT(LEVEL::MAIN))
		Desc.vSunPos = _float4{ -122.29f, 105.36f, -153.458f, 1.f };
	else if (m_iLevel == _UINT(LEVEL::BASE))
		Desc.vSunPos = _float4{ -185.89f, 95.86f, -43.45f, 1.f };
	else if (m_iLevel == _UINT(LEVEL::CHURCH))
		Desc.vSunPos = _float4{ -296.82, 102.69f, -69.81f, 1.f };
	m_pGameInstance->Add_GameObject_To_Layer(_UINT(LEVEL::STATIC), Proto_GameObject_SphereSun, m_iLevel, Layer_Sky, nullptr, &Desc);

	return S_OK;
}

#ifdef _DEBUG
//////////////////////////////////////////////////////// 인벤토리 테스트 함수 ////////////////////////////////////////////////////////
void Client::Level_Client::Test_Inventory(const _float fTimeDelta)
{
#pragma region 아이템 들어오는거 확인용
	//////ItemTest
	//if (m_pGameInstance->KeyDown(DIK_NUMPAD1))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"블러드 샷");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}

	//if (m_pGameInstance->KeyDown(DIK_NUMPAD2))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"블레이즈 로어");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}

	//if (m_pGameInstance->KeyDown(DIK_NUMPAD3))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"기프트힐");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}


	//if (m_pGameInstance->KeyDown(DIK_NUMPAD4))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"헤비 액스");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}

	//if (m_pGameInstance->KeyDown(DIK_NUMPAD5))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"체력 증가");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}

	//if (m_pGameInstance->KeyDown(DIK_NUMPAD6))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"명혈 농축약");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}

	//if (m_pGameInstance->KeyDown(DIK_NUMPAD7))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"여왕 토벌대 아장/가시형");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}


	//if (m_pGameInstance->KeyDown(DIK_NUMPAD8))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"재생력");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}

	//if (m_pGameInstance->KeyDown(DIK_NUMPAD9))
	//{
	//	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"캐스터");
	//	InventoryManager::GetInstance()->Add_Item(pItemInfo);

	//	Safe_Delete(pItemInfo);
	//}


	//if (m_pGameInstance->KeyDown(DIK_DOWNARROW))
	//{
	//	InventoryManag
// er::GetInstance()->Use_Item(L"명혈 농축약");

	//}


#pragma endregion
}
void Client::Level_Client::Test_SavePoint(const _float fTimeDelta)
{
	if (m_pGameInstance->KeyDown(DIK_P))
	{
		UI_MasterEvent Event;
		Event.m_ActionName = "ActiveUI";
		Event.m_Text = L"PopUp_SavePoint";
		Event.m_bFlag = true;
		Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;

		m_pGameInstance->Publish(Event);

	}

}

void Client::Level_Client::Test_Haze()
{
	if (m_pGameInstance->KeyDown(DIK_O))
	{
		InventoryManager::GetInstance()->Add_Haze(1000);
	}


}

void Client::Level_Client::Test_WeaponChange()
{
	if (m_pGameInstance->KeyDown(DIK_NUMPAD1))
	{
		ItemInfo* pInfo = ItemManager::GetInstance()->Get_ItemInfo(L"흑의의 곡도");
		if (pInfo)
			InventoryManager::GetInstance()->Add_Item(pInfo);

		Safe_Delete(pInfo);

	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD2))
	{
		ItemInfo* pInfo = ItemManager::GetInstance()->Get_ItemInfo(L"여왕 토벌대의 도끼창");
		if (pInfo)
			InventoryManager::GetInstance()->Add_Item(pInfo);

		Safe_Delete(pInfo);

	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD1))
	{
		ItemInfo* pInfo = ItemManager::GetInstance()->Get_ItemInfo(L"츠바이헨더");
		if (pInfo)
			InventoryManager::GetInstance()->Add_Item(pInfo);

		Safe_Delete(pInfo);

	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD3))
	{
		ItemInfo* pInfo = ItemManager::GetInstance()->Get_ItemInfo(L"타락한 브로드 소드");
		if (pInfo)
			InventoryManager::GetInstance()->Add_Item(pInfo);

		Safe_Delete(pInfo);

	}

	if (m_pGameInstance->KeyDown(DIK_NUMPAD4))
	{
		ItemInfo* pInfo = ItemManager::GetInstance()->Get_ItemInfo(L"이형의 저주검");
		if (pInfo)
			InventoryManager::GetInstance()->Add_Item(pInfo);

		Safe_Delete(pInfo);

	}

}

//////////////////////////////////////////////////////// 플레이어 스탯 테스트 함수 ////////////////////////////////////////////////////////
void Client::Level_Client::Test_PlayerStatus(const _float fTimeDelta)
{/*가드 이벤트 실행*/
	//피격시..실행할것들(가드 게이지 갱신 / hp 줄어들기)
	if (m_pGameInstance->KeyDown(DIK_SPACE))
	{
		//가드값 줄어들고있는 와중엔 처리X
		CHECK_TRUE(m_PlayerStatus.m_bFocusState);
		//m_PlayerStatus.fCurrentHp -= m_pGameInstance->RandomValue(3.f, 10.f);
		m_PlayerStatus.fCurrentGuard += m_pGameInstance->RandomValue(3.f, 7.f);

		m_PlayerStatus.fCurrentGuard = clamp<float>(m_PlayerStatus.fCurrentGuard, 0.f, m_PlayerStatus.fMaxGuard);

		UI_MasterEvent	Event;
		Event.m_ActionName = "OnGuardEvent";
		Event.m_EventTarget = UI_EVENT_TARGET::UIOBJECT;
		Event.m_bFlag = true;
		//플레이어의 objectid도 보낸다(몬스터도 이 가드바를 사용하므로 구분이필요함)
		Event.m_fValue = m_pPlayer->Get_ObjectID();

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

	if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::RB))
	{

		UIObject* pInteraction_UI = m_pGameInstance->Find_PersistentUI_ByName(L"PopUp_Interaction");
		if (pInteraction_UI)
			pInteraction_UI->Set_Active(true);

	}

	return;
}
/******************************************************* 플레이어 스탯 테스트 함수 *******************************************************/


/******************************************************* UI 테스트 함수 *******************************************************/

#endif // _DEBUG



//////////////////////////////////////////////////////// 값 세팅함수 ////////////////////////////////////////////////////////
HRESULT Client::Level_Client::Ready_Values()
{
	

	UIObject* pShortCutMenu = m_pGameInstance->Find_PersistentUI_ByName(L"ShortCut_Menu");
	if (pShortCutMenu)
		pShortCutMenu->Set_Active(false);

	UIObject* pInteraction_UI = m_pGameInstance->Find_PersistentUI_ByName(L"PopUp_Interaction");
	if (pInteraction_UI)
		pInteraction_UI->Set_Active(false);



	UIObject* pPlayerHud = m_pGameInstance->Find_PersistentUI_ByName(L"PlayerHud_Left");
	if (pPlayerHud)
		pPlayerHud->Set_Active(true, true);

	UIObject* pPlayerMenu = m_pGameInstance->Find_PersistentUI_ByName(L"Window_PlayerMenu");
	if (pPlayerMenu)
		pPlayerMenu->Set_Active(false);

	UIObject* pWindow_Storage = m_pGameInstance->Find_PersistentUI_ByName(L"Window_Storage");
	if (pWindow_Storage)
		pWindow_Storage->Set_Active(false);


	UIObject* pBloodcode = m_pGameInstance->Find_PersistentUI_ByName(L"Window_Bloodcode");
	if (pBloodcode)
		pBloodcode->Set_Active(false);

	return S_OK;
}
/******************************************************* UI 테스트 함수 *******************************************************/
void Client::Level_Client::Publish_ExitEvent(_bool bFadeScreenExit, _bool bLoadingExit)
{
	//씬바뀜이벤트~
	if (bFadeScreenExit)
	{
		UIObj_FadeScreen::FadeScreenEvent Event;
		Event.m_fSecond = 3.f;
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
/******************************************************* UI 테스트 함수 *******************************************************/

//////////////////////////////////////////////////////// 세이브포인트와 텔포 ////////////////////////////////////////////////////////

void Client::Level_Client::Teleport_With_SavePoint()
{
	SAVE_POINT_INFO* pInfo = InteractionManager::GetInstance()->Get_LastSavePointInfo();
	if (pInfo)
	{
		Player* pPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
		if (pPlayer)
		{
			pPlayer->Teleport(pInfo->spawnPosition, pInfo->spawnRotation);
			//InteractionManager::GetInstance()->Reset_LastSavePointInfo();
		}
	}

	//InteractionManager::GetInstance()->Reset_LastSavePointInfo();
}
/*******************************************************  세이브포인트와 텔포*******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Level_Client* Client::Level_Client::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	/*Level_Client* pInstance = new Level_Client(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_level), L"Level_Client 원본 생성 실패", L"경고!!!", nullptr);*/

	return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Level_Client::Free()
{
	//m_pGameInstance->Delete_Light(0);

	__super::Free();

	//m_pGameInstance->UnsubScribe(m_iLevelChangeHandle);

	//if (m_pParsing_UI)	
	//	Safe_Release(m_pParsing_UI);

	if (m_pParsing_VFX)
		Safe_Release(m_pParsing_VFX);
	
	Safe_Release(m_pPoolingManager);

	//MinimapManager::DestroyInstance();

	m_pGameInstance->Clear_ColliderManager();
	m_pGameInstance->Clear_OcTree();
}
/******************************************************* 객체 반환 함수 *******************************************************/


