#include "Engine_Define.h"
#include "Shader.h"

#include "GameInstance.h"
#include "Buffer.h"
#include "Sampler.h"
#include "SamplerManager.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Shader::Shader()
{
}

Engine::Shader::Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext)
{
}

Engine::Shader::Shader(const Shader& original)
	: Component(original), m_vecPass(original.m_vecPass), m_iNumPass(original.m_iNumPass), m_pSamplerManager(original.m_pSamplerManager)
	, m_umapBuffers(original.m_umapBuffers), m_vecResourceInfos(original.m_vecResourceInfos)
{
	for (auto& pass : m_vecPass)
	{
		if (pass.pVS) Safe_AddRef(pass.pVS);
		if (pass.pPS) Safe_AddRef(pass.pPS);
		if (pass.pGS) Safe_AddRef(pass.pGS);
		if (pass.pHS) Safe_AddRef(pass.pHS);
		if (pass.pDS) Safe_AddRef(pass.pDS);
		Safe_AddRef(pass.pInputLayout);
	}

	for (auto& buffer : m_umapBuffers)
	{
		Safe_AddRef(buffer.second);
	}

	Safe_AddRef(m_pSamplerManager);

	for (_uint i = 0; i < 14; ++i)
	{
		m_arrBuffersBySlot[i] = original.m_arrBuffersBySlot[i];
	}

	m_bIsClone = true;
}

Engine::Shader::~Shader()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
void Engine::Shader::LogError(ID3DBlob* _errorBlob)
{
	if (_errorBlob)
	{
		// 디버그용 에러 로그
		OutputDebugStringA(static_cast<const char*>(_errorBlob->GetBufferPointer()));
		Safe_Release(_errorBlob);
	}
}

_wstring Engine::Shader::AnsiToWide(const char* str)
{
	if (!str || !*str) return L"";
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
	std::wstring wstr(len - 1, 0);
	MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], len);
	return wstr;
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader::Initialize_Prototype(const _wstring& _shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _elementsDesc, _uint _numElements, const SHADERENTRIES _entries)
{
	m_iNumPass = _entries.iNumpass;
	m_eShaderEntries.pEntries = new SHADERENTRY[m_iNumPass];
	for (_uint i = 0; i < m_iNumPass; ++i) {
		m_eShaderEntries.pEntries[i] = _entries.pEntries[i];  // std::string 자동 깊은 복사
	}
	m_eShaderEntries.iNumpass = m_iNumPass;

	UINT flags = {};
#ifdef _DEBUG
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif


	// pass의 개수만큼 순회하면서 각 pass에 있는 쉐이더를 생성 후 컨테이너에 등록
	for (_uint i = 0; i < m_iNumPass; ++i)
	{
#pragma region 쉐이더 컴파일
		SHADERENTRY entry = _entries.pEntries[i];
		ID3DBlob* errorBlob = nullptr;

		// 버텍스 쉐이더 컴파일
		ID3DBlob* vsBlob = nullptr;
		if (!entry.vsEntry.empty())
		{
			//쉐이더 경로, 매크로정의, inlcude처리, 엔트리함수 이름, 추가옵션, 결과 Blob, 에러메시지
			if (FAILED(D3DCompileFromFile(_shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.vsEntry.c_str(), "vs_5_0", flags, 0, &vsBlob, &errorBlob)))
			{
				LogError(errorBlob);
				Safe_Release(vsBlob);
				BREAK;
				return E_FAIL;
			}
		}
		// 버텍스 쉐이더 컴파일


		// 픽셀쉐이더 컴파일
		ID3DBlob* psBlob = nullptr;
		if (!entry.psEntry.empty())
		{
			if (FAILED(D3DCompileFromFile(_shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.psEntry.c_str(), "ps_5_0", flags, 0, &psBlob, &errorBlob)))
			{
				char* pErrorMessage = (char*)errorBlob->GetBufferPointer();

				MessageBoxA(nullptr, pErrorMessage, "Shader Compile Error", MB_OK | MB_ICONERROR);

				LogError(errorBlob);
				Safe_Release(vsBlob); Safe_Release(psBlob);
				BREAK;
				return E_FAIL;
			}
		}
		// 픽셀 쉐이더 컴파일

		// 지오메트리 쉐이더 컴파일
		ID3DBlob* gsBlob = nullptr;
		if (!entry.gsEntry.empty())
		{
			if (FAILED(D3DCompileFromFile(_shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.gsEntry.c_str(), "gs_5_0", flags, 0, &gsBlob, &errorBlob)))
			{
				LogError(errorBlob);
				Safe_Release(vsBlob); Safe_Release(psBlob); Safe_Release(gsBlob);
				BREAK;
				return E_FAIL;
			}
		}
		// 지오메트리 쉐이더 컴파일

		// 헐 쉐이더 컴파일
		ID3DBlob* hsBlob = nullptr;
		if (!entry.hsEntry.empty())
		{
			if (FAILED(D3DCompileFromFile(_shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.hsEntry.c_str(), "hs_5_0", flags, 0, &hsBlob, &errorBlob)))
			{
				LogError(errorBlob);
				Safe_Release(vsBlob); Safe_Release(psBlob);	Safe_Release(gsBlob); Safe_Release(hsBlob);
				BREAK;
				return E_FAIL;
			}
		}
		// 헐 쉐이더 컴파일

		// 도메인 쉐이더 컴파일
		ID3DBlob* dsBlob = nullptr;
		if (!entry.dsEntry.empty())
		{
			if (FAILED(D3DCompileFromFile(_shaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.dsEntry.c_str(), "ds_5_0", flags, 0, &dsBlob, &errorBlob)))
			{
				LogError(errorBlob);
				Safe_Release(vsBlob); Safe_Release(psBlob); Safe_Release(gsBlob); Safe_Release(hsBlob); Safe_Release(dsBlob);
				BREAK;
				return E_FAIL;
			}
		}
		// 도메인 쉐이더 컴파일


		Safe_Release(errorBlob);

		//쉐이더 Blob을 해석해서 자동으로 리소스 바인딩?(뭔소리지?) - 다시봐야해
		if (vsBlob != nullptr) { ReflectResources(vsBlob, stage_VS); }
		if (psBlob != nullptr) { ReflectResources(psBlob, stage_PS); }
		if (hsBlob != nullptr) { ReflectResources(hsBlob, stage_HS); }
		if (dsBlob != nullptr) { ReflectResources(dsBlob, stage_DS); }
		if (gsBlob != nullptr) { ReflectResources(gsBlob, stage_GS); }
#pragma endregion


#pragma region 쉐이더 생성
		// 패스 구조체 생성
		PASS pass{};

		// 버텍스 쉐이더 생성
		if (vsBlob != nullptr)
		{
			MSG_FAIL(m_pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pass.pVS),
				L"버텍스 쉐이더 생성에 실패했습니다.", L"쉐이더 생성 실패!", E_FAIL);
		}
		// 버텍스 쉐이더 생성

		// 픽셀 쉐이더 생성
		if (psBlob != nullptr)
		{
			MSG_FAIL(m_pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pass.pPS),
				L"픽셀 쉐이더 생성에 실패했습니다.", L"쉐이더 생성 실패!", E_FAIL);
		}
		// 픽셀 쉐이더 생성

		// 지오메트리 쉐이더 생성
		if (gsBlob != nullptr)
		{
			MSG_FAIL(m_pDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &pass.pGS),
				L"지오메트리 쉐이더 생성에 실패했습니다.", L"쉐이더 생성 실패!", E_FAIL);
		}
		// 지오메트리 쉐이더 생성

		// 헐 쉐이더 생성
		if (hsBlob != nullptr)
		{
			MSG_FAIL(m_pDevice->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &pass.pHS),
				L"헐 쉐이더 생성에 실패했습니다.", L"쉐이더 생성 실패!", E_FAIL);
		}
		// 헐 쉐이더 생성

		// 도메인 쉐이더 생성
		if (dsBlob != nullptr)
		{
			MSG_FAIL(m_pDevice->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &pass.pDS),
				L"도메인 쉐이더 생성에 실패했습니다.", L"쉐이더 생성 실패!", E_FAIL);
		}
		// 도메인 쉐이더 생성



		// 입력 레이아웃은 vsBlob이 있을 때만 생성
		if (vsBlob != nullptr)
		{
			MSG_FAIL(m_pDevice->CreateInputLayout(_elementsDesc, _numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pass.pInputLayout),
				L"인풋 레이아웃 생성에 실패했습니다.", L"쉐이더 생성 실패!", E_FAIL);
		}


		m_vecPass.push_back(pass);

		AssignResourcesToPass(i);

#pragma endregion


		Safe_Release(vsBlob);
		Safe_Release(psBlob);
		Safe_Release(gsBlob);
		Safe_Release(hsBlob);
		Safe_Release(dsBlob);
	}

	for (auto& info : m_vecResourceInfos)
	{
		switch (info.bindDesc.Type)
		{
		case D3D_SIT_CBUFFER:
			Ready_Buffers(info);
			break;
		case D3D_SIT_TEXTURE:
			break;
		case D3D_SIT_STRUCTURED:
			break;
		case D3D_SIT_BYTEADDRESS:
			break;
		}
	}

	Ready_Sampler();

	return S_OK;
}

HRESULT Engine::Shader::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// string 핸들값 Get함수 ////////////////////////////////////////////////////////
SHADERHANDLE Engine::Shader::Get_Handle(const _string& _name)
{
	// 상수 버퍼 슬롯은 최대 14개 (D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
	for (_int i = 0; i < 14; ++i)
	{
		// 이 쉐이더에서 해당 슬롯에 버퍼가 연결되어 있지 않으면 패스(어차피 한 번만 할거라 의미없긴 함)
		if (m_arrBuffersBySlot[i] == nullptr)
			continue;

		// 버퍼 내부에서 변수 인덱스를 검색
		_int iVarIndex = m_arrBuffersBySlot[i]->Get_VariableIndex(_name);

		// 변수 인덱스를 발견했다면(-1이 아니면 발견한거)
		if (iVarIndex != -1)
		{
			SHADERHANDLE handle;
			handle.iBufferNum = i;      // 슬롯 번호 (예: b1이면 1)
			handle.iVariableNum = iVarIndex;  // 변수 인덱스 (예: g_WorldMatrix가 0번째면 0)

			return handle; // 핸들 반환
		}
	}

	// 모든 버퍼를 뒤졌는데도 없으면 실패 핸들 반환 이럼 버그임 좆된거(나한테 말하도록)
	SHADERHANDLE errorHandle;
	errorHandle.iBufferNum = -1;
	errorHandle.iVariableNum = -1;
	_wstring msg = L"상수버퍼에" + stringToWstring(_name) + L"이라는 변수가 없습니다";
	MSG_ON(msg.c_str(), L"검색 실패!");
	BREAK;
	return errorHandle;
}
/******************************************************* string 핸들값 Get함수 *******************************************************/



//////////////////////////////////////////////////////// 버퍼 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader::Ready_Buffers(const RESOURCEINFO& _resourceinfo)
{
	// 이름 중복 방지 + reflector 찾기
	_uint slot = _resourceinfo.slot;
	_string cbName = _resourceinfo.name;

	// 이미 같은 슬롯의 버퍼가 있으면 생성하지 말고 재사용
	if (m_arrBuffersBySlot[slot] != nullptr)
	{
		// 이름도 같아야 정상 (다르면 경고)
		if (m_umapBuffers[cbName] != m_arrBuffersBySlot[slot])
		{
			// 이름은 다르지만 슬롯은 같음 -> HLSL 오류 가능성
			MSG_ON(L"이름은 다른데 슬롯은 같은 이상한 버퍼가 있습니다", L"이런일이 일어날리가 없음");
		}
		return S_OK;  // 이미 있으면 스킵
	}

	// BufferManager에서 먼저 찾아본다
	Buffer* pBuffer = m_pGameInstance->Find_Buffer(slot);

	// 매니저에 있으면 그걸로 등록
	if (pBuffer != nullptr)
	{
		Safe_AddRef(pBuffer);
		m_umapBuffers.try_emplace(cbName, pBuffer);
		m_arrBuffersBySlot[slot] = pBuffer;
		return S_OK;
	}

	// 아니면 상수 버퍼 리플렉션
	ID3D11ShaderReflectionConstantBuffer* pCBReflect = nullptr;

	// 모든 reflector에서 해당 CB 찾기
	for (auto* pReflector : m_vecReflectors)
	{
		ID3D11ShaderReflectionConstantBuffer* pTempreflect = pReflector->GetConstantBufferByName(cbName.c_str());
		D3D11_SHADER_BUFFER_DESC desc{};
		if (SUCCEEDED(pTempreflect->GetDesc(&desc)) && desc.Type == D3D_CT_CBUFFER)
		{
			pCBReflect = pTempreflect;
			break;
		}
	}

	if (!pCBReflect) return E_FAIL;

	// Buffer 생성 및 등록
	pBuffer = Buffer::Create(m_pDevice, m_pContext, pCBReflect, _resourceinfo.slot);
	CHECK_NULLPTR(pBuffer);

	// 새로 만들어진 버퍼를 Manager에 등록(공유)
	Buffer* testAlready = m_pGameInstance->Register_Buffer(slot, pBuffer);

	if (testAlready)
	{
		MSG_ON(L"중복 버퍼가 있습니다", L"공유버퍼 오류");
		BREAK;
	}

	m_umapBuffers.try_emplace(cbName, pBuffer);
	m_arrBuffersBySlot[slot] = pBuffer;
	// 레퍼런스 카운트 증가는 그냥 안 함
	// 어차피 쉐이더가 가지고 있는 Buffer를 그냥 이름, 슬롯 별로 따로 구분해서 저장한것이기 때문

	return S_OK;
}
/******************************************************* 버퍼 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 샘플러, 인풋레이아웃 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader::Ready_Sampler()
{
	m_pSamplerManager = m_pGameInstance->Get_SamplerManager();
	Safe_AddRef(m_pSamplerManager);
	return S_OK;
}
/******************************************************* 샘플러, 인풋레이아웃 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 리소스 정보 추출 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader::ReflectResources(ID3DBlob* _blob, _uint _stageMask)
{
	if (_blob == nullptr) return E_FAIL;

	// 리플렉터 생성: 컴파일된 쉐이더 바이너리에서 메타데이터 추출
	ID3D11ShaderReflection* reflector = nullptr;
	MSG_FAIL(D3DReflect(_blob->GetBufferPointer(), _blob->GetBufferSize(), IID_PPV_ARGS(&reflector)), L"리플렉트 실패", L"오류!!!", E_FAIL);

	// 쉐이더 전체 구조 정보 획득 (쉐이더 타입, 리소스 수 등)
	D3D11_SHADER_DESC shaderdesc{};
	reflector->GetDesc(&shaderdesc);

	// 쉐이더에 바인딩된 모든 리소스 (CB, SRV, Sampler 등)를 순회
	for (_uint i = 0; i < shaderdesc.BoundResources; ++i)
	{
		// 각 리소스의 바인딩 정보 (이름, 슬롯, 타입 등)
		D3D11_SHADER_INPUT_BIND_DESC binddesc{};
		if (FAILED(reflector->GetResourceBindingDesc(i, &binddesc))) continue;

		// HLSL에서 선언된 리소스 이름과 슬롯, 타입 추출 (예: "cbPerFrame"(b0), "g_DiffuseTexture"(t1))
		_string name = _string(binddesc.Name);
		if (name.size() > 3 && name.substr(name.size() - 3) == "[0]")
		{
			name = name.substr(0, name.size() - 3);
		}
		_uint slot = binddesc.BindPoint;
		D3D_SHADER_INPUT_TYPE type = binddesc.Type;

		// slot + type기준 중복체크
		auto it = std::find_if(m_vecResourceInfos.begin(), m_vecResourceInfos.end(),
			[slot, type](const RESOURCEINFO& info) {
				return info.slot == slot && info.bindDesc.Type == type;
			});

		// 없으면 새로 등록
		if (it == m_vecResourceInfos.end())
		{
			RESOURCEINFO info;
			info.name = name;
			info.slot = binddesc.BindPoint;
			info.bindDesc = binddesc;
			info.stageMask = _stageMask;
			m_vecResourceInfos.push_back(info);
		}
		// 이미 있으면 스테이지만 누적
		else
		{
			it->stageMask |= _stageMask;
		}
	}

	m_vecReflectors.push_back(reflector);

	return S_OK;
}
/******************************************************* 리소스 정보 추출 함수 *******************************************************/



//////////////////////////////////////////////////////// 패스 <-> 리소스 연결 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader::AssignResourcesToPass(_uint _passIndex)
{
	if (_passIndex >= m_iNumPass) return E_FAIL;

	PASS& pass = m_vecPass[_passIndex];

	// 현재 Pass에서 사용한 엔트리 정보
	const SHADERENTRY& entry = m_eShaderEntries.pEntries[_passIndex];  // <- 저장해둬야 함!

	for (const auto& resInfo : m_vecResourceInfos)
	{
		// 이 리소스가 현재 Pass의 쉐이더에서 사용됐는지 확인
		bool usedInThisPass = false;

		// VS에서 사용했는지?
		if (!entry.vsEntry.empty() && (resInfo.stageMask & stage_VS))
			usedInThisPass = true;

		// PS에서 사용했는지?
		if (!entry.psEntry.empty() && (resInfo.stageMask & stage_PS))
			usedInThisPass = true;

		// GS에서 사용했는지?
		if (!entry.gsEntry.empty() && (resInfo.stageMask & stage_GS))
			usedInThisPass = true;

		// HS에서 사용했는지?
		if (!entry.hsEntry.empty() && (resInfo.stageMask & stage_HS))
			usedInThisPass = true;

		// DS에서 사용했는지?
		if (!entry.dsEntry.empty() && (resInfo.stageMask & stage_DS))
			usedInThisPass = true;

		// 하나라도 사용했으면 다음으로
		// 바인딩 정보 중복 선언 방지
		if (!usedInThisPass) continue;

		// 바인딩 정보 생성
		RESOURCEINFO binding;
		binding.slot = resInfo.slot;
		binding.stageMask = resInfo.stageMask;
		binding.name = resInfo.name;
		binding.bindDesc = resInfo.bindDesc;

		// 타입별로 분류
		switch (resInfo.bindDesc.Type)
		{
		case D3D_SIT_CBUFFER:
			pass.vecCBs.push_back(binding);
			break;
		case D3D_SIT_TEXTURE:
		case D3D_SIT_STRUCTURED:
		case D3D_SIT_BYTEADDRESS:
			pass.vecSRVs.push_back(binding);
			break;
		case D3D_SIT_SAMPLER:
			pass.vecSamplers.push_back(binding);
			break;
		}
	}

	return S_OK;
}
/******************************************************* 패스 <-> 리소스 연결 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 값 대입 함수(Raw) ////////////////////////////////////////////////////////
HRESULT Engine::Shader::Bind_RawValue(const _string& _constvarname, const void* _value, _uint _length)
{
	for (auto& [bufferName, buffer] : m_umapBuffers)
	{
		if (buffer->Find_Variable(_constvarname))
		{
			return buffer->Bind_RawValue(_constvarname, _value, _length);
		}
	}

	MSG_ON((L"대입한 이름의 변수 \n[ " + stringToWstring(_constvarname) + L" ]\n가 없습니다").c_str(), L"바인딩 실패!");
	BREAK;

	return E_FAIL;
}

HRESULT Engine::Shader::Bind_RawValue_FullName(const _string& _constbuffername, const _string& _constvarname, const void* _value, _uint _length)
{
	if (!m_umapBuffers.contains(_constbuffername))
	{
		MSG_ON(L"해당 이름의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_umapBuffers.at(_constbuffername)->Bind_RawValue(_constvarname, _value, _length);
}

HRESULT Engine::Shader::Bind_RawValue_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const void* _value, _uint _length)
{
	if (m_arrBuffersBySlot[_constbufferslot] == nullptr)
	{
		MSG_ON((to_wstring(_constbufferslot) + L"번 슬롯에 \n[ " + stringToWstring(_constvarname) + L" ]\n이라는 버퍼가 없습니다.").c_str(), L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_constbufferslot]->Bind_RawValue(_constvarname, _value, _length);
}

HRESULT Engine::Shader::Bind_RawValue_ByHandle(SHADERHANDLE _handle, const void* _value, _uint _length)
{
	if(m_arrBuffersBySlot[_handle.iBufferNum] == nullptr)
	{
		MSG_ON((to_wstring(_handle.iBufferNum) + L"번 슬롯 버퍼가 없습니다.").c_str(), L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_handle.iBufferNum]->Bind_RawValue_ByIndex(_handle.iVariableNum, _value, _length);
}

HRESULT Engine::Shader::Bind_EntireBuffer_BySlot(_uint _slot, const void* _data, _uint _length)
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
HRESULT Engine::Shader::Bind_Matrix(const _string& _constvarname, const _float4x4& _value)
{
	for (auto& buffer : m_umapBuffers)
	{
		if (buffer.second->Find_Variable(_constvarname))
		{
			return buffer.second->Bind_Matrix(_constvarname, _value);
		}
	}

	MSG_ON(L"대입한 이름의 변수가 없습니다.", L"바인딩 실패!");
	BREAK;

	return E_FAIL;
}

HRESULT Engine::Shader::Bind_Matrix_FullName(const _string& _constbuffername, const _string& _constvarname, const _float4x4& _value)
{
	if (!m_umapBuffers.contains(_constbuffername))
	{
		MSG_ON(L"해당 이름의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_umapBuffers.at(_constbuffername)->Bind_Matrix(_constvarname, _value);
}

HRESULT Engine::Shader::Bind_Matrix_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const _float4x4& _value)
{
	if (m_arrBuffersBySlot[_constbufferslot] == nullptr)
	{
		MSG_ON(L"해당 슬롯의 버퍼가 없습니다.", L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_constbufferslot]->Bind_Matrix(_constvarname, _value);
}

HRESULT Engine::Shader::Bind_Matrix_ByHandle(SHADERHANDLE _handle, const _float4x4& _value)
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



//////////////////////////////////////////////////////// 쉐이더 값 대입 함수(Matrcies) ////////////////////////////////////////////////////////
HRESULT Engine::Shader::Bind_Matrices(const _string& _constvarname, const _float4x4* _value, _uint _length)
{
	for (auto& buffer : m_umapBuffers)
	{
		if (buffer.second->Find_Variable(_constvarname))
		{
			return buffer.second->Bind_Matrices(_constvarname, _value, _length);
		}
	}

	MSG_ON(L"대입한 이름의 변수가 없습니다.", L"바인딩 실패!");
	BREAK;

	return E_FAIL;
}

HRESULT Engine::Shader::Bind_Matrices_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const _float4x4* _value, _uint _length)
{
	if (m_arrBuffersBySlot[_constbufferslot] == nullptr)
	{
		MSG_ON((to_wstring(_constbufferslot) + L"번 슬롯에 버퍼가 없습니다.").c_str(), L"바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_constbufferslot]->Bind_Matrices(_constvarname, _value, _length);
}

HRESULT Engine::Shader::Bind_Matrices_ByHandle(SHADERHANDLE _handle, const _float4x4* _value, _uint _length)
{
	if (m_arrBuffersBySlot[_handle.iBufferNum] == nullptr)
	{
		MSG_ON(L"바인딩 하려는 쉐이더에 해당 버퍼가 없습니다", L"바인딩 실패!");
		BREAK;
		return E_FAIL;
	}

	return m_arrBuffersBySlot[_handle.iBufferNum]->Bind_Matrices_ByIndex(_handle.iVariableNum, _value, _length);
}
/******************************************************* 쉐이더 값 대입 함수(Matrcies) *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader::Begin(_uint _passIndex, _bool _Shadow)
{
	if (m_iNumPass <= _passIndex)
	{
		MSG_ON(L"쉐이더가 가지고 있는 pass의 개수보다 높은 값이 입력되었습니다", L"쉐이더 오류");
		BREAK;
		return E_FAIL;
	}

	PASS pass = m_vecPass[_passIndex];

	m_pContext->VSSetShader(pass.pVS, nullptr, 0);

	if (_Shadow)
		m_pContext->PSSetShader(nullptr, nullptr, 0);
	else
		m_pContext->PSSetShader(pass.pPS, nullptr, 0);

	m_pContext->GSSetShader(pass.pGS, nullptr, 0);
	m_pContext->HSSetShader(pass.pHS, nullptr, 0);
	m_pContext->DSSetShader(pass.pDS, nullptr, 0);

	if (pass.pInputLayout)
		m_pContext->IASetInputLayout(pass.pInputLayout);

	return S_OK;
}

HRESULT Engine::Shader::Bind_SRV(const _string& _constbuffername, ID3D11ShaderResourceView* _srv)
{
	return Bind_SRVs(_constbuffername, &_srv, 1);
}

HRESULT Engine::Shader::Bind_SRVs(const _string& _constbuffername, ID3D11ShaderResourceView** _srvs, _uint _count)
{
	if (_count >= 128)
	{
		MSG_ON(L"바인딩하려는 텍스쳐가 너무 많습니다.", L"바인딩 오류");
		BREAK;
		return E_FAIL;
	}

	_bool bBound = false;

	for (_uint i = 0; i < m_iNumPass; ++i)
	{
		PASS& pass = m_vecPass[i];

		// 이름으로 SRV 정보 찾기
		auto it = std::find_if(pass.vecSRVs.begin(), pass.vecSRVs.end(),
			[&_constbuffername](const RESOURCEINFO& info) { return info.name == _constbuffername; });

		if (it == pass.vecSRVs.end()) continue;

		const RESOURCEINFO& srvInfo = *it;
		_uint startSlot = srvInfo.slot;

		// D3D11은 startSlot부터 연속으로만 바인딩 가능
		// -> count가 1이면 문제 없음, 여러 개면 슬롯이 연속되어야 함
		ID3D11ShaderResourceView* ppTempSRVs[128] = { nullptr };
		memcpy(ppTempSRVs, _srvs, sizeof(ID3D11ShaderResourceView*) * _count);

		// 스테이지별 바인딩
		if (srvInfo.stageMask & stage_VS)
			m_pContext->VSSetShaderResources(startSlot, _count, ppTempSRVs);
		if (srvInfo.stageMask & stage_PS)
			m_pContext->PSSetShaderResources(startSlot, _count, ppTempSRVs);
		if (srvInfo.stageMask & stage_GS)
			m_pContext->GSSetShaderResources(startSlot, _count, ppTempSRVs);
		if (srvInfo.stageMask & stage_HS)
			m_pContext->HSSetShaderResources(startSlot, _count, ppTempSRVs);
		if (srvInfo.stageMask & stage_DS)
			m_pContext->DSSetShaderResources(startSlot, _count, ppTempSRVs);
		// HS, GS, CS 필요 시 추가

		bBound = true;
	}

	if (!bBound)
	{
		MSG_ON((L"쉐이더가 [" + stringToWstring(_constbuffername) + L"] 라는 SRV를 찾을 수 없습니다.").c_str(), L"SRV 바인딩 실패");
		BREAK;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Engine::Shader::Bind_SRV_FullSlot(const _uint _slot, ID3D11ShaderResourceView* _srv, _uint _stageMask)
{
	return Bind_SRVs_FullSlot(_slot, &_srv, 1, _stageMask);
}

HRESULT Engine::Shader::Bind_SRVs_FullSlot(const _uint _slot, ID3D11ShaderResourceView** _srvs, _uint _count, _uint _stageMask)
{
	if (_count >= 128)
	{
		MSG_ON(L"바인딩하려는 텍스쳐가 너무 많습니다.", L"바인딩 오류");
		BREAK;
		return E_FAIL;
	}

	ID3D11ShaderResourceView* ppTempSRVs[128] = { nullptr };
	memcpy(ppTempSRVs, _srvs, sizeof(ID3D11ShaderResourceView*) * _count);

	// 스테이지별 바인딩
	if (_stageMask & stage_VS)
		m_pContext->VSSetShaderResources(_slot, _count, ppTempSRVs);
	if (_stageMask & stage_PS)
		m_pContext->PSSetShaderResources(_slot, _count, ppTempSRVs);
	if (_stageMask & stage_GS)
		m_pContext->GSSetShaderResources(_slot, _count, ppTempSRVs);
	if (_stageMask & stage_HS)
		m_pContext->HSSetShaderResources(_slot, _count, ppTempSRVs);
	if (_stageMask & stage_DS)
		m_pContext->DSSetShaderResources(_slot, _count, ppTempSRVs);

	return S_OK;
}

HRESULT Engine::Shader::Bind_Resources(_uint _passIndex)
{
	if (m_iNumPass <= _passIndex)
	{
		MSG_ON(L"쉐이더가 가지고 있는 pass의 개수보다 높은 값이 입력되었습니다", L"쉐이더 오류");
		BREAK;
		return E_FAIL;
	}

	// 패스를 가져온다
	PASS& pass = m_vecPass.at(_passIndex);

	// 패스에 저장돼있는 버퍼의 정보를 순회
	for (const auto& cb : pass.vecCBs)
	{
		//// 패스가 가지고 있는 슬롯의 정보가 쉐이더가 가지고 있는 버퍼의 슬롯 정보와 일치하는지 확인
		//auto it = m_umapBuffersBySlot.find(cb.slot);
		//if (it == m_umapBuffersBySlot.end())
		//	continue;

		// 가지고 있던 버퍼를 찾아 가져옴
		Buffer* pBuffer = m_arrBuffersBySlot[cb.slot];
		if (pBuffer == nullptr)
			continue;

		// stageMask 기반으로 각 스테이지에 바인딩
		pBuffer->Bind_Buffer(cb.stageMask);
	}

	for (const auto& sampler : pass.vecSamplers)
	{
		m_pGameInstance->Bind_Sampler(sampler.slot, sampler.stageMask);
	}

	return S_OK;
}

// 위의 Bind_Resources랑 똑같음 함수 이름만 다름
HRESULT Engine::Shader::Commit(_uint _passIndex)
{
	return Bind_Resources(_passIndex);
}

HRESULT Engine::Shader::Update_ContantBuffer(ID3D11Buffer* _buffer, const void* _data, _uint _byteSize)
{
	if (_buffer == nullptr || _data == nullptr)
	{
		MSG_ON(L"해당 버퍼가 만들어지지 않았습니다.", L"업데이트 오류");
		BREAK;
		return E_FAIL;
	}

	m_pContext->UpdateSubresource(_buffer, 0, nullptr, _data, 0, 0);

	return S_OK;
}
const PASS& Engine::Shader::Get_Pass(_uint iPassIndex)
{
	if (iPassIndex >= m_vecPass.size()) {
		MSG_ON(L"인덱스초과.", L"바인딩 오류");
	}
	return m_vecPass[iPassIndex];
}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Shader* Engine::Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* elementsDesc, _uint numElements
	, const SHADERENTRIES _entries)
{
	Shader* pInstance = new Shader(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(shaderFilePath, elementsDesc, numElements, _entries), L"Shader 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::Shader::Clone(void* arg)
{
	Shader* pInstance = this;
	Safe_AddRef(pInstance);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Shader::Free()
{
	__super::Free();

	for (auto& pass : m_vecPass)
	{
		Safe_Release(pass.pVS);
		Safe_Release(pass.pPS);
		Safe_Release(pass.pGS);
		Safe_Release(pass.pHS);
		Safe_Release(pass.pDS);
		Safe_Release(pass.pInputLayout);
	}
	m_vecPass.clear();

	for (auto& buffer : m_umapBuffers)
	{
		Safe_Release(buffer.second);
	}
	m_umapBuffers.clear();

	if (!m_bIsClone)
	{
		for (auto& reflector : m_vecReflectors)
		{
			Safe_Release(reflector);
		}
		m_vecReflectors.clear();

		Safe_Delete_Array(m_eShaderEntries.pEntries);
	}

	Safe_Release(m_pSamplerManager);
}
/******************************************************* 객체 반환 함수 *******************************************************/
