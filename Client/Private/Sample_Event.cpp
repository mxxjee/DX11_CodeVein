#include "Client_Define.h"
#include "Sample_Event.h"

#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_Event::Sample_Event()
{
}

Client::Sample_Event::Sample_Event(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Sample_Event_Parent(pDevice, pContext)
{
}

Client::Sample_Event::Sample_Event(const Sample_Event& original)
	: Sample_Event_Parent(original)
{
}

Client::Sample_Event::~Sample_Event()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Sample_Event::Initialize(void* arg)
{
	static _uint namenum = 0;

	GAMEOBJECT_DESC desc;

	if (arg == nullptr)
	{
		arg = &desc;
	}
	else
	{

	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Sample_Event_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_iTargetID = TARGET_PLAYER;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event::Ready_Components()
{


	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_Event::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_Event::Update(const _float fTimeDelta)
{
	// 스페이스바 누르면
 	if (m_pGameInstance->KeyDown(DIK_SPACE))
	{
		// 이벤트 매니저에 이런 이벤트를 발생시키겠다고 전달
		TestEvent event;
		event.m_iDamage = 5;			// 줄 데미지
		event.m_iSource = m_iTargetID;	// 데미지를 주는 주체
		event.m_iTarget = TARGET_MONSTER;	// 데미지를 받는 주체, 또는 그룹(당신이 원하는대로 설정)

		// 이벤트 매니저에 전달
		m_pGameInstance->Publish(event);
	}

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_Event::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_Event* Client::Sample_Event::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Sample_Event* pInstance = new Sample_Event(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Sample_Event 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Sample_Event::Clone(void* pArg)
{
	Sample_Event* pInstance = new Sample_Event(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Sample_Event 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_Event::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
