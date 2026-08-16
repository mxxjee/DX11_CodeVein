#include "Engine_Define.h"
#include "Prototype_Class.h"

//#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Prototype_Class::Prototype_Class()
{
}

Engine::Prototype_Class::Prototype_Class(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)//, m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	//Safe_AddRef(m_pGameInstance);
}

Engine::Prototype_Class::Prototype_Class(const Prototype_Class& original)
	: m_pDevice(original.m_pDevice), m_pContext(original.m_pContext)//, m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	//Safe_AddRef(m_pGameInstance);
}

Engine::Prototype_Class::~Prototype_Class()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Prototype_Class::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::Prototype_Class::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Prototype_Class::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Prototype_Class::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Prototype_Class::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Prototype_Class::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Prototype_Class* Engine::Prototype_Class::Create()
{
	Prototype_Class* pInstance = new Prototype_Class();

	MSG_FAIL(pInstance->Initialize_Prototype(), L"Prototype_Class 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Prototype_Class* Engine::Prototype_Class::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Prototype_Class* pInstance = new Prototype_Class(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"Prototype_Class 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Prototype_Class* Engine::Prototype_Class::Clone(void* arg)
{
	Prototype_Class* pInstance = new Prototype_Class(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"Prototype_Class 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Prototype_Class::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	//Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

