#include "Engine_Define.h"
#include "GameObjectManager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include <execution>

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::GameObjectManager::GameObjectManager()
{
}

Engine::GameObjectManager::GameObjectManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::GameObjectManager::~GameObjectManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/





//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameObjectManager::Initialize(const _uint _levelMax)
{
	m_iLevelMax = _levelMax;

	m_umapLayers = new unordered_map<_wstring, class Layer*>[m_iLevelMax];

	m_pGameInstance = GameInstance::GetInstance();

	EventHandle handle = m_pGameInstance->Subscribe<DEAD_EVENT>([this](const DEAD_EVENT& _event)
		{
			m_bDeadObjectIs = _event.bDeadObject;
		});

	m_vecEvent.push_back(handle);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 오브젝트 관리 함수 ////////////////////////////////////////////////////////
// 레이어가 생성될 때 오브젝트가 파트오브젝트면 해당 레이어는 업데이트를 안 함
HRESULT GameObjectManager::Add_GameObject(const _uint _prototypeLevelID, const _wstring& _prototypename, const _uint _levelID, const _wstring& _layername,
    GameObject** _outobject, void* pArg)
{
	if (_levelID >= m_iLevelMax)
	{
		MSG_ON(L"레벨 ID가 범위를 초과했습니다.", L"Caution!!!");
		return E_FAIL;
	}

	GameObject* pgameobject = CAST(GameObject*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _prototypeLevelID, _prototypename, pArg));

	CHECK_NULLPTR(pgameobject);

	// 레이어가 없다면 생성
	if (!m_umapLayers[_levelID].contains(_layername))
	{
		Layer* newLayer = Layer::Create(m_pDevice, m_pContext);
		newLayer->Set_Name(_layername);
		m_umapLayers[_levelID].emplace(_layername, newLayer);

		if (pgameobject->Is_PartObj() == true)
		{
			m_umapLayers[_levelID][_layername]->Set_PartObjLayer(true);
		}
	}

    if(_outobject)
        *_outobject = pgameobject;

    pgameobject->Set_PrototypeName(_prototypename);

	m_umapLayers[_levelID][_layername]->Add_GameObject(pgameobject);

	return S_OK;
}

HRESULT Engine::GameObjectManager::Add_GameObject(GameObject* pGameObject, const _uint _levelID, const _wstring& _layername)
{
	CHECK_NULL_RESULT(pGameObject, E_FAIL);
	if (!m_umapLayers[_levelID].contains(_layername))
	{
		Layer* newLayer = Layer::Create(m_pDevice, m_pContext);
		newLayer->Set_Name(_layername);
		m_umapLayers[_levelID].emplace(_layername, newLayer);

		if (pGameObject->Is_PartObj() == true)
		{
			m_umapLayers[_levelID][_layername]->Set_PartObjLayer(true);
		}
	}

	m_umapLayers[_levelID][_layername]->Add_GameObject(pGameObject);

	return S_OK;
}

GameObject* GameObjectManager::Get_GameObject(const _uint _levelID, const _wstring& _layername, const _wstring& _gameobjectname) const
{
	return m_umapLayers[_levelID][_layername]->Get_GameObject(_gameobjectname);
}

HRESULT Engine::GameObjectManager::Change_Level(const _uint _levelID)
{
	m_iCurrentLevel = _levelID;

	return S_OK;
}
/******************************************************* 오브젝트 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 정리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameObjectManager::Clear_Level(_uint _levelID)
{
	for (auto& layer : m_umapLayers[_levelID])
	{
		Safe_Release(layer.second);
	}
	m_umapLayers[_levelID].clear();

	return S_OK;
}

void Engine::GameObjectManager::Delete_DeadGameObject()
{
	if (!m_bDeadObjectIs)
		return;

    for (_uint i = 0; i < m_iLevelMax; ++i)
    {
        for (auto& layer : m_umapLayers[i])
        {
            layer.second->Delete_DeadGameObject();
        }
    }
    if (m_bDeadObjectIs)
    {
        m_pGameInstance->Delete_DeadUI();
    }
	m_bDeadObjectIs = false;
}

void Engine::GameObjectManager::Set_Parallel_Dirty()
{
	m_bParallelDirty = true;
}
/******************************************************* 컨테이너 정리 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::GameObjectManager::Update_Priority(const _float fTimeDelta)
{
	for (auto& layer : m_umapLayers[m_iCurrentLevel])
	{
		if (layer.second->Is_PartObjLayer() == true)
			continue;

		layer.second->Update_Priority(fTimeDelta);
	}

	if(g_bClient)
	{
		for (auto& layer : m_umapLayers[_UINT(8)])
		{
			if (layer.second->Is_PartObjLayer() == true)
				continue;

			layer.second->Update_Priority(fTimeDelta);
		}
	}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::GameObjectManager::Update_Parallel(const _float fTimeDelta)
{
	if (m_bParallelDirty)
	{
		m_vecParallelTargets.clear();

		auto Gather = [&](auto& umapLayer)
			{
				for (auto& layer : umapLayer)
				{
					if (layer.second->Is_PartObjLayer())
						continue;

					auto& vec = layer.second->Get_GameObjects_Vector();
					for (auto& obj : vec)
						m_vecParallelTargets.emplace_back(obj);
				}
			};

		Gather(m_umapLayers[m_iCurrentLevel]);

		if (g_bClient)
			Gather(m_umapLayers[_UINT(8)]);

		m_bParallelDirty = false;
	}

	std::for_each(std::execution::par,
		m_vecParallelTargets.begin(),
		m_vecParallelTargets.end(),
		[fTimeDelta](GameObject* _obj)
		{
			_obj->Update_Parallel(fTimeDelta);
		});

	return 0;
}

_int Engine::GameObjectManager::Update(const _float fTimeDelta)
{
	for (auto& layer : m_umapLayers[m_iCurrentLevel])
	{
		if (layer.second->Is_PartObjLayer() == true)
			continue;

		layer.second->Update(fTimeDelta);
	}

	if (g_bClient)
	{
		for (auto& layer : m_umapLayers[_UINT(8)])
		{
			if (layer.second->Is_PartObjLayer() == true)
				continue;

			layer.second->Update(fTimeDelta);
		}
	}


	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::GameObjectManager::Update_Late(const _float fTimeDelta)
{
	for (auto& layer : m_umapLayers[m_iCurrentLevel])
	{
		if (layer.second->Is_PartObjLayer() == true)
			continue;

		layer.second->Update_Late(fTimeDelta);
	}

	if (g_bClient)
	{
		for (auto& layer : m_umapLayers[_UINT(8)])
		{
			if (layer.second->Is_PartObjLayer() == true)
				continue;

			layer.second->Update_Late(fTimeDelta);
		}
	}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameObjectManager::Render(const _float fTimeDelta)
{
	for (auto& layer : m_umapLayers[m_iCurrentLevel])
	{
		if (layer.second->Is_PartObjLayer() == true)
			continue;

		layer.second->Render(fTimeDelta);
	}

	if (g_bClient)
	{
		for (auto& layer : m_umapLayers[_UINT(8)])
		{
			if (layer.second->Is_PartObjLayer() == true)
				continue;

			layer.second->Render(fTimeDelta);
		}
	}


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GameObjectManager* Engine::GameObjectManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint _levelMax)
{
	GameObjectManager* pInstance = new GameObjectManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_levelMax), L"GameObjectManager Create Failed", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::GameObjectManager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (_uint i = 0; i < m_iLevelMax; ++i)
	{
		for (auto& pair : m_umapLayers[i])
		{
			Safe_Release(pair.second);
		}
		m_umapLayers[i].clear();
	}

	for (auto& event : m_vecEvent)
	{
		m_pGameInstance->UnsubScribe(event);
	}
	
	Safe_Delete_Array(m_umapLayers);

}
/******************************************************* 객체 반환 함수 *******************************************************/

