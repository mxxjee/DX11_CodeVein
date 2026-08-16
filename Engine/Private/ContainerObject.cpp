#include "Engine_Define.h"
#include "ContainerObject.h"

#include "GameInstance.h"
#include "PartObject.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::ContainerObject::ContainerObject()
{
}

Engine::ContainerObject::ContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Character(pDevice, pContext)
{
    m_eObjType = OBJTYPE::TYPE_CONTAINER;
}

Engine::ContainerObject::ContainerObject(const ContainerObject& original)
    : Character(original)
{
    m_eObjType = OBJTYPE::TYPE_CONTAINER;
}

Engine::ContainerObject::~ContainerObject()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ContainerObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::ContainerObject::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ContainerObject::Add_PartObject(_uint _prototypeLevelIndex, const _wstring& _prototypeName, const _wstring& _partOBJName, void* arg, _bool _addRef)
{
    if (m_umapPartObjects.contains(_partOBJName))
    {
        MSG_ON((L"이미 " + _partOBJName + L"이라는 파츠가 존재합니다.").c_str(), L"파츠 추가 실패");
        BREAK;
        return E_FAIL;
    }

    PartObject* partObj = nullptr;// = CAST(PartObject*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _prototypeLevelIndex, _prototypeName, arg));
    m_pGameInstance->Add_GameObject_To_Layer(m_iLevel, _prototypeName, m_iLevel, L"Layer_PartObject", RCAST(GameObject**)(&partObj), arg);

    m_umapPartObjects.emplace(_partOBJName, partObj);
    if (_addRef == true)
    {
        Safe_AddRef(partObj);
    }

    return S_OK;
}

PartObject* Engine::ContainerObject::Find_PartObject(const _wstring& _partOBJName)
{
    if (m_umapPartObjects.contains(_partOBJName))
    {
        return m_umapPartObjects[_partOBJName];
    }

    MSG_ON((_partOBJName + L"이라는 파츠는 존재하지 않습니다").c_str(), L"파츠 추가 실패");
    BREAK;

    return nullptr;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ContainerObject::Update_Priority(const _float fTimeDelta)
{
    for (auto iter = m_umapPartObjects.begin(); iter != m_umapPartObjects.end();)
    {
        if((*iter).second->Get_IsPartActive()==true)
        (*iter).second->Update_Priority(fTimeDelta);
        ++iter;
    }

    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/

_int Engine::ContainerObject::Update_Parallel(const _float fTimeDelta)
{
    for (auto iter = m_umapPartObjects.begin(); iter != m_umapPartObjects.end();)
    {
        if ((*iter).second->Get_IsPartActive() == true)
        (*iter).second->Update_Parallel(fTimeDelta);
        ++iter;
    }

    return 0;
}


////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ContainerObject::Update(const _float fTimeDelta)
{
    for (auto iter = m_umapPartObjects.begin(); iter != m_umapPartObjects.end();)
    {
        if ((*iter).second->Get_IsPartActive() == true)
      (*iter).second->Update(fTimeDelta);
        ++iter;
    }

    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ContainerObject::Update_Late(const _float fTimeDelta)
{
    for (auto iter = m_umapPartObjects.begin(); iter != m_umapPartObjects.end();)
    {
        if ((*iter).second->Get_IsPartActive() == true)
        (*iter).second->Update_Late(fTimeDelta);
        ++iter;
    }

    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ContainerObject::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
ContainerObject* Engine::ContainerObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

GameObject* Engine::ContainerObject::Clone(void* arg)
{
    return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::ContainerObject::Free()
{
    __super::Free();

    for (auto& partobj : m_umapPartObjects)
    {
        Safe_Release(partobj.second);
    }
    m_umapPartObjects.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/

