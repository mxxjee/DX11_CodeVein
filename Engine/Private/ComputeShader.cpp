#include "Engine_Define.h"
#include "ComputeShader.h"

#include "GameInstance.h"
#include "Buffer.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::ComputeShader::ComputeShader()
{
}

Engine::ComputeShader::ComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext)
{
}

Engine::ComputeShader::ComputeShader(const ComputeShader& original)
	: Component(original)
	, m_pCS(original.m_pCS)
	, m_pReflector(original.m_pReflector)
	, m_umapBuffers(original.m_umapBuffers)
	, m_vecResourceInfos(original.m_vecResourceInfos)
	, m_vecSRVInfos(original.m_vecSRVInfos)
	, m_vecUAVInfos(original.m_vecUAVInfos)
	, m_vThreadGroupSize(original.m_vThreadGroupSize)
	, m_umapHandles(original.m_umapHandles)
{
	Safe_AddRef(m_pCS);
	Safe_AddRef(m_pReflector);

	for (auto& buffer : m_umapBuffers)
	{
		Safe_AddRef(buffer.second);
	}

	for (_uint i = 0; i < 14; ++i)
	{
		m_arrBuffersBySlot[i] = original.m_arrBuffersBySlot[i];
	}
}

Engine::ComputeShader::~ComputeShader()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::Initialize_Prototype(const _wstring& _shaderFilePath, const _string& _shaderEntry)
{
	UINT flags = {};
#ifdef _DEBUG
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	// 컴퓨트 쉐이더 컴파일
	/* Blob = Binary Large OBject, 바이너리 데이터를 담는 범용 메모리 버퍼, 쉐이더 생성했으면 지워주면 됨 */
	ID3DBlob* csBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	if (FAILED(D3DCompileFromFile(_shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		_shaderEntry.c_str(), "cs_5_0", flags, 0, &csBlob, &errorBlob)))
	{
		LogError(errorBlob);
		Safe_Release(csBlob);
		BREAK;
		return E_FAIL;
	}
	Safe_Release(errorBlob);

	// 리플렉션으로 정보 추출
	MSG_FAIL(ReflectResources(csBlob), L"   ", L"!!!", E_FAIL);

	// 컴퓨트 쉐이더 생성  
	MSG_FAIL(m_pDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &m_pCS),
		L"   .", L"!!!", E_FAIL);

	// 생성했으니까 필요없는 Blob지우기
	Safe_Release(csBlob);


	// 상수 버퍼 생성
	for (auto& info : m_vecResourceInfos)
	{
		// 상수버퍼만 만들고 나머지는 패스(샘플러나 텍스쳐같은건 외부에서 생성하니까
		if (info.bindDesc.Type == D3D_SIT_CBUFFER)
		{
			Ready_Buffers(info);
		}
	}

	// 핸들 캐시 구축
	for (_uint i = 0; i < 14; ++i)
	{
		if (m_arrBuffersBySlot[i] == nullptr)
			continue;

		auto umapNames = m_arrBuffersBySlot[i]->Get_UmapVariablenames();
		for (auto& pair : umapNames)
		{
			SHADERHANDLE handle = {};
			handle.iBufferNum = i;
			handle.iVariableNum = pair.second;
			m_umapHandles.try_emplace(pair.first, handle);
		}
	}

	return S_OK;
}

HRESULT Engine::ComputeShader::Initialize(void* arg)
{
	// 일단 아무것도 없음
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::Ready_Buffers(const RESOURCEINFO& _resourceinfo)
{
	_uint slot = _resourceinfo.slot;
	_string cbName = _resourceinfo.name;

	// 이미 같은 슬롯의 버퍼가 있으면 스킵
	if (m_arrBuffersBySlot[slot] != nullptr)
		return S_OK;

	// 전역 풀에서 같은 슬롯 확인
	Buffer* pGlobalBuffer = m_pGameInstance->Find_Buffer(slot);
	if (pGlobalBuffer != nullptr)
	{
		// 같은 cbuffer인지 첫 번째 변수명으로 판별
		ID3D11ShaderReflectionConstantBuffer* pCBReflect = m_pReflector->GetConstantBufferByName(cbName.c_str());
		if (pCBReflect)
		{
			D3D11_SHADER_BUFFER_DESC desc = {};
			pCBReflect->GetDesc(&desc);

			if (desc.Variables > 0)
			{
				ID3D11ShaderReflectionVariable* pFirstVar = pCBReflect->GetVariableByIndex(0);
				D3D11_SHADER_VARIABLE_DESC varDesc = {};
				pFirstVar->GetDesc(&varDesc);

				// 전역 버퍼에 같은 변수가 있으면 같은 cbuffer -> 공유
				if (pGlobalBuffer->Get_UmapVariablenames().contains(varDesc.Name))
				{
					Safe_AddRef(pGlobalBuffer);
					m_umapBuffers.try_emplace(cbName, pGlobalBuffer);
					m_arrBuffersBySlot[slot] = pGlobalBuffer;
					return S_OK;
				}
			}
		}
		// 슬롯은 같지만 다른 cbuffer -> 로컬로 fall through
	}

	// 로컬 풀에서 같은 이름의 버퍼가 이미 있는지 확인
	Buffer* pLocalBuffer = m_pGameInstance->Find_LocalBuffer(cbName);
	if (pLocalBuffer != nullptr)
	{
		// 이미 다른 CS가 만든 같은 로컬 버퍼 -> 공유
		Safe_AddRef(pLocalBuffer);
		m_umapBuffers.try_emplace(cbName, pLocalBuffer);
		m_arrBuffersBySlot[slot] = pLocalBuffer;
		return S_OK;
	}

	// 새 버퍼 생성
	ID3D11ShaderReflectionConstantBuffer* pCBReflect = m_pReflector->GetConstantBufferByName(cbName.c_str());
	if (!pCBReflect)
		return E_FAIL;

	D3D11_SHADER_BUFFER_DESC desc = {};
	if (FAILED(pCBReflect->GetDesc(&desc)) || desc.Type != D3D_CT_CBUFFER)
		return E_FAIL;

	Buffer* pBuffer = Buffer::Create(m_pDevice, m_pContext, pCBReflect, slot);
	CHECK_NULLPTR(pBuffer);

	if (pGlobalBuffer == nullptr)
	{
		// 전역 풀 비어있으면 전역으로 등록 (내부에서 Update_Handles 자동 호출)
		Buffer* testAlready = m_pGameInstance->Register_Buffer(slot, pBuffer);
		if (testAlready)
		{
			MSG_ON(L"중복 버퍼가 있습니다", L"공유버퍼 오류");
			BREAK;
		}
	}
	else
	{
		// 전역 슬롯이 이미 차있으면 로컬로 등록 (내부에서 Update_Handles 자동 호출)
		m_pGameInstance->Register_LocalBuffer(cbName, slot, pBuffer);
	}

	m_umapBuffers.try_emplace(cbName, pBuffer);
	m_arrBuffersBySlot[slot] = pBuffer;

	return S_OK;
}
/*******************************************************  컨테이너 추가 함수  *******************************************************/



//////////////////////////////////////////////////////// 리플렉트 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::ReflectResources(ID3DBlob* _blob)
{
	CHECK_NULLPTR(_blob);

	// 리플렉트 생성
	MSG_FAIL(D3DReflect(_blob->GetBufferPointer(), _blob->GetBufferSize(), IID_PPV_ARGS(&m_pReflector)),
		L" ", L"", E_FAIL);

	// 쉐이더 전체 구조 정보
	D3D11_SHADER_DESC shaderDesc = {};
	m_pReflector->GetDesc(&shaderDesc);

	// numthreads(X, Y, Z) 값 추출
	m_pReflector->GetThreadGroupSize(&m_vThreadGroupSize.x, &m_vThreadGroupSize.y, &m_vThreadGroupSize.z);

	// 바인딩된 리소스 순회
	for (_uint i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc = {};
		if (FAILED(m_pReflector->GetResourceBindingDesc(i, &bindDesc)))
			continue;

		// 리소스 이름 정리 (배열 표기 제거)
		_string name = _string(bindDesc.Name);
		if (name.size() > 3 && name.substr(name.size() - 3) == "[0]")
		{
			name = name.substr(0, name.size() - 3);
		}

		// 리소스 정보 생성
		RESOURCEINFO info = {};
		info.name = name;
		info.slot = bindDesc.BindPoint;
		info.bindDesc = bindDesc;
		info.stageMask = stage_CS;  // 컴퓨트 쉐이더 전용

		// 타입별 분류
		switch (bindDesc.Type)
		{
		case D3D_SIT_CBUFFER:
			m_vecResourceInfos.push_back(info);
			break;

		case D3D_SIT_TEXTURE:
		case D3D_SIT_STRUCTURED:
		case D3D_SIT_BYTEADDRESS:
			m_vecSRVInfos.push_back(info);
			break;

		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_UAV_RWBYTEADDRESS:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
			// 내장 카운터가 있는 구조체 버퍼
			// GPU파티클 시스템에서 많이 쓰는 구조체 버퍼    
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			m_vecUAVInfos.push_back(info);
			break;
		}
	}


	return S_OK;
}
/******************************************************* 리플렉트 함수 *******************************************************/



//////////////////////////////////////////////////////// Get함수 ////////////////////////////////////////////////////////
SHADERHANDLE Engine::ComputeShader::Get_Handle(const _string& _variableName) const
{
	auto it = m_umapHandles.find(_variableName);
	if (it == m_umapHandles.end())
	{
		SHADERHANDLE invalid = {};
		invalid.iBufferNum = UINT_MAX;
		invalid.iVariableNum = UINT_MAX;
		return invalid;
	}

	return it->second;
}
/******************************************************* Get함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 값 대입 함수(Raw) ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::Bind_RawValue(const _string& _constvarname, const void* _value, _uint _length)
{
	for (_uint i = 0; i < 14; ++i)
	{
		if (m_arrBuffersBySlot[i] && m_arrBuffersBySlot[i]->Find_Variable(_constvarname))
		{
			return m_arrBuffersBySlot[i]->Bind_RawValue(_constvarname, _value, _length);
		}
	}

	MSG_ON((L"대입한 이름의 변수 \n[ " + stringToWstring(_constvarname) + L" ]\n가 없습니다").c_str(), L"바인딩 실패!");
	BREAK;
	return E_FAIL;
}

HRESULT Engine::ComputeShader::Bind_RawValue_FullSlot(_uint _slotNum, const _string& _constvarname, const void* _value, _uint _length)
{
	if (m_arrBuffersBySlot[_slotNum] == nullptr)
	{
		MSG_ON((to_wstring(_slotNum) + L"번 슬롯이 비어있습니다.").c_str(), L"바인딩 실패!!!");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_slotNum]->Bind_RawValue(_constvarname, _value, _length);
}

HRESULT Engine::ComputeShader::Bind_RawValue_ByHandle(SHADERHANDLE _handle, const void* _value, _uint _length)
{
	if (m_arrBuffersBySlot[_handle.iBufferNum] == nullptr)
	{
		MSG_ON((to_wstring(_handle.iBufferNum) + L"번 슬롯에 \n[ " + to_wstring(_handle.iVariableNum) + L" ]\n이라는 버퍼가 없습니다.").c_str(), L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_handle.iBufferNum]->Bind_RawValue_ByIndex(_handle.iVariableNum, _value, _length);
}

// 전체 버퍼 한 번에 바인딩
HRESULT Engine::ComputeShader::Bind_EntireBuffer_BySlot(_uint _slot, const void* _data, _uint _length)
{
	if (m_arrBuffersBySlot[_slot] == nullptr)
	{
		MSG_ON(L"해당 슬롯의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_slot]->Bind_EntireBuffer(_data, _length);
}
/******************************************************* 쉐이더 값 대입 함수(Raw) *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 값 대입 함수(Matrix) ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::Bind_Matrix(const _string& _constvarname, const _float4x4& _value)
{
	for (_uint i = 0; i < 14; ++i)
	{
		if (m_arrBuffersBySlot[i] && m_arrBuffersBySlot[i]->Find_Variable(_constvarname))
		{
			return m_arrBuffersBySlot[i]->Bind_Matrix(_constvarname, _value);
		}
	}

	MSG_ON(L"대입한 이름의 변수가 없습니다.", L"바인딩 실패!");
	BREAK;
	return E_FAIL;
}

HRESULT Engine::ComputeShader::Bind_Matrix_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const _float4x4& _value)
{
	if (m_arrBuffersBySlot[_constbufferslot] == nullptr)
	{
		MSG_ON(L"해당 슬롯의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_constbufferslot]->Bind_Matrix(_constvarname, _value);
}

HRESULT Engine::ComputeShader::Bind_Matrix_ByHandle(SHADERHANDLE _handle, const _float4x4& _value)
{
	if (m_arrBuffersBySlot[_handle.iBufferNum] == nullptr)
	{
		MSG_ON(L"해당 슬롯의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_handle.iBufferNum]->Bind_Matrix_ByIndex(_handle.iVariableNum, _value);
}
/******************************************************* 쉐이더 값 대입 함수(Matrix) *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 값 대입 함수(Matrix) ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::Bind_Matrices(const _string& _constvarname, const _float4x4* _value, _uint _length)
{
	for (_uint i = 0; i < 14; ++i)
	{
		if (m_arrBuffersBySlot[i] && m_arrBuffersBySlot[i]->Find_Variable(_constvarname))
		{
			return m_arrBuffersBySlot[i]->Bind_Matrices(_constvarname, _value, _length);
		}
	}

	MSG_ON(L"대입한 이름의 변수가 없습니다.", L"바인딩 실패!");
	BREAK;
	return E_FAIL;
}

HRESULT Engine::ComputeShader::Bind_Matrices_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const _float4x4* _value, _uint _length)
{
	if (m_arrBuffersBySlot[_constbufferslot] == nullptr)
	{
		MSG_ON(L"해당 슬롯의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_constbufferslot]->Bind_Matrices(_constvarname, _value, _length);
}

HRESULT Engine::ComputeShader::Bind_Matrices_ByHandle(SHADERHANDLE _handle, const _float4x4* _value, _uint _length)
{
	if (m_arrBuffersBySlot[_handle.iBufferNum] == nullptr)
	{
		MSG_ON(L"해당 슬롯의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_handle.iBufferNum]->Bind_Matrices_ByIndex(_handle.iVariableNum, _value, _length);
}
/******************************************************* 쉐이더 값 대입 함수(Matrix) *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 값 대입 함수(SRV, UAV) ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::Bind_SRV(const _string& _name, ID3D11ShaderResourceView* _srv)
{
	// 이름으로 SRV 슬롯 찾기
	auto it = std::find_if(m_vecSRVInfos.begin(), m_vecSRVInfos.end(),
		[&_name](const RESOURCEINFO& info) { return info.name == _name; });

	if (it == m_vecSRVInfos.end())
	{
		MSG_ON((L"컴퓨트 쉐이더가 [" + stringToWstring(_name) + L"] SRV를 찾을 수 없습니다.").c_str(), L"SRV 바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	m_pContext->CSSetShaderResources(it->slot, 1, &_srv);
	return S_OK;
}

HRESULT Engine::ComputeShader::Bind_SRV_FullSlot(_uint _slot, ID3D11ShaderResourceView* _srv)
{
	m_pContext->CSSetShaderResources(_slot, 1, &_srv);
	return S_OK;
}

HRESULT Engine::ComputeShader::Bind_UAV(const _string& _name, ID3D11UnorderedAccessView* _uav)
{
	// 이름으로 UAV 슬롯 찾기
	auto it = std::find_if(m_vecUAVInfos.begin(), m_vecUAVInfos.end(),
		[&_name](const RESOURCEINFO& info) { return info.name == _name; });

	if (it == m_vecUAVInfos.end())
	{
		MSG_ON((L"컴퓨트 쉐이더가 [" + stringToWstring(_name) + L"] UAV를 찾을 수 없습니다.").c_str(), L"UAV 바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	m_pContext->CSSetUnorderedAccessViews(it->slot, 1, &_uav, nullptr);
	return S_OK;
}

HRESULT Engine::ComputeShader::Bind_UAV_FullSlot(_uint _slot, ID3D11UnorderedAccessView* _uav)
{
	m_pContext->CSSetUnorderedAccessViews(_slot, 1, &_uav, nullptr);
	return S_OK;
}
/******************************************************* 쉐이더 값 대입 함수(SRV, UAV) *******************************************************/




//////////////////////////////////////////////////////// 실행 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ComputeShader::Dispatch(_uint _groupX, _uint _groupY, _uint _groupZ)
{
	// 상수 버퍼 바인딩
	for (_uint i = 0; i < 14; ++i)
	{
		if (m_arrBuffersBySlot[i])
			m_arrBuffersBySlot[i]->Bind_Buffer(stage_CS);
	}

	// 컴퓨트 쉐이더 설정 및 실행
	m_pContext->CSSetShader(m_pCS, nullptr, 0);
	m_pContext->Dispatch(_groupX, _groupY, _groupZ);

	// 리소스 언바인드 (다음 드로우콜에 영향 방지)
	Unbind_Resources();

	return S_OK;
}
HRESULT Engine::ComputeShader::None_UnbiendDispatch(_uint _groupX, _uint _groupY, _uint _groupZ)
{
	// 상수 버퍼 바인딩
	for (_uint i = 0; i < 14; ++i)
	{
		if (m_arrBuffersBySlot[i])
			m_arrBuffersBySlot[i]->Bind_Buffer(stage_CS);
	}

	// 컴퓨트 쉐이더 설정 및 실행
	m_pContext->CSSetShader(m_pCS, nullptr, 0);
	m_pContext->Dispatch(_groupX, _groupY, _groupZ);

	return S_OK;
}
/******************************************************* 실행 함수 *******************************************************/



//////////////////////////////////////////////////////// 유틸리티 함수 ////////////////////////////////////////////////////////
void Engine::ComputeShader::LogError(ID3DBlob* _errorBlob)
{
	if (_errorBlob)
	{
		OutputDebugStringA(CAST(_cchar)(_errorBlob->GetBufferPointer()));
		Safe_Release(_errorBlob);
	}
}

void Engine::ComputeShader::Unbind_Resources()
{
	// 쉐이더 해제
	m_pContext->CSSetShader(nullptr, nullptr, 0);

	// SRV 해제
	if (!m_vecSRVInfos.empty())
	{
		ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
		_uint maxSlot = 0;
		for (const auto& info : m_vecSRVInfos)
		{
			if (info.slot > maxSlot) maxSlot = info.slot;
		}
		m_pContext->CSSetShaderResources(0, min(maxSlot + 1, (_uint)D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT), nullSRVs);
	}

	// UAV 해제
	if (!m_vecUAVInfos.empty())
	{
		ID3D11UnorderedAccessView* nullUAVs[D3D11_1_UAV_SLOT_COUNT] = { nullptr };
		_uint maxSlot = 0;
		for (const auto& info : m_vecUAVInfos)
		{
			if (info.slot > maxSlot) maxSlot = info.slot;
		}
		m_pContext->CSSetUnorderedAccessViews(0, min(maxSlot + 1, (_uint)D3D11_1_UAV_SLOT_COUNT), nullUAVs, nullptr);
	}
}
/******************************************************* 유틸리티 함수  *******************************************************/



////////////////////////////////////////////////////////  생성자 호출 함수  ////////////////////////////////////////////////////////
ComputeShader* Engine::ComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _shaderFilePath, const _string& _entryPoint)
{
	ComputeShader* pInstance = new ComputeShader(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_shaderFilePath, _entryPoint), L"ComputeShader  ", L"Caution!!!", nullptr);

	return pInstance;
}


//    
ComputeShader* Engine::ComputeShader::Clone(void* arg)
{
	ComputeShader* pInstance = this;
	Safe_AddRef(pInstance);

	return pInstance;
}
/*******************************************************  생성자 호출 함수  *******************************************************/



////////////////////////////////////////////////////////  객체 반환 함수  ////////////////////////////////////////////////////////
void Engine::ComputeShader::Free()
{
	__super::Free();

	for (auto& buffer : m_umapBuffers)
	{
		Safe_Release(buffer.second);
	}
	m_umapBuffers.clear();

	if (!m_bIsClone)
	{
		Safe_Release(m_pReflector);
	}

	Safe_Release(m_pCS);
}
/*******************************************************  객체 반환 함수  *******************************************************/
