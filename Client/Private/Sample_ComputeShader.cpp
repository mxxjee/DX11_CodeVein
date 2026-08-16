#include "Client_Define.h"
#include "Sample_ComputeShader.h"

//#include "GameInstance.h"
#include "ComputeShader.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Sample_ComputeShader::Sample_ComputeShader()
{
}

Client::Sample_ComputeShader::Sample_ComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

Client::Sample_ComputeShader::Sample_ComputeShader(const Sample_ComputeShader& original)
	: GameObject(original)
{
}

Client::Sample_ComputeShader::~Sample_ComputeShader()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_ComputeShader::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Sample_ComputeShader::Initialize(void* arg)
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

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Sample_ComputeShader_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	CHECK_FAILED(Ready_Components(), E_FAIL);

	CHECK_FAILED(Ready_CShaderResources(), E_FAIL);

	CHECK_FAILED(Ready_BoneCompute(), E_FAIL);

	m_bIsActive = false;
	m_bIsVisible = false;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Sample_ComputeShader::Ready_Components()
{
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Shader(L"Sample_CS"), Com_Shader, RCAST(Component**)(&m_pCShader)), E_FAIL);

	m_pCShader2 = ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Double.hlsl", "CS_ACC");

	g_vSample = m_pCShader2->Get_Handle("g_vSample");
	g_matTest = m_pCShader2->Get_Handle("g_matTest");

	return S_OK;
}

HRESULT Client::Sample_ComputeShader::Ready_CShaderResources()
{
	// 예시 : 1024개
	m_iElementCount = 1000000;
	m_vecElements.resize(m_iElementCount);
	m_vecElements2.resize(m_iElementCount);

	// 벡터 내부를 0, 1, 2, 3, ... 으로 채워넣음
	for (_uint i = 0; i < m_iElementCount; ++i)
	{
		m_vecElements[i] = _float(i);
		m_vecElements2[i] = _float(i);
	}

	/* 입력 버퍼 생성 (SRV용 - 읽기 전용) */
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(_float) * m_iElementCount;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(_float);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = m_vecElements.data();

		m_pDevice->CreateBuffer(&desc, &initData, &m_pInputBuffer);
	}

	/* 출력 버퍼 생성 (UAV용 - 읽고 쓰기) */
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(_float) * m_iElementCount;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; // UNORDERED_ACCESS_VIEW
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(_float);
		desc.Usage = D3D11_USAGE_DEFAULT;

		m_pDevice->CreateBuffer(&desc, nullptr, &m_pOutputBuffer);
	}

	/* SRV 생성 (입력 버퍼용) */
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_UNKNOWN;  // Structured Buffer는 UNKNOWN
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = m_iElementCount;

		m_pDevice->CreateShaderResourceView(m_pInputBuffer, &desc, &m_pInputSRV);
	}

	/* UAV 생성 (출력 버퍼용) */
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;  // Structured Buffer는 UNKNOWN
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_iElementCount;

		m_pDevice->CreateUnorderedAccessView(m_pOutputBuffer, &uavDesc, &m_pOutputUAV);
	}

	/* 읽기, 쓰기 모두 되는 버퍼 */
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(_float) * m_iElementCount;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; // SRV와 UAV를 모두
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(_float);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = m_vecElements2.data();

		m_pDevice->CreateBuffer(&desc, &initData, &m_pBothBuffer);
	}

	/* 읽고쓰기용 UAV (위의 읽기, 쓰기 버퍼로 만들기) */
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;  // Structured Buffer는 UNKNOWN
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_iElementCount;

		m_pDevice->CreateUnorderedAccessView(m_pBothBuffer, &uavDesc, &m_pOutputUAV2);
	}

	/* 스테이징용 버퍼 생성 */
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(_float) * m_iElementCount;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // 스테이징은 READ

		m_pDevice->CreateBuffer(&desc, nullptr, &m_pStagingBuffer);
	}

	return S_OK;
}

HRESULT Client::Sample_ComputeShader::Ready_BoneCompute()
{
	m_iNumBones = 500;

	// 500개의 본 행렬
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		IdentityMatrix(m_matBones[i]);
		IdentityMatrix(m_matCPUBones[i]);

		 // 약간의 값 변화 (확인하기 쉽게)
		 //m_matBones[i]._11 = 1.1f;
	}

	/* 읽기, 쓰기 모두 되는 버퍼 */
	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(_float4x4) * m_iNumBones;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; // SRV와 UAV를 모두
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(_float4x4);
		desc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = m_matBones;

		m_pDevice->CreateBuffer(&desc, &initData, &m_pBoneBuffer);
	}

	/* 읽고쓰기용 UAV (위의 읽기, 쓰기 버퍼로 만들기) */
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;  // Structured Buffer는 UNKNOWN
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_iNumBones;

		m_pDevice->CreateUnorderedAccessView(m_pBoneBuffer, &uavDesc, &m_pBoneUAV);
	}

	// 컴퓨트 쉐이더 생성
	m_pCShader_Bone = ComputeShader::Create(m_pDevice, m_pContext,
		L"../../Shader/CShader_Double.hlsl", "CS_BONES");

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Sample_ComputeShader::Update(const _float fTimeDelta)
{
	// C 누르면 컴퓨트 쉐이더 1회 실행
	if (m_pGameInstance->KeyDown(DIK_C))
	{
		Test_ComputeShader_Base();
	}
	
	if (m_bIsActive && m_fTimer >= 1.f)
	{
		//Test_ComputeShader_Multiple();

		m_fTimer = 0.f;
	}

	// 
	if (m_bIsActive)
	{
		Test_ComputeShader_BoneMatrices();
	}

	if (m_bIsVisible)
	{
		Test_CPU_BoneMatrices();
	}

	// 껐다켰다 하면서 본행렬 계산 FPS차이 느껴보기
	if (m_pGameInstance->KeyDown(DIK_V))
	{
		m_bIsActive = !m_bIsActive;
		if (m_bIsActive)
		{
			COUT("컴퓨트 쉐이더 계산 활성화");
		}
		else
			COUT("컴퓨트 쉐이더 계산 비활성화");
	}
	else if (m_pGameInstance->KeyDown(DIK_B))
	{
		m_bIsVisible = !m_bIsVisible;
		if (m_bIsVisible)
		{
			COUT("CPU 계산 활성화");
		}
		else
			COUT("CPU 계산 비활성화");
	}

	m_fTimer += fTimeDelta;

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 예제 함수 ////////////////////////////////////////////////////////
void Client::Sample_ComputeShader::Test_ComputeShader_Base()
{
	_float 실수{};
	m_pCShader2->Bind_RawValue_ByHandle(g_vSample, &실수, sizeof(실수));
	_float4x4 행렬{};
	m_pCShader2->Bind_Matrix_ByHandle(g_matTest, 행렬);

	// 쉐이더에 바인딩
	m_pCShader->Bind_SRV_FullSlot(0, m_pInputSRV);
	m_pCShader->Bind_UAV_FullSlot(0, m_pOutputUAV);



	// 1024개 요소 / 64 스레드 = 16 그룹
	// 쉐이더 내부에서 한 그룹당 64개의 쓰레드를 사용하겠다고 선언해둠
	m_pCShader->Dispatch(3907, 1, 1);

	// GPU -> Staging버퍼로 복사
	m_pContext->CopyResource(m_pStagingBuffer, m_pOutputBuffer);

	// CPU에서 읽기
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	if (SUCCEEDED(m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
	{
		_float* pResultData = static_cast<_float*>(mappedResource.pData);

		_string input = "입력 : ";
		_string output = "출력 : ";

		// 결과 확인 (처음 3개만 출력)	
		for (_uint i = 0; i < 3; ++i)
		{
			// 입력: 0, 1, 2, 3, ...
			// 출력: 0, 2, 4, 6, ... (2배)
			input = input + to_string(m_vecElements[i]) + ", ";
			output = output + to_string(pResultData[i]) + ", ";
		}
		input += "...  ";
		output += "...  ";
		for (_uint back = m_iElementCount - 3; back < m_iElementCount; ++back)
		{
			input = input + to_string(m_vecElements[back]) + ", ";
			output = output + to_string(pResultData[back]) + ", ";
		}

		COUT(input);
		COUT(output);

		m_pContext->Unmap(m_pStagingBuffer, 0);
	}
}

void Client::Sample_ComputeShader::Test_ComputeShader_Multiple()
{
	// 쉐이더에 바인딩
	//m_pCShader->Bind_SRV_FullSlot(0, m_pInputSRV2);
	m_pCShader2->Bind_UAV_FullSlot(1, m_pOutputUAV2);

	// 1024개 요소 / 64 스레드 = 16 그룹
	// 쉐이더 내부에서 한 그룹당 64개의 쓰레드를 사용하겠다고 선언해둠
	m_pCShader2->Dispatch(16, 1, 1);

	// GPU -> Staging버퍼로 복사
	m_pContext->CopyResource(m_pStagingBuffer, m_pBothBuffer);

	// CPU에서 읽기
	// 순전히 콘솔창에 띄워보기 위한 용도지 실전에서는 Map UnMap 안 해도 됨
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	if (SUCCEEDED(m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
	{
		_float* pResultData = static_cast<_float*>(mappedResource.pData);

		_string input = "입력 : ";
		_string output = "출력 : ";

		// 결과 확인 (처음 3개만 출력)	
		for (_uint i = 0; i < 3; ++i)
		{
			// 입력: 0, 1, 2, 3, ...
			// 출력: 0, 2, 4, 6, ... (2배)
			input = input + to_string(m_vecElements2[i]) + ", ";
			output = output + to_string(pResultData[i]) + ", ";
		}
		input += "...  ";
		output += "...  ";
		for (_uint back = m_iElementCount - 3; back < m_iElementCount; ++back)
		{
			input = input + to_string(m_vecElements2[back]) + ", ";
			output = output + to_string(pResultData[back]) + ", ";
		}

		COUT(input);
		COUT(output);

		m_pContext->Unmap(m_pStagingBuffer, 0);
	}

}

void Client::Sample_ComputeShader::Test_ComputeShader_BoneMatrices()
{
	// 본 개수 바인딩 (상수 버퍼)
	m_pCShader_Bone->Bind_RawValue_FullSlot(6, "g_NumBones", &m_iNumBones, sizeof(_uint));

	// 본 행렬 UAV 바인딩
	m_pCShader_Bone->Bind_UAV_FullSlot(0, m_pBoneUAV);

	// Dispatch: 500개 / 256 스레드 = 2 그룹
	_uint iGroupCount = (m_iNumBones + 255) / 256;
	m_pCShader_Bone->Dispatch(iGroupCount, 1, 1);
}

void Client::Sample_ComputeShader::Test_CPU_BoneMatrices()
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_matrix bone = XMLoadFloat4x4(&m_matBones[i]);
		bone *= bone;
		XMStoreFloat4x4(&m_matBones[i], bone);
	}
}
/******************************************************* 예제 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sample_ComputeShader* Client::Sample_ComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Sample_ComputeShader* pInstance = new Sample_ComputeShader(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Sample_ComputeShader 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Sample_ComputeShader::Clone(void* pArg)
{
	Sample_ComputeShader* pInstance = new Sample_ComputeShader(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Sample_ComputeShader 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Sample_ComputeShader::Free()
{
	__super::Free();

	Safe_Release(m_pCShader);
	Safe_Release(m_pCShader2);
	Safe_Release(m_pCShader_Bone);
	Safe_Release(m_pInputSRV);
	Safe_Release(m_pOutputUAV);
	Safe_Release(m_pOutputUAV2);
	Safe_Release(m_pInputBuffer);
	Safe_Release(m_pOutputBuffer);
	Safe_Release(m_pStagingBuffer);
	Safe_Release(m_pBothBuffer);
	Safe_Release(m_pBoneBuffer);
	Safe_Release(m_pBoneUAV);
}
/******************************************************* 객체 반환 함수 *******************************************************/
