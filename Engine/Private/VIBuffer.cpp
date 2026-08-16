#include "Engine_Define.h"
#include "VIBuffer.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer::VIBuffer()
{
}

Engine::VIBuffer::VIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext)
{
}

Engine::VIBuffer::VIBuffer(const VIBuffer& original)
	: Component(original)
	, m_pVB{ original.m_pVB }
	, m_pIB{ original.m_pIB }
	, m_iNumVertices{ original.m_iNumVertices }
	, m_iVertexStride{ original.m_iVertexStride }
	, m_pVertexPositions{ original.m_pVertexPositions }
	, m_iNumIndices{ original.m_iNumIndices }
	, m_iIndexStride{ original.m_iIndexStride }
	, m_iNumVertexBuffers{ original.m_iNumVertexBuffers }
	, m_ePrimitiveType{ original.m_ePrimitiveType }
    , m_pIndices(original.m_pIndices)
{
	Safe_AddRef(m_pVB);
	Safe_AddReff(m_pIB);
}

Engine::VIBuffer::~VIBuffer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::VIBuffer::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer::Render(const _float fTimeDelta)
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 리소스 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer::Bind_Resource()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
	};

	_uint iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint iOffesets[] = {
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffesets);
	m_pContext->IASetIndexBuffer(m_pIB, m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}
/******************************************************* 리소스 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer* Engine::VIBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//VIBuffer* pInstance = new VIBuffer(pDevice, pContext);

	//MSG_FAIL(pInstance->Initialize_Prototype(), L"VIBuffer 생성 실패", L"Caution!!!", nullptr);

	return nullptr;
}

Component* Engine::VIBuffer::Clone(void* arg)
{
	/*VIBuffer* pInstance = new VIBuffer(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer 복사 실패", L"Caution!!!", nullptr);*/

	return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer::Free()
{
	__super::Free();

	if (m_bIsClone == false)
	{
		Safe_Delete_Array(m_pVertexPositions);
        Safe_Delete_Array(m_pIndices);
        // Safe_Delete_Array(m_pSkinnedVertexPositions);
	}

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
/******************************************************* 객체 반환 함수 *******************************************************/

