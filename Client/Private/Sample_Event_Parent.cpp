#include "Client_Define.h"
#include "Sample_Event_Parent.h"

#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_Event_Parent::Sample_Event_Parent()
{
}

Client::Sample_Event_Parent::Sample_Event_Parent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

Client::Sample_Event_Parent::Sample_Event_Parent(const Sample_Event_Parent& original)
	: GameObject(original)
{
}

Client::Sample_Event_Parent::~Sample_Event_Parent()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event_Parent::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Sample_Event_Parent::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_Event_Parent* Client::Sample_Event_Parent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_Event_Parent::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
