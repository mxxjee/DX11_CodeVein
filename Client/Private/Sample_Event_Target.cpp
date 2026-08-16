#include "Client_Define.h"
#include "Sample_Event_Target.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_Event_Target::Sample_Event_Target()
{
}

Client::Sample_Event_Target::Sample_Event_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Sample_Event_Parent(pDevice, pContext)
{
}

Client::Sample_Event_Target::Sample_Event_Target(const Sample_Event_Target& original)
	: Sample_Event_Parent(original)
{
}

Client::Sample_Event_Target::~Sample_Event_Target()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event_Target::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Sample_Event_Target::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Sample_Event_Target_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_iTargetID = TARGET_MONSTER;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event_Target::Ready_Components()
{
	// 이벤트 매니저에 이런 이벤트가 발생하면 나에게 알려달라고 구독
	m_iTestHandle = GameObject::Subscribe_Event<TestEvent>([this](const TestEvent& e) {
		// 만약 구독한 이벤트가 발생하면 여기로 진입
		// 발생한 이벤트중 Target이 같다면 이런 함수를 실행시키도록 함
		if (e.m_iTarget == m_iTargetID)
			TextRPG(e);
		});

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_Event_Target::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_Event_Target::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_Event_Target::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event_Target::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_Event_Target::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 테스트용 이벤트콜 함수 ////////////////////////////////////////////////////////
void Client::Sample_Event_Target::TextRPG(SAMPLE_EVENT _event)
{
	m_iHP -= _event.m_iDamage;

	COUT("Player가 Monster를 공격!\n몬스터의 남은 HP : " << m_iHP);
}
/******************************************************* 테스트용 이벤트콜 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_Event_Target* Client::Sample_Event_Target::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Sample_Event_Target* pInstance = new Sample_Event_Target(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Sample_Event_Target 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Sample_Event_Target::Clone(void* pArg)
{
	Sample_Event_Target* pInstance = new Sample_Event_Target(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Sample_Event_Target 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_Event_Target::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
