#include "Client_Define.h"
#include "Sample_StructuredBuffer.h"

//#include "GameInstance.h"
#include "StructuredBuffer.h"
#include "Shader.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_StructuredBuffer::Sample_StructuredBuffer()
{
}

Client::Sample_StructuredBuffer::Sample_StructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

Client::Sample_StructuredBuffer::Sample_StructuredBuffer(const Sample_StructuredBuffer& original)
	: GameObject(original)
{
}

Client::Sample_StructuredBuffer::~Sample_StructuredBuffer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_StructuredBuffer::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Sample_StructuredBuffer::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Sample_StructuredBuffer_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_StructuredBuffer::Ready_Components()
{
	vector<_float> vecElements;
	_uint number = 100;
	vecElements.resize(number);

	for (_uint i = 0; i < number; ++i)
	{
		vecElements[i] = i;
	}

	m_pSB = StructuredBuffer::Create(m_pDevice, m_pContext, SBUSAGE::DYNAMIC, number, sizeof(_float), vecElements.data());
	m_pSB2 = StructuredBuffer::Create(m_pDevice, m_pContext, SBUSAGE::IMMUTABLE, number, sizeof(_float), vecElements.data());
	m_pSB3 = StructuredBuffer::Create(m_pDevice, m_pContext, SBUSAGE::GPU_READWRITE, number, sizeof(_float), vecElements.data());
	
	// 또는

	GameObject::Add_StructuredBuffer(m_pSB, SBUSAGE::DYNAMIC, number, sizeof(_float), vecElements.data());

	SB_DESC sbdesc;
	sbdesc.pBuffer = m_pSB2;
	sbdesc.eUsage = SBUSAGE::GPU_READWRITE;
	sbdesc.iNumElements = number;
	sbdesc.iStride = sizeof(_float);
	sbdesc.initialData = vecElements.data();

	GameObject::Add_StructuredBuffer(sbdesc);

	// 이렇게 하면 자동으로 SB도 해제해줌(Safe_Release(m_pSB) 안 해도 됨)

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_StructuredBuffer::Update(const _float fTimeDelta)
{
	// 바인드 할때는 이렇게
	m_pShaderCom->Bind_SRV_FullSlot(0, m_pSB->Get_SRV());
	m_pCSCom->Bind_SRV_FullSlot(0, m_pSB2->Get_SRV());
	m_pCSCom->Bind_UAV_FullSlot(0, m_pSB3->Get_UAV());

	// 데이터 받아올때는 이렇게(SBUSAGE::DYNAMIC, GPU_READWRITE전용)
	vector<_float> vecElements(m_pSB3->Get_NumElements());	// 크기 미리 할당
	m_pSB3->ReadBack(vecElements.data(), m_pSB3->Get_ByteWidth());	// 데이터 받아오기

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_StructuredBuffer::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_StructuredBuffer::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_StructuredBuffer* Client::Sample_StructuredBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Sample_StructuredBuffer* pInstance = new Sample_StructuredBuffer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Sample_StructuredBuffer 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Sample_StructuredBuffer::Clone(void* pArg)
{
	Sample_StructuredBuffer* pInstance = new Sample_StructuredBuffer(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Sample_StructuredBuffer 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_StructuredBuffer::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
