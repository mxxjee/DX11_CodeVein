#include "Shader_Old.h"
#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Shader_Old::Shader_Old()
{
}

Engine::Shader_Old::Shader_Old(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext)
{
}

Engine::Shader_Old::Shader_Old(const Shader_Old& original)
	: Component(original), m_pEffect(original.m_pEffect), m_vecInputLayouts(original.m_vecInputLayouts)
{
	for (auto& effect : m_vecInputLayouts)
	{
		Safe_AddRef(effect);
	}

	Safe_AddRef(m_pEffect);
}

Engine::Shader_Old::~Shader_Old()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader_Old::Initialize_Prototype(const _wstring& Shader_OldFilePath, const D3D11_INPUT_ELEMENT_DESC* elementsDesc, _uint numElements)
{
	_uint iHlslFlag = {};

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | (1 << 2);
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif   

	MSG_FAIL(D3DX11CompileEffectFromFile(Shader_OldFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr),
		L"쉐이더 파일 읽기 실패", L"로드 에러!", E_FAIL);

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	MSG_NULL(pTechnique, L"테크닉 불러오기 실패", L"로드 에러!", E_FAIL);

	D3DX11_TECHNIQUE_DESC	Techniquedesc = {};

	pTechnique->GetDesc(&Techniquedesc);

	m_iNumPass = Techniquedesc.Passes;

	for (_uint i = 0; i < m_iNumPass; i++)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };

		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC PassDesc = {};

		pPass->GetDesc(&PassDesc);

		MSG_FAIL(m_pDevice->CreateInputLayout(elementsDesc, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)
		, L"인풋 레이아웃 생성 실패", L"생성 에러!", E_FAIL);

		m_vecInputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT Engine::Shader_Old::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Shader_Old::Bind_Matrix(const _string& constantname, const _float4x4* matrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(constantname.c_str());
	CHECK_NULLPTR(pVariable);

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	CHECK_NULLPTR(pMatrixVariable);

	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(matrix));
}

HRESULT Engine::Shader_Old::Bind_SRV(const _string& constantname, ID3D11ShaderResourceView* SRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(constantname.c_str());
	CHECK_NULLPTR(pVariable);

	ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
	CHECK_NULLPTR(pSRVVariable);

	return pSRVVariable->SetResource(SRV);
}

HRESULT Engine::Shader_Old::Bind_SRVs(const _string& constantname, ID3D11ShaderResourceView** SRV, _uint numsrv)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(constantname.c_str());
	CHECK_NULLPTR(pVariable);

	ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
	CHECK_NULLPTR(pSRVVariable);

	return pSRVVariable->SetResourceArray(SRV, 0, numsrv);
}

HRESULT Engine::Shader_Old::Bind_RawValue(const _string& _constantname, const void* _data, _uint _length)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_constantname.c_str());
	MSG_NULL(pVariable, L"바인딩에 실패했습니다", L"오류!!!", E_FAIL);

	return pVariable->SetRawValue(_data, 0, _length);
}

HRESULT Engine::Shader_Old::Begin(_uint passIndex)
{
	MSG_NULL(m_vecInputLayouts.at(passIndex), L"해당 pass의 Index가 없습니다.", L"경고!!!", E_FAIL);

	m_pContext->IASetInputLayout(m_vecInputLayouts[passIndex]);

	m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(passIndex)->Apply(0, m_pContext);

	return S_OK;
}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Shader_Old* Engine::Shader_Old::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& Shader_OldFilePath, const D3D11_INPUT_ELEMENT_DESC* elementsDesc, _uint numElements)
{
	Shader_Old* pInstance = new Shader_Old(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(Shader_OldFilePath, elementsDesc, numElements), L"Shader_Old 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::Shader_Old::Clone(void* arg)
{
	Shader_Old* pInstance = new Shader_Old(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"Shader_Old 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Shader_Old::Free()
{
	__super::Free();

	for (auto& effect : m_vecInputLayouts)
	{
		Safe_Release(effect);
	}
	m_vecInputLayouts.clear();

	Safe_Release(m_pEffect);
}
/******************************************************* 객체 반환 함수 *******************************************************/

