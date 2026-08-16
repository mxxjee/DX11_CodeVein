#include "Engine_Define.h"
#include "Component.h"

#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Component::Component()
{
}

Engine::Component::Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

Engine::Component::Component(const Component& original)
	: m_pDevice(original.m_pDevice), m_pContext(original.m_pContext), m_pGameInstance(original.m_pGameInstance), m_wstrPrototypeName(original.m_wstrPrototypeName), m_iSavePriority(original.m_iSavePriority)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
	m_bIsClone = true;
}

Engine::Component::~Component()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Component::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::Component::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Component::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Component::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Component::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Component::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 디버그 렌더 함수 ////////////////////////////////////////////////////////
#ifdef _DEBUG
void Engine::Component::Add_DebugRender()
{
	m_pGameInstance->Add_DebugComponent(this);
}
#endif // _DEBUG
/******************************************************* 디버그 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Component* Engine::Component::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	/*Component* pInstance = new Component(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"Component 생성 실패", L"Caution!!!", nullptr);*/

	return nullptr;
}

Component* Engine::Component::Clone(void* arg)
{
	/*Component* pInstance = new Component(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"Component 생성 실패", L"Caution!!!", nullptr);*/

	return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Component::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

