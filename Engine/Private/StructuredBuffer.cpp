#include "Engine_Define.h"
#include "StructuredBuffer.h"

//#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::StructuredBuffer::StructuredBuffer()
{
}

Engine::StructuredBuffer::StructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)//, m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	//Safe_AddRef(m_pGameInstance);
}

Engine::StructuredBuffer::~StructuredBuffer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::StructuredBuffer::Initialize(SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData)
{
	// 데이터 등록
	m_eUsage = _usage;
	m_iStride = _stride;
	m_iNumElements = _numElements;
	m_iByteWidth = m_iStride * m_iNumElements;

	// 버퍼 생성용 정의
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = m_iByteWidth;			// 전체 버퍼의 크기
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 구조체 버퍼로 생성할것
	desc.StructureByteStride = m_iStride;	// 데이터 한 뭉치 크기(1byte, 2byte, 4byte ... )

	// 사용 방식에 따른 버퍼 정의 분류
	switch (_usage)
	{
	case SBUSAGE::IMMUTABLE:
		// 읽기 전용 버퍼
		// 읽기 전용이기 때문에 초기 데이터가 반드시 필요함
		if (_initialData == nullptr)
		{
			MSG_ON(L"IMMUTABLE 모드에서 초기 데이터가 없습니다.", L"생성 실패!!");
			BREAK;
			return E_FAIL;
		}
		desc.Usage = D3D11_USAGE_IMMUTABLE;				// 버퍼 사용 방식 결정
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// SRV로 사용하는 버퍼
		desc.CPUAccessFlags = 0;						// 0 : CPU가 직접 접근할 수 없음(GPU전용)
		break;

	case SBUSAGE::DYNAMIC:
		desc.Usage = D3D11_USAGE_DYNAMIC;				// 쓰기 가능한 버퍼
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// SRV로 사용할것
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU가 데이터를 써서 GPU로 전달 가능
		break;

	case SBUSAGE::GPU_READWRITE:
		desc.Usage = D3D11_USAGE_DEFAULT;				// 읽고 쓰기 모두 되는 버퍼(대신 비교적 좀 느림)
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;	// SRV와 UAV로 모두 사용할것
		desc.CPUAccessFlags = 0;						// 0 : CPU가 직접 접근할 수 없음(GPU전용)
		break;
	}

	D3D11_SUBRESOURCE_DATA initData = {};
	D3D11_SUBRESOURCE_DATA* pInitData = { nullptr };

	if (_initialData != nullptr)
	{
		initData.pSysMem = _initialData;
		pInitData = &initData;
	}

	MSG_FAIL(m_pDevice->CreateBuffer(&desc, pInitData, &m_pBuffer), L"StructuredBuffer 생성 실패", L"생성 실패!!", E_FAIL);

	// SRV 생성(무조건 필요함)
	CHECK_FAILED(Create_SRV(), E_FAIL);

	if(m_eUsage == SBUSAGE::GPU_READWRITE)
	{
		CHECK_FAILED(Create_UAV(), E_FAIL);
	}

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 뷰 생성 함수 ////////////////////////////////////////////////////////
HRESULT Engine::StructuredBuffer::Create_SRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = m_iNumElements;

	MSG_FAIL(m_pDevice->CreateShaderResourceView(m_pBuffer, &desc, &m_pSRV), L"StructuredBuffer SRV 생성 실패", L"생성 실패!!", E_FAIL);

	return S_OK;
}

HRESULT Engine::StructuredBuffer::Create_UAV()
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = m_iNumElements;
	desc.Buffer.Flags = 0;

	MSG_FAIL(m_pDevice->CreateUnorderedAccessView(m_pBuffer, &desc, &m_pUAV), L"StructuredBuffer UAV 생성 실패", L"생성 실패!!", E_FAIL);

	return S_OK;
}

HRESULT Engine::StructuredBuffer::Create_StagingBuffer()
{
	// 이미 생성된 경우 스킵
	if (m_pStagingBuffer != nullptr)
		return S_OK;

	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.ByteWidth = m_iByteWidth;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	stagingDesc.StructureByteStride = m_iStride;

	MSG_FAIL(m_pDevice->CreateBuffer(&stagingDesc, nullptr, &m_pStagingBuffer), L"Staging Buffer 생성 실패", L"생성 실패!!", E_FAIL);

	return S_OK;
}
/******************************************************* 뷰 생성 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
HRESULT Engine::StructuredBuffer::Update(const void* _data, _uint _byteSize)
{
	if (m_eUsage != SBUSAGE::DYNAMIC)
	{
		MSG_ON(L"DYNAMIC 모드가 아닌 버퍼에 Update를 시도했습니다.", L"업데이트 실패!!");
		BREAK;
		return E_FAIL;
	}

	if (_byteSize > m_iByteWidth)
	{
		MSG_ON(L"업데이트 데이터가 버퍼 크기를 초과합니다.", L"업데이트 실패!!");
		BREAK;
		return E_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	MSG_FAIL(m_pContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
		L"Map 실패", L"업데이트 실패!!", E_FAIL);

	memcpy(mappedResource.pData, _data, _byteSize);
	m_pContext->Unmap(m_pBuffer, 0);

	return S_OK;
}

HRESULT Engine::StructuredBuffer::ReadBack(void* _outData, _uint _byteSize)
{
	if (_outData == nullptr)
	{
		MSG_ON(L"출력 버퍼가 nullptr입니다", L"ReadBack 실패!!!");
		BREAK;
		return E_FAIL;
	}

	BREAK;

	// StagingBuffer 지연 생성
	// ReadBack 함수 안 부를거면 굳이 미리 Buffer 생성해놓을 필요 없기 때문
	MSG_FAIL(Create_StagingBuffer(), L"StagingBuffer 생성에 실패했습니다!", L"생성 실패!!", E_FAIL);

	// GPU 버퍼 -> Staging버퍼로 복사
	m_pContext->CopyResource(m_pStagingBuffer, m_pBuffer);

	// Staging 버퍼 -> CPU 메모리 복사
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	MSG_FAIL(m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource), L"Staging Map 실패", L"복사 실패!!!", E_FAIL);

	_uint copySize = min(_byteSize, m_iByteWidth);
	memcpy(_outData, mappedResource.pData, copySize);
	m_pContext->Unmap(m_pStagingBuffer, 0);

	return S_OK;
}
/******************************************************* 업데이트 함수 *******************************************************/



////////////////////////////////////////////////////////  셰이더 바인딩  ////////////////////////////////////////////////////////
HRESULT Engine::StructuredBuffer::Bind_SRV(_uint _slot, _uint _stageMask)
{
	if (m_pSRV == nullptr)
	{
		MSG_ON(L"SRV가 생성되지 않았습니다.", L"StructuredBuffer");
		BREAK;
		return E_FAIL;
	}

	if (_stageMask & stage_VS) m_pContext->VSSetShaderResources(_slot, 1, &m_pSRV);
	if (_stageMask & stage_PS) m_pContext->PSSetShaderResources(_slot, 1, &m_pSRV);
	if (_stageMask & stage_GS) m_pContext->GSSetShaderResources(_slot, 1, &m_pSRV);
	if (_stageMask & stage_HS) m_pContext->HSSetShaderResources(_slot, 1, &m_pSRV);
	if (_stageMask & stage_DS) m_pContext->DSSetShaderResources(_slot, 1, &m_pSRV);
	if (_stageMask & stage_CS) m_pContext->CSSetShaderResources(_slot, 1, &m_pSRV);

	return S_OK;
}

HRESULT Engine::StructuredBuffer::Bind_UAV(_uint _slot)
{
	if (m_pUAV == nullptr)
	{
		MSG_ON(L"UAV가 생성되지 않았습니다. GPU_READWRITE 모드인지 확인하세요.", L"StructuredBuffer");
		BREAK;
		return E_FAIL;
	}

	m_pContext->CSSetUnorderedAccessViews(_slot, 1, &m_pUAV, nullptr);

	return S_OK;
}

void Engine::StructuredBuffer::Unbind_SRV(_uint _slot, _uint _stageMask)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;

	if (_stageMask & stage_VS) m_pContext->VSSetShaderResources(_slot, 1, &nullSRV);
	if (_stageMask & stage_PS) m_pContext->PSSetShaderResources(_slot, 1, &nullSRV);
	if (_stageMask & stage_GS) m_pContext->GSSetShaderResources(_slot, 1, &nullSRV);
	if (_stageMask & stage_HS) m_pContext->HSSetShaderResources(_slot, 1, &nullSRV);
	if (_stageMask & stage_DS) m_pContext->DSSetShaderResources(_slot, 1, &nullSRV);
	if (_stageMask & stage_CS) m_pContext->CSSetShaderResources(_slot, 1, &nullSRV);
}

void Engine::StructuredBuffer::Unbind_UAV(_uint _slot)
{
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	m_pContext->CSSetUnorderedAccessViews(_slot, 1, &nullUAV, nullptr);
}
/*******************************************************  셰이더 바인딩  *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
StructuredBuffer* Engine::StructuredBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData)
{
	StructuredBuffer* pInstance = new StructuredBuffer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_usage, _numElements, _stride, _initialData), L"StructuredBuffer 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::StructuredBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pUAV);
	Safe_Release(m_pSRV);
	Safe_Release(m_pStagingBuffer);
	Safe_Release(m_pBuffer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	//Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/
