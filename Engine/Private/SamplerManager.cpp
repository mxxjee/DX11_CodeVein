#include "Engine_Define.h"
#include "SamplerManager.h"

#include "Sampler.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::SamplerManager::SamplerManager()
{
}

Engine::SamplerManager::SamplerManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::SamplerManager::~SamplerManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/


//샘플러 똑같이 필요하면 추가해서 사용
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::SamplerManager::Initialize()
{
	D3D11_SAMPLER_DESC desc{};

	// 기본 선형 필터 + Wrap
	desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias     = 0.0f;
	desc.MaxAnisotropy  = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD         = 0.0f;
	desc.MaxLOD         = D3D11_FLOAT32_MAX;
	Sampler* sampler = Sampler::Create(m_pDevice, m_pContext, desc, 0);

	m_umapSampler.try_emplace("LinearSampler", sampler);
	m_umapSamplerBySlot.try_emplace(0, sampler);


	D3D11_SAMPLER_DESC pointdesc{};
	// Point필터
	pointdesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointdesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointdesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointdesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointdesc.MipLODBias     = 0.0f;
	pointdesc.MaxAnisotropy  = 1;                    // 포인트 필터는 이방성 무시
	pointdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pointdesc.BorderColor[0] = 0.0f;
	pointdesc.BorderColor[1] = 0.0f;
	pointdesc.BorderColor[2] = 0.0f;
	pointdesc.BorderColor[3] = 0.0f;
	pointdesc.MinLOD         = 0.0f;
	pointdesc.MaxLOD         = D3D11_FLOAT32_MAX;

	sampler = Sampler::Create(m_pDevice, m_pContext, pointdesc, 1);

	m_umapSampler.try_emplace("PointSampler", sampler);
	m_umapSamplerBySlot.try_emplace(1, sampler);


    D3D11_SAMPLER_DESC clampdesc{};

    // 기본 선형 필터 + Wrap
    clampdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    clampdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    clampdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    clampdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    clampdesc.MipLODBias = 0.0f;
    clampdesc.MaxAnisotropy = 1;
    clampdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    clampdesc.MinLOD = 0.0f;
    clampdesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampler = Sampler::Create(m_pDevice, m_pContext, clampdesc, 2);

    m_umapSampler.try_emplace("LinearClampSampler", sampler);
    m_umapSamplerBySlot.try_emplace(2, sampler);


    D3D11_SAMPLER_DESC anisodesc{};
    anisodesc.Filter         = D3D11_FILTER_ANISOTROPIC;
    anisodesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    anisodesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    anisodesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    anisodesc.MipLODBias     = 0.0f;
    anisodesc.MaxAnisotropy  = 16;  // 1~16, 높을수록 품질↑
    anisodesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    anisodesc.MinLOD         = 0.0f;
    anisodesc.MaxLOD         = D3D11_FLOAT32_MAX;

    sampler = Sampler::Create(m_pDevice, m_pContext, anisodesc, 3);

    m_umapSampler.try_emplace("AnisotropicSampler", sampler);
    m_umapSamplerBySlot.try_emplace(3, sampler);


	D3D11_SAMPLER_DESC ShadowDesc = {};
	ShadowDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;  // ← 선형 필터링!
	ShadowDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	ShadowDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	ShadowDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	ShadowDesc.BorderColor[0] = 1.0f;  // 범위 밖은 밝게 (그림자 X)
	ShadowDesc.BorderColor[1] = 1.0f;
	ShadowDesc.BorderColor[2] = 1.0f;
	ShadowDesc.BorderColor[3] = 1.0f;
	ShadowDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;  // ← Comparison 모드!
	ShadowDesc.MinLOD = 0;
	ShadowDesc.MaxLOD = D3D11_FLOAT32_MAX;

	sampler = Sampler::Create(m_pDevice, m_pContext, ShadowDesc, 4);

	m_umapSampler.try_emplace("ShadowSampler", sampler);
	m_umapSamplerBySlot.try_emplace(4, sampler);


	/////미니맵에 쓸 bordersampler
	D3D11_SAMPLER_DESC MinimapDesc = {};
	MinimapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	MinimapDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	MinimapDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	MinimapDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	MinimapDesc.BorderColor[0] = 0.0f; // R
	MinimapDesc.BorderColor[1] = 0.0f; // G
	MinimapDesc.BorderColor[2] = 0.0f; // B
	MinimapDesc.BorderColor[3] = 1.0f; // A (알파)
	MinimapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	MinimapDesc.MinLOD = 0;
	MinimapDesc.MaxLOD = D3D11_FLOAT32_MAX;

	sampler = Sampler::Create(m_pDevice, m_pContext, MinimapDesc, 5);

	m_umapSampler.try_emplace("BorderSampler", sampler);
	m_umapSamplerBySlot.try_emplace(5, sampler);

	//SSAO에서 쓸 Sampler
	D3D11_SAMPLER_DESC pointWrapdesc{};
	// Point필터
	pointWrapdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointWrapdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapdesc.MipLODBias = 0.0f;
	pointWrapdesc.MaxAnisotropy = 1;                    // 포인트 필터는 이방성 무시
	pointWrapdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pointWrapdesc.BorderColor[0] = 0.0f;
	pointWrapdesc.BorderColor[1] = 0.0f;
	pointWrapdesc.BorderColor[2] = 0.0f;
	pointWrapdesc.BorderColor[3] = 0.0f;
	pointWrapdesc.MinLOD = 0.0f;
	pointWrapdesc.MaxLOD = D3D11_FLOAT32_MAX;

	sampler = Sampler::Create(m_pDevice, m_pContext, pointWrapdesc, 6);

	m_umapSampler.try_emplace("PointWrapSampler", sampler);
	m_umapSamplerBySlot.try_emplace(6, sampler);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::SamplerManager::Bind_Sampler(const _string& _samplername, _uint _stageMask)
{
	return m_umapSampler.at(_samplername)->Bind_Shader(_stageMask);
}

HRESULT Engine::SamplerManager::Bind_Sampler(_uint _slot, _uint _stageMask)
{
    if (!m_umapSamplerBySlot.contains(_slot))
    {
        MSG_ON(L"해당 슬롯의 샘플러가 없습니다.", L"바인딩 실패");
        BREAK;
        return E_FAIL;
    }

	return m_umapSamplerBySlot.at(_slot)->Bind_Shader(_stageMask);
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
SamplerManager* Engine::SamplerManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	SamplerManager* pInstance = new SamplerManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(), L"SamplerManager 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::SamplerManager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& sampler : m_umapSampler)
	{
		Safe_Release(sampler.second);
	}
	m_umapSampler.clear();
	m_umapSamplerBySlot.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/

