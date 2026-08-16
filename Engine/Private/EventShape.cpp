#include "Engine_Define.h"
#include "EventShape.h"

#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::EventShape::EventShape()
{
}

Engine::EventShape::EventShape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

Engine::EventShape::EventShape(const EventShape& original)
	: GameObject(original)
{
}

Engine::EventShape::~EventShape()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::EventShape::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::EventShape::Initialize(void* arg)
{
	m_eObjType = OBJTYPE::TYPE_EVENT;

	if (arg != nullptr)
	{
		CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);
	}


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::EventShape::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::EventShape::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::EventShape::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::EventShape::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
EventShape* Engine::EventShape::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//EventShape* pInstance = new EventShape(pDevice, pContext);

	//MSG_FAIL(pInstance->Initialize_Prototype(), L"EventShape 생성 실패", L"Caution!!!", nullptr);

	return nullptr;
}

EventShape* Engine::EventShape::Clone(void* arg)
{
	//EventShape* pInstance = new EventShape(*this);

	//MSG_FAIL(pInstance->Initialize(arg), L"EventShape 복사 실패", L"Caution!!!", nullptr);

	return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::EventShape::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/


