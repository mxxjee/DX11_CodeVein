#include "Engine_Define.h"
#include "Buffer.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Buffer::Buffer()
{
}

Engine::Buffer::Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::Buffer::~Buffer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Buffer::Initialize(ID3D11ShaderReflectionConstantBuffer* _pCBReflect, _uint _slotnum, D3D_SHADER_INPUT_TYPE _type)
{
	CHECK_NULLPTR(_pCBReflect);

	D3D11_SHADER_BUFFER_DESC cbDesc{};
	_pCBReflect->GetDesc(&cbDesc);

	m_iSlot = _slotnum;
	m_iSize = cbDesc.Size;

	m_pCPUData.resize((m_iSize + 15) & ~15, 0);

	for (_uint i = 0; i < cbDesc.Variables; ++i)
	{
		ID3D11ShaderReflectionVariable* pVar = _pCBReflect->GetVariableByIndex(i);
		CHECK_NULLPTR(pVar);

		D3D11_SHADER_VARIABLE_DESC vardesc{};
		CHECK_FAILED(pVar->GetDesc(&vardesc), E_FAIL);

		ID3D11ShaderReflectionType* pType = pVar->GetType();
		CHECK_NULLPTR(pType);

		D3D11_SHADER_TYPE_DESC typedesc{};
		CHECK_FAILED(pType->GetDesc(&typedesc), E_FAIL);

		BFVARIABLE bfVar{};
		bfVar.iOffset = vardesc.StartOffset;
		bfVar.iSize = vardesc.Size;
		bfVar.eClass = typedesc.Class;

		m_vecVariables.push_back(bfVar);
		m_umapVariablenames.try_emplace(vardesc.Name, i);
	}

	// GPU 상수 버퍼 생성
	D3D11_BUFFER_DESC bufferdesc{};
	bufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferdesc.ByteWidth = (m_iSize + 15) & ~15;
	bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferdesc.MiscFlags = 0;
	bufferdesc.StructureByteStride = 0;
	bufferdesc.Usage = D3D11_USAGE_DYNAMIC;

	MSG_FAIL(m_pDevice->CreateBuffer(&bufferdesc, nullptr, &m_pBuffer), L"버퍼 생성에 실패했습니다.", L"생성 실패", E_FAIL);

	m_bDirty = true;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// Varialbe string 해시값 int 변환 함수 ////////////////////////////////////////////////////////
_int Engine::Buffer::Get_VariableIndex(const _string& _name)
{
	// 만들어진 버퍼에서 변수 찾기
	auto it = m_umapVariablenames.find(_name);

	// 못 찾았으면 -1
	if (it == m_umapVariablenames.end())
	{
		//_wstring msg = L"상수버퍼에" + stringToWstring(_name) + L"이라는 변수가 없습니다";
		//MSG_ON(msg.c_str(), L"검색 실패!");
		//BREAK;
		return -1;
	}

	// 찾았으면 저장된 인덱스(Value) 반환
	return (_int)it->second;
}
/******************************************************* Varialbe string 해시값 int 변환 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Buffer::Bind_RawValue(const _string& _constvarname, const void* _data, const _uint _length)
{
	auto it = m_umapVariablenames.find(_constvarname);
	if (it == m_umapVariablenames.end())
	{
		MSG_ON((L"상수 버퍼에 [" + stringToWstring(_constvarname) + L"] 변수가 없습니다").c_str(), L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	_uint index = it->second;
	const BFVARIABLE& var = m_vecVariables.at(index);

	if (var.iSize != _length)
	{
		MSG_ON(L"데이터 크기가 변수 크기와 다릅니다.", L"크기 불일치");
		BREAK;
		return E_FAIL;
	}

	// 값 비교 결과가 완전히 동일하면 dirtyflag 안 켜고 바로 리턴
	if (memcmp(m_pCPUData.data() + var.iOffset, _data, _length) == 0)
		return S_OK;

	memcpy(m_pCPUData.data() + var.iOffset, _data, _length);
	m_bDirty = true;

	return S_OK;
}

HRESULT Engine::Buffer::Bind_RawValue_ByIndex(_int iIndex, const void* pData, _uint iLength)
{
	/* 유효하지 않은 index 검사 */
	if (iIndex < 0 || iIndex >= CAST(_int)(m_vecVariables.size()))
	{
		MSG_ON(L"유효하지 않은 변수 인덱스입니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	const BFVARIABLE& var = m_vecVariables[(_uint)(iIndex)];

	if (var.iSize != iLength)
	{
		MSG_ON(L"데이터 크기가 변수 크기와 다릅니다.", L"크기 불일치");
		BREAK;
		return E_FAIL;
	}

	// 값 비교 결과가 완전히 동일하면 dirtyflag 안 켜고 바로 리턴
	if (memcmp(m_pCPUData.data() + var.iOffset, pData, iLength) == 0)
		return S_OK;

	memcpy(m_pCPUData.data() + var.iOffset, pData, iLength);
	m_bDirty = true;
	return S_OK;
}

HRESULT Engine::Buffer::Bind_Matrix(const _string& _constvarname, const _float4x4& _value)
{
	// 이름으로 변수 검사(변수명이 같은게 있는지)
	auto it = m_umapVariablenames.find(_constvarname);
	if (it == m_umapVariablenames.end())
	{
		MSG_ON((L"상수 버퍼에 [" + stringToWstring(_constvarname) + L"] 변수가 없습니다").c_str(), L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	// umap컨테이너에서 변수명의 index번호를 추출
	_uint index = it->second;
	// index번호로 저장된 변수 정보 추출
	const BFVARIABLE& var = m_vecVariables.at(index);

	// 크기 검증
	if (var.iSize != sizeof(_float4x4))
	{
		MSG_ON(L"Bind_Matrix는 _float4x4(64바이트) 전용입니다.", L"타입 불일치");
		BREAK;
		return E_FAIL;
	}

	// 기존 행렬과 동일하면 패스
	if (memcmp(m_pCPUData.data() + var.iOffset, &_value, sizeof(_float4x4)) == 0)
		return S_OK;

	memcpy(m_pCPUData.data() + var.iOffset, &_value, sizeof(_float4x4));

	m_bDirty = true;

	return S_OK;
}

HRESULT Engine::Buffer::Bind_Matrix_ByIndex(_int iIndex, const _float4x4& _value)
{
	/* 유효하지 않은 index 검사 */
	if (iIndex < 0 || iIndex >= static_cast<_int>(m_vecVariables.size()))
	{
		MSG_ON(L"유효하지 않은 변수 인덱스입니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}
	// index번호로 저장된 변수 정보 추출
	const BFVARIABLE& var = m_vecVariables.at(static_cast<_uint>(iIndex));
	// 크기 검증
	if (var.iSize != sizeof(_float4x4))
	{
		MSG_ON(L"Bind_Matrix는 _float4x4(64바이트) 전용입니다.", L"타입 불일치");
		BREAK;
		return E_FAIL;
	}
	// 기존 행렬과 동일하면 패스
	if (memcmp(m_pCPUData.data() + var.iOffset, &_value, sizeof(_float4x4)) == 0)
		return S_OK;
	memcpy(m_pCPUData.data() + var.iOffset, &_value, sizeof(_float4x4));
	m_bDirty = true;
	return S_OK;
}

HRESULT Engine::Buffer::Bind_Matrices(const _string& _constvarname, const _float4x4* _value, _uint _length)
{
	// 이름으로 변수 검사(변수명이 같은게 있는지)
	auto it = m_umapVariablenames.find(_constvarname);
	if (it == m_umapVariablenames.end())
	{
		MSG_ON((L"상수 버퍼에 [" + stringToWstring(_constvarname) + L"] 변수가 없습니다").c_str(), L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}
	// umap컨테이너에서 변수명의 index번호를 추출
	const _uint   index = it->second;
	// index번호로 저장된 변수 정보 추출
	const BFVARIABLE& var = m_vecVariables.at(index);

	//// 크기 검증
	//if (var.iSize != sizeof(_float4x4) * _length)
	//{
	//	MSG_ON(L"Matrices는 _float4x4(64바이트) 전용입니다.", L"타입 불일치");
	//	BREAK;
	//	return E_FAIL;
	//}

	// 요청한 길이가 버퍼가 허용하는 최대 길이를 넘는지 검사
	const _uint maxElements = var.iSize / sizeof(_float4x4);
	if (_length == 0 || _length > maxElements)
	{
		MSG_ON((L"행렬의 바인드 최대 개수가 초과되었습니다.\n요청 : {" + to_wstring(_length) + L"}개, 최대 : {" + to_wstring(maxElements) + L"}개").c_str(), L"배열 범위 초과");
		BREAK;
		return E_FAIL;
	}

	// 실제 복사 (16-byte 정렬 보장된 memcpy)
	const size_t copyBytes = sizeof(_float4x4) * _length;
	memcpy(m_pCPUData.data() + var.iOffset, _value, copyBytes);

	// 더티 플래그
	m_bDirty = true;

	return S_OK;
}

HRESULT Engine::Buffer::Bind_Matrices_ByIndex(_int iIndex, const _float4x4* _value, _uint _length)
{
	/* 유효하지 않은 index 검사 */
	if (iIndex < 0 || iIndex >= CAST(_int)(m_vecVariables.size()))
	{
		MSG_ON(L"유효하지 않은 변수 인덱스입니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}
	// index번호로 저장된 변수 정보 추출
	const BFVARIABLE& var = m_vecVariables.at(static_cast<_uint>(iIndex));
	//// 크기 검증
	//if (var.iSize != sizeof(_float4x4) * _length)
	//{
	//	MSG_ON(L"Matrices는 _float4x4(64바이트) 전용입니다.", L"타입 불일치");
	//	BREAK;
	//	return E_FAIL;
	//}
	// 요청한 길이가 버퍼가 허용하는 최대 길이를 넘는지 검사
	const _uint maxElements = var.iSize / sizeof(_float4x4);
	if (_length == 0 || _length > maxElements)
	{
		MSG_ON((L"행렬의 바인드 최대 개수가 초과되었습니다.\n요청 : {" + to_wstring(_length) + L"}개, 최대 : {" + to_wstring(maxElements) + L"}개").c_str(), L"배열 범위 초과");
		BREAK;
		return E_FAIL;
	}
	// 실제 복사 (16-byte 정렬 보장된 memcpy)
	const size_t copyBytes = sizeof(_float4x4) * _length;
	memcpy(m_pCPUData.data() + var.iOffset, _value, copyBytes);
	// 더티 플래그
	m_bDirty = true;
	return S_OK;
}

HRESULT Engine::Buffer::Bind_EntireBuffer(const void* _data, _uint _length)
{
	if (_length != m_iSize)
	{
		MSG_ON(L"데이터 크기가 버퍼 전체 크기와 다릅니다.", L"크기 불일치");
		BREAK;
		return E_FAIL;
	}

	// 1KB보다 작은건 검사 후 같으면 바로 return
	if (_length <= 1024)
	{
		if (memcmp(m_pCPUData.data(), _data, _length) == 0)
			return S_OK;
	}

	memcpy(m_pCPUData.data(), _data, _length);
	m_bDirty = true;

	return S_OK;
}

HRESULT Engine::Buffer::Commit()
{
	if (!m_bDirty) return S_OK;

	D3D11_MAPPED_SUBRESOURCE ms{};
	if (SUCCEEDED(m_pContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)))
	{
		memcpy(ms.pData, m_pCPUData.data(), m_iSize);
		m_pContext->Unmap(m_pBuffer, 0);
		m_bDirty = false;
	}

	return S_OK;
}

HRESULT Engine::Buffer::Bind_Buffer(_uint stageMask)
{
	MSG_FAIL(Commit(), L"버퍼 바인딩 실패", L"이럼 안 되는데", E_FAIL);  // 필요 시 업데이트

	if (stageMask & stage_VS) m_pContext->VSSetConstantBuffers(m_iSlot, 1, &m_pBuffer); // VS
	if (stageMask & stage_PS) m_pContext->PSSetConstantBuffers(m_iSlot, 1, &m_pBuffer); // PS
	if (stageMask & stage_GS) m_pContext->GSSetConstantBuffers(m_iSlot, 1, &m_pBuffer); // GS
	if (stageMask & stage_HS) m_pContext->HSSetConstantBuffers(m_iSlot, 1, &m_pBuffer); // HS
	if (stageMask & stage_DS) m_pContext->DSSetConstantBuffers(m_iSlot, 1, &m_pBuffer); // DS
	if (stageMask & stage_CS) m_pContext->CSSetConstantBuffers(m_iSlot, 1, &m_pBuffer); // CS

	return S_OK;
}

_bool Engine::Buffer::Find_Variable(const _string& _varname)
{
	return m_umapVariablenames.contains(_varname);
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Buffer* Engine::Buffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11ShaderReflectionConstantBuffer* _pCBReflect, _uint _slotnum, D3D_SHADER_INPUT_TYPE _type)
{
	Buffer* pInstance = new Buffer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_pCBReflect, _slotnum, _type), L"Buffer 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Buffer::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pBuffer);
}
/******************************************************* 객체 반환 함수 *******************************************************/
