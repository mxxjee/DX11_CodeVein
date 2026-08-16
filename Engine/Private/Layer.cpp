#include "Engine_Define.h"
#include "Layer.h"

#include "GameObject.h"

#include <execution>

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Layer::Layer()
{
}

Engine::Layer::Layer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::Layer::~Layer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Layer::Initialize()
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Layer::Add_GameObject(GameObject* pGameObject)
{
	_wstring gameobjectname = pGameObject->Get_Name();

	if(m_umapGameObjects.contains(gameobjectname))
	{
		_wstring message = L"추가하려는 게임 오브젝트 : \n" + gameobjectname + L"\n가 이미 존재합니다.";
		MSG_ON(message.c_str(), L"게임 오브젝트 추가 실패");
        BREAK;
		return E_FAIL;
	}

	pGameObject->Set_Layer(this);
	m_umapGameObjects.emplace(gameobjectname, pGameObject);
	m_vecGameObjects.push_back(pGameObject);

	return S_OK;
}

GameObject* Engine::Layer::Get_GameObject(const _wstring& gameobjectname) const
{
	if(m_umapGameObjects.contains(gameobjectname))
	{
		return m_umapGameObjects.at(gameobjectname);
	}

	_wstring message = L"찾으려는 게임 오브젝트 : \n" + gameobjectname + L"\n가 존재하지 않습니다.";
	MSG_ON(message.c_str(), L"게임 오브젝트 검색 실패");
    BREAK;

	return nullptr;
}

_bool Engine::Layer::Delete_DeadGameObject()
{
    _bool isDeadObjectExist = false;
    /*for (auto iter = m_umapGameObjects.begin(); iter != m_umapGameObjects.end();)
    {
        if ((*iter).second->Is_Dead())
        {
            Safe_Release((*iter).second);
            iter = m_umapGameObjects.erase(iter);
            isDeadObjectExist = true;
        }
        else
        {
            ++iter;
        }
    }*/

	for (auto iter = m_vecGameObjects.begin(); iter != m_vecGameObjects.end();)
	{
		if ((*iter)->Is_Dead())
		{
			(*iter)->OnDestroyInLayer();	//진짜로 레이어에서 제거됨을알림
			m_umapGameObjects.erase((*iter)->Get_Name());
			Safe_Release((*iter));
			iter = m_vecGameObjects.erase(iter);

			isDeadObjectExist = true;
		}
		else
		{
			++iter;
		}
	}

    return isDeadObjectExist;
}

//UI툴에서 부모-자식관계를 동적으로 제어하다보니 추가된함수..
_bool Engine::Layer::Remove_GameObject(GameObject* pGameObject)
{
	// 1. Map에서 제거 (이름 기반 검색)
	auto iterMap = m_umapGameObjects.find(pGameObject->Get_Name());
	if (iterMap != m_umapGameObjects.end())
	{
		m_umapGameObjects.erase(iterMap);
	}  
	 
	// 2. Vector에서 제거 (포인터 비교)
	auto iterVec = find(m_vecGameObjects.begin(), m_vecGameObjects.end(), pGameObject);
	if (iterVec != m_vecGameObjects.end())
	{
		m_vecGameObjects.erase(iterVec);
	}

	return S_OK;
}
/******************************************************* 컨테이너 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Layer::Update_Priority(const _float fTimeDelta)
{
	if (m_bIsPartObjLayer)
		return 0;

	for (auto& object : m_vecGameObjects)
	{
		object->Update_Priority(fTimeDelta);
	}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Layer::Update_Parallel(const _float fTimeDelta)
{
	if (m_bIsPartObjLayer)
		return 0;

    std::for_each(std::execution::par,
		m_vecGameObjects.begin(),
		m_vecGameObjects.end(),
        [fTimeDelta](auto& pair)
        {
            pair->Update_Parallel(fTimeDelta);
        });

    return 0;
}

_int Engine::Layer::Update(const _float fTimeDelta)
{
	if (m_bIsPartObjLayer)
		return 0;

	for (auto& object : m_vecGameObjects)
	{
		object->Update(fTimeDelta);
	}

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Layer::Update_Late(const _float fTimeDelta)
{
	if (m_bIsPartObjLayer)
		return 0;

	for (auto& object : m_vecGameObjects)
	{
		object->Update_Late(fTimeDelta);
	}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Layer::Render(const _float fTimeDelta)
{

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Layer* Engine::Layer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Layer* pInstance = new Layer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(), L"Layer Create Failed", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Layer::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& gameobject : m_umapGameObjects)
	{
		Safe_Release(gameobject.second);
	}
	m_umapGameObjects.clear();
	m_vecGameObjects.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/

