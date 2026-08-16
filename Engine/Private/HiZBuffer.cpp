#include "Engine_Define.h"
#include "HiZBuffer.h"

//#include "GameInstance.h"
#include "ComputeShader.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::HiZBuffer::HiZBuffer()
{
}

Engine::HiZBuffer::HiZBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)//, m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	//Safe_AddRef(m_pGameInstance);
}

Engine::HiZBuffer::~HiZBuffer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::HiZBuffer::Initialize(_uint _width, _uint _height)
{
	m_iWidth = _width;
	m_iHeight = _height;

	// mip level 수 계산 log2(max(w, h)) + 1
	// log2 = N을 1이 될 때 까지 몇 번이나 2로 나눌 수 있는가
	// 해상도가 1024면 miplevel은 10(2의 10제곱)
	// 가로, 세로 중 더 긴쪽을 선택해서 mipLevel을 구함
	_uint maxDim = max(m_iWidth, m_iHeight);
	m_iMipLevels = (_uint)floor(log2((_double)maxDim)) + 1;

	CHECK_FAILED(Create_Resources(), E_FAIL);

	m_pDownsampleCS = ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShaer_HiZMipMap.hlsl", "CS_MAIN");
	CHECK_NULLPTR(m_pDownsampleCS);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성 함수 ////////////////////////////////////////////////////////
HRESULT Engine::HiZBuffer::Create_Resources()
{
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = m_iWidth;
	desc.Height = m_iHeight;
	desc.MipLevels = m_iMipLevels;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc = { 1, 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	MSG_FAIL(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pHiZTexture), L"HI-Z Buffer 생성에 실패했습니다!", L"생성 실패!!!", E_FAIL);

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc{};
		srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MostDetailedMip = 0;
		srvdesc.Texture2D.MipLevels = m_iMipLevels;

		MSG_FAIL(m_pDevice->CreateShaderResourceView(m_pHiZTexture, &srvdesc, &m_pHiZSRV), L"HI-Z Buffer 생성에 실패했습니다!", L"생성 실패!!!", E_FAIL);
	}

	// MipLevel별 UAV, SRV 생성
	m_vecMipSRV.resize(m_iMipLevels);
	m_vecMipUAV.resize(m_iMipLevels);

	for (_uint i = 0; i < m_iMipLevels; ++i)
	{
		// 개별 Mip UAV - CS에서 이 mip에 쓰기
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavdesc{};
		uavdesc.Format = DXGI_FORMAT_R32_FLOAT;
		uavdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavdesc.Texture2D.MipSlice = i;

		MSG_FAIL(m_pDevice->CreateUnorderedAccessView(m_pHiZTexture, &uavdesc, &m_vecMipUAV[i]), L"HI-Z Buffer 생성에 실패했습니다!", L"생성 실패!!!", E_FAIL);


		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc{};
		srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MostDetailedMip = i;
		srvdesc.Texture2D.MipLevels = 1;

		MSG_FAIL(m_pDevice->CreateShaderResourceView(m_pHiZTexture, &srvdesc, &m_vecMipSRV[i]), L"HI-Z Buffer 생성에 실패했습니다!", L"생성 실패!!!", E_FAIL);
	}


	return S_OK;
}

HRESULT Engine::HiZBuffer::Generate_MipChain(ID3D11ShaderResourceView* _depthSRV)
{
	if (_depthSRV == nullptr)
		return E_FAIL;

	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	// ── Mip 0: Target_Depth(R32G32B32A32_FLOAT)에서 .r(NDC z)만 추출 ──
	{
		_uint isCopy = 1;
		m_pDownsampleCS->Bind_RawValue_ByHandle(g_IsCopyPass, &isCopy, sizeof(_uint));

		_float2 texSize = { (_float)m_iWidth, (_float)m_iHeight };
		m_pDownsampleCS->Bind_RawValue_ByHandle(g_MipTexSize, &texSize, sizeof(_float2));

		// Target_Depth SRV(float4) -> mip 0 UAV(float)
		m_pContext->CSSetShaderResources(0, 1, &_depthSRV);
		m_pContext->CSSetUnorderedAccessViews(0, 1, &m_vecMipUAV[0], nullptr);

		_uint groupX = (m_iWidth + 7) / 8;
		_uint groupY = (m_iHeight + 7) / 8;
		m_pDownsampleCS->Dispatch(groupX, groupY, 1);

		m_pContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
		m_pContext->CSSetShaderResources(0, 1, &nullSRV);
	}

	// ── Mip 1 ~ N: 이전 mip에서 2x2 max downsample ──
	{
		_uint isCopy = 0;
		m_pDownsampleCS->Bind_RawValue_ByHandle(g_IsCopyPass, &isCopy, sizeof(_uint));

		for (_uint i = 1; i < m_iMipLevels; ++i)
		{
			_uint prevWidth = max(1u, m_iWidth >> (i - 1));
			_uint prevHeight = max(1u, m_iHeight >> (i - 1));
			_float2 prevSize = { (_float)prevWidth, (_float)prevHeight };
			m_pDownsampleCS->Bind_RawValue_ByHandle(g_MipTexSize, &prevSize, sizeof(_float2));

			// 이전 mip SRV -> 현재 mip UAV
			m_pContext->CSSetShaderResources(0, 1, &m_vecMipSRV[i - 1]);
			m_pContext->CSSetUnorderedAccessViews(0, 1, &m_vecMipUAV[i], nullptr);

			_uint mipWidth = max(1u, m_iWidth >> i);
			_uint mipHeight = max(1u, m_iHeight >> i);
			_uint groupX = max(1u, (mipWidth + 7) / 8);
			_uint groupY = max(1u, (mipHeight + 7) / 8);
			m_pDownsampleCS->Dispatch(groupX, groupY, 1);

			m_pContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
			m_pContext->CSSetShaderResources(0, 1, &nullSRV);
		}
	}

	return S_OK;
}
/******************************************************* 생성 함수 *******************************************************/



//////////////////////////////////////////////////////// 화면비 전환 함수 ////////////////////////////////////////////////////////
HRESULT Engine::HiZBuffer::Resize(_uint _width, _uint _height)
{
	Release_Resources();

	m_iWidth = _width;
	m_iHeight = _height;

	_uint maxDim = max(m_iWidth, m_iHeight);
	m_iMipLevels = (_uint)floor(log2((double)maxDim)) + 1;

	return Create_Resources();
}
/******************************************************* 화면비 전환 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
HiZBuffer* Engine::HiZBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _width, _uint _height)
{
	HiZBuffer* pInstance = new HiZBuffer(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_width, _height), L"HiZBuffer 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::HiZBuffer::Free()
{
	__super::Free();

	Release_Resources();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	//Safe_Release(m_pGameInstance);
}

void Engine::HiZBuffer::Release_Resources()
{
	for (auto& uav : m_vecMipUAV) Safe_Release(uav);
	for (auto& srv : m_vecMipSRV) Safe_Release(srv);
	m_vecMipUAV.clear();
	m_vecMipSRV.clear();

	Safe_Release(m_pHiZSRV);
	Safe_Release(m_pHiZTexture);
	Safe_Release(m_pDownsampleCS);
}
/******************************************************* 객체 반환 함수 *******************************************************/

