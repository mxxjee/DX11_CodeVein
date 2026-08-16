#include "Engine_Define.h"
#include "Empty.h"

#include "GameInstance.h"
#include "StructuredBuffer.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Empty::Empty()
{
}

Engine::Empty::Empty(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

Engine::Empty::Empty(const Empty& original)
	: m_pDevice(original.m_pDevice), m_pContext(original.m_pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

Engine::Empty::~Empty()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Empty::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::Empty::Initialize(void* arg)
{
	m_pTransformCom = Transform::Create(m_pDevice, m_pContext);
	CHECK_NULLPTR(m_pTransformCom);
	m_umapComponents.emplace(Com_Transform, m_pTransformCom);

	if (arg != nullptr)
	{
		EMPTY_DESC* desc = CAST(EMPTY_DESC*)(arg);

		m_wstrName = desc->wstrName;
		m_wstrShaderName = desc->wstrShaderName;

		m_pTransformCom->Initialize(arg);
	}



	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Empty::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Empty::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Empty::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Empty::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Empty::Add_Component(_uint prototypelevelID, const _wstring& prototypename, const _wstring& componentname, Component** outcomponent, void* arg)
{
	if (m_umapComponents.contains(componentname))
	{
		_wstring message = componentname + L"컴포넌트가 이미 존재합니다.";
		_wstring caption = Get_Name() + L"오브젝트 컴포넌트 추가 실패";
		MSG_ON(message.c_str(), caption.c_str());
		BREAK;
		return E_FAIL;
	}

	*outcomponent = CAST(Component*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, prototypelevelID, prototypename, arg));

	MSG_NULL(*outcomponent, L"컴포넌트가 nullptr입니다.", L"오류!!!", E_FAIL);

	m_umapComponents.emplace(componentname, *outcomponent);

	// 쉐이더라면 쉐이더 이름을 저장
	if (DCAST(Shader*)(*outcomponent) != nullptr)
	{
		m_wstrShaderName = prototypename;
	}

	return S_OK;
}

HRESULT Engine::Empty::Add_StructuredBuffer(StructuredBuffer*& _buffer, SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData)
{
	_buffer = StructuredBuffer::Create(m_pDevice, m_pContext, _usage, _numElements, _stride, _initialData);
	m_vecCleanUpTargets.push_back(_buffer);
	return S_OK;
}

HRESULT Engine::Empty::Add_StructuredBuffer(SB_DESC& _desc)
{
	return Add_StructuredBuffer(_desc.pBuffer, _desc.eUsage, _desc.iNumElements, _desc.iStride, _desc.initialData);
}

Component* Engine::Empty::Get_Component_FromName(const _wstring& _componentName)
{
	if (!m_umapComponents.contains(_componentName))
	{
		//MSG_ON((_componentName + L" 컴포넌트가 없습니다.").c_str(), L"컴포넌트 검색 실패");
		//BREAK;
		return nullptr;
	}

	return m_umapComponents[_componentName];
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 변수 겟 함수 ////////////////////////////////////////////////////////
_float3 Engine::Empty::Get_Scale()
{
	return m_pTransformCom->Get_Scale();
}

_float Engine::Empty::Get_Speed()
{
	return m_pTransformCom->Get_Speed();
}
/******************************************************* 변수 겟 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Empty* Engine::Empty::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//Empty* pInstance = new Empty(pDevice, pContext);

	//MSG_FAIL(pInstance->Initialize_Prototype(), L"Empty 생성 실패", L"Caution!!!", nullptr);

	return nullptr;
}

Empty* Engine::Empty::Clone(void* arg)
{
	//Empty* pInstance = new Empty(*this);

	//MSG_FAIL(pInstance->Initialize(arg), L"Empty 복사 실패", L"Caution!!!", nullptr);

	return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Empty::Free()
{
	__super::Free();

	// 컴포넌트 전부 해제
	for (auto& component : m_umapComponents)
	{
		Safe_Release(component.second);
	}
	m_umapComponents.clear();

	// 아무튼 등록된거 다 해제
	for (auto& base : m_vecCleanUpTargets)
	{
		Safe_Release(base);
	}
	m_vecCleanUpTargets.clear();

	// 구독목록 전부 해제
	for (auto& handle : m_vecSubscribeNumbers)
	{
		m_pGameInstance->UnsubScribe(handle);
	}
	m_vecSubscribeNumbers.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

