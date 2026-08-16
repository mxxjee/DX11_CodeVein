#include "Engine_Define.h"
#include "SpawnManager.h"

#include "GameInstance.h"
#include "GameObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::SpawnManager::SpawnManager()
{
}

Engine::SpawnManager::SpawnManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}
Engine::SpawnManager::~SpawnManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::SpawnManager::Initialize()
{
	m_pGameInstance->Subscribe<SPAWN_EVENT>([this](const SPAWN_EVENT& _event) {
		if (_event.wstrPrototypeName == L"" || _event.wstrLayerName == L"")
		{
			COUT("소환할 프로토타입명 또는 레이어 이름이 비어있음");
			return;
		}

		Spawn_Character(_event);

		});

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
HRESULT Engine::SpawnManager::Spawn_Character(const SPAWN_EVENT& _event)
{
	_uint levelID = m_pGameInstance->Get_Current_LevelID();

	GameObject::GAMEOBJECT_DESC desc;
	desc.vPosition = { _event.vSpawnPoint.x, _event.vSpawnPoint.y, _event.vSpawnPoint.z, 1.f };

	m_pGameInstance->Add_GameObject_To_Layer(levelID, _event.wstrPrototypeName, levelID, _event.wstrLayerName, nullptr, &desc);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
SpawnManager* Engine::SpawnManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	SpawnManager* pInstance = new SpawnManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(), L"SpawnManager 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::SpawnManager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

