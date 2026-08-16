#include "Engine_Define.h"
#include "LevelManager.h"
#include "Level.h"

#include "GameInstance.h"
#include "GameObjectManager.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::LevelManager::LevelManager()
{
}

Engine::LevelManager::LevelManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::LevelManager::~LevelManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Engine::LevelManager::Initialize(_uint _levelcount, GameObjectManager* _gameobjectmanaer)
{
	/* 레벨의 갯수를 받아와서 멤버변수에 등록
	/* vector컨테이너의 내부를 밀어버리고 레벨 갯수만큼 nullptr로 채워넣기 */
	m_iLevelCount = _levelcount;

	m_vecLevels.assign(m_iLevelCount, nullptr);

	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	m_pGameObjectManager = _gameobjectmanaer;
	Safe_AddRef(m_pGameObjectManager);
	
	return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::LevelManager::Add_Level(_uint _levelname, Level* level)
{
	if (m_vecLevels[_levelname] != nullptr)
	{
		MSG_ON(L"추가하려는 레벨이 이미 존재합니다.", L"레벨 추가 실패");
		return E_FAIL;
	}
	else if (m_iLevelCount <= _levelname)
	{
		MSG_ON(L"상정한 레벨 범위 밖입니다.", L"레벨 추가 실패");
		return E_FAIL;
	}

	m_vecLevels[_levelname] = level;

	return S_OK;
}
HRESULT Engine::LevelManager::Add_PlayerLevel(Level* level)
{
	m_pPlayerLevel = level;
	//Safe_AddRef(m_pPlayerLevel); 누수

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 레벨 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::LevelManager::Change_Level(_uint _levelname)
{
	/* 첫 번째 레벨이 아닐 경우 */
	if ((m_pCurrentLevel != nullptr))
	{
		Safe_Release(m_pCurrentLevel);

		/* 삭제해도 되는 레벨일 경우 */
		if(m_pCurrentLevel->IsClearLevel())
		{
			CHECK_FAILED(Delete_Level(m_iCurrentLevelID), E_FAIL);
		}
	}

	m_pCurrentLevel = m_vecLevels[_levelname];

	Safe_AddRef(m_pCurrentLevel);

	m_iCurrentLevelID = _levelname;

	m_pCurrentLevel->Initialize_Level();

	m_pGameObjectManager->Change_Level(m_iCurrentLevelID);

	return S_OK;
}

HRESULT Engine::LevelManager::Delete_Level(_uint _levelname)
{
	Safe_Release(m_vecLevels[_levelname]);

	if (m_vecLevels[_levelname] != nullptr)
	{
		MSG_ON(L"레벨이 정상적으로 삭제되지 않았습니다.", L"레벨 삭제 실패");
		return E_FAIL;
	}

	return S_OK;
}
/******************************************************* 레벨 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트, 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::LevelManager::Update_Priority(const _float fTimeDelta)
{
    if (!m_pCurrentLevel)
        return E_FAIL;

	m_pCurrentLevel->Update_Priority(fTimeDelta);

	return S_OK;
}

HRESULT Engine::LevelManager::Update(const _float fTimeDelta)
{
    if (!m_pCurrentLevel)
        return E_FAIL;

	m_pCurrentLevel->Update(fTimeDelta);

	return S_OK;
}

HRESULT Engine::LevelManager::Update_Late(const _float fTimeDelta)
{
    if (!m_pCurrentLevel)
        return E_FAIL;

	m_pCurrentLevel->Update_Late(fTimeDelta);

	return S_OK;
}

HRESULT Engine::LevelManager::Render_Level(const _float fTimeDelta)
{
    if (!m_pCurrentLevel)
        return E_FAIL;

	m_pCurrentLevel->Render(fTimeDelta);

	return S_OK;
}
HRESULT Engine::LevelManager::Update_Player_Priority(const _float fTimeDelta)
{
	if (m_pPlayerLevel == nullptr)
		return E_FAIL;

	m_pPlayerLevel->Update_Priority(fTimeDelta);

	return S_OK;
}
HRESULT Engine::LevelManager::Update_Player(const _float fTimeDelta)
{
	if (m_pPlayerLevel == nullptr)
		return E_FAIL;

	m_pPlayerLevel->Update(fTimeDelta);

	return S_OK;
}
HRESULT Engine::LevelManager::Update_Player_Late(const _float fTimeDelta)
{
	if (m_pPlayerLevel == nullptr)
		return E_FAIL;

	m_pPlayerLevel->Update_Late(fTimeDelta);

	return S_OK;
}
HRESULT Engine::LevelManager::Render_Player(const _float fTimeDelta)
{
	if (m_pPlayerLevel == nullptr)
		return E_FAIL;

	m_pPlayerLevel->Render(fTimeDelta);

	return S_OK;
}
/******************************************************* 업데이트, 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
LevelManager* Engine::LevelManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _levelcount, GameObjectManager* _gameobjectmanaer)
{
	LevelManager* pInstance = new LevelManager();

	MSG_FAIL(pInstance->Initialize(_levelcount, _gameobjectmanaer), L"LevelManager Create Failed", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::LevelManager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& level : m_vecLevels)
	{
		Safe_Release(level);
	}
	m_vecLevels.clear();

	Safe_Release(m_pCurrentLevel);
	Safe_Release(m_pPlayerLevel);
	Safe_Release(m_pGameObjectManager);
	
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

