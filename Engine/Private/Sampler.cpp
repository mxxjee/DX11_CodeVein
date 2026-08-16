#include "Engine_Define.h"
#include "Sampler.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Sampler::Sampler()
{
}

Engine::Sampler::Sampler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::Sampler::~Sampler()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Sampler::Initialize(const D3D11_SAMPLER_DESC& _desc, _uint _slot)
{
	m_ibindSlot = _slot;

	return m_pDevice->CreateSamplerState(&_desc, &m_pSampler);
}

HRESULT Engine::Sampler::Bind_Shader(_uint _stageMask)
{
	if (_stageMask & stage_VS) m_pContext->VSSetSamplers(m_ibindSlot, 1, &m_pSampler); // VS
	if (_stageMask & stage_PS) m_pContext->PSSetSamplers(m_ibindSlot, 1, &m_pSampler); // PS

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Sampler* Engine::Sampler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const D3D11_SAMPLER_DESC& _desc, _uint _slot)
{
	Sampler* pInstance = new Sampler(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(_desc, _slot), L"Sampler 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Sampler::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pSampler);
}
/******************************************************* 객체 반환 함수 *******************************************************/

