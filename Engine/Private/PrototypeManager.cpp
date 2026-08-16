#include "Engine_Define.h"
#include "PrototypeManager.h"
#include "GameObject.h"

Engine::PrototypeManager::PrototypeManager()
{
}

Engine::PrototypeManager::PrototypeManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::PrototypeManager::~PrototypeManager()
{
}

HRESULT Engine::PrototypeManager::Initialize(const _uint _levelMax)
{
	m_iLevelMax = _levelMax;
    
	m_umapPrototypes = new unordered_map<_wstring, class Base*>[m_iLevelMax];
	
	return S_OK;
}



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::PrototypeManager::Add_Prototype(_uint _levelID, const _wstring& prototypename, Base* pPrototype)
{
	if(m_iLevelMax <= _levelID)
	{	
		MSG_ON(L"레벨 ID가 범위를 초과했습니다.", L"Caution!!!");
		BREAK;
		return E_FAIL;
	}

	if(m_umapPrototypes[_levelID].contains(prototypename))
	{
		_wstring message = L"추가하려는 프로토타입\n" + prototypename + L"\n가 이미 존재합니다.";
		MSG_ON(message.c_str(), L"Caution!!!");
		BREAK;
		return E_FAIL;
	}

	// 프로토타입 이름을 저장
	if (DCAST(Component*)(pPrototype) != nullptr)
	{
		CAST(Component*)(pPrototype)->Set_PrototypeName(prototypename);
	}

	// 게임오브젝트라면 타입 이름도 세팅
	if (DCAST(GameObject*)(pPrototype) != nullptr)
	{
		CAST(GameObject*)(pPrototype)->Set_PrototypeName(prototypename);
		CAST(GameObject*)(pPrototype)->Set_TypeName();
	}



	m_umapPrototypes[_levelID].emplace(prototypename, pPrototype);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 프로토타입 복사 함수 ////////////////////////////////////////////////////////
Base* Engine::PrototypeManager::Clone_Prototype(const PROTOTYPE _prototypeID,const _uint _levelID, const _wstring& prototypename, void* pArg)
{
	Base* Prototype = Find_Prototype(prototypename, _levelID);

	CHECK_NULL_RESULT(Prototype, nullptr);

	if (_prototypeID == PROTOTYPE::GAMEOBJECT)
	{
		return CAST(GameObject*)(Prototype)->Clone(pArg);
	}
	else if (_prototypeID == PROTOTYPE::COMPONENT)
	{
		return CAST(Component*)(Prototype)->Clone(pArg);
	}

	return nullptr;
}
/******************************************************* 프로토타입 복사 함수 *******************************************************/



//////////////////////////////////////////////////////// 검색 함수 ////////////////////////////////////////////////////////
Base* Engine::PrototypeManager::Find_Prototype(const _wstring& _prototypeName, _uint _levelID) const
{
	if (m_iLevelMax <= _levelID)
	{
		MSG_ON(L"레벨 ID가 범위를 초과했습니다.", L"Caution!!!");
		BREAK;
		return nullptr;
	}

	auto iter = m_umapPrototypes[_levelID].find(_prototypeName);
	if (iter != m_umapPrototypes[_levelID].end())
		return iter->second;

	_wstring msg = _prototypeName + L"\n이라는 프로토타입이 없습니다.";
	MSG_ON(msg.c_str(), L"Caution!!!");
	UMAP<_wstring, class Base*> debug = m_umapPrototypes[_levelID];
	BREAK;

	return nullptr;
}
/******************************************************* 검색 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
HRESULT Engine::PrototypeManager::Clear_Level(_uint _levelID)
{
	for (auto& prototype : m_umapPrototypes[_levelID])
	{
		Safe_Release(prototype.second);
	}
	m_umapPrototypes[_levelID].clear();

	return S_OK;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
PrototypeManager* Engine::PrototypeManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint _levelMax)
{
	PrototypeManager* pInstance = new PrototypeManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_levelMax), L"PrototypeManager Create Failed", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::PrototypeManager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for(_uint i = 0; i < m_iLevelMax; ++i)
	{
		_uint check = 0;
		for (auto& pair : m_umapPrototypes[i])
		{
			Safe_Release(pair.second);
			++check;
		}
		m_umapPrototypes[i].clear();
	}

	Safe_Delete_Array(m_umapPrototypes);
}
/******************************************************* 객체 반환 함수 *******************************************************/
