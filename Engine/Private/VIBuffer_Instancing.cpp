#include "Engine_Define.h"
#include "VIBuffer_Instancing.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Instancing::VIBuffer_Instancing()
{
}

Engine::VIBuffer_Instancing::VIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Instancing::VIBuffer_Instancing(const VIBuffer_Instancing& original)
	: VIBuffer(original)
{
}

Engine::VIBuffer_Instancing::~VIBuffer_Instancing()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Instancing::Initialize_Prototype(_uint iMaxInstance)
{
    m_iMaxInstance = iMaxInstance;
    m_iVertexStride = sizeof(_float4x4);
    m_iNumVertices = iMaxInstance;

    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, nullptr, &m_pVB),
        L"인스턴스 버퍼 생성실패", L"오류", E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/

HRESULT Engine::VIBuffer_Instancing::Update_Instancing(const vector<_float4x4>& vecWorldMatrices)
{
    _uint iInstanceCount = (_uint)vecWorldMatrices.size();
    if (iInstanceCount > m_iMaxInstance)
    {
        iInstanceCount = m_iMaxInstance;
    }

    D3D11_MAPPED_SUBRESOURCE MappedResource;

    if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
        return E_FAIL;

    memcpy(MappedResource.pData, vecWorldMatrices.data(), sizeof(_float4x4) * iInstanceCount);

    m_pContext->Unmap(m_pVB, 0);

    return S_OK;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Instancing* Engine::VIBuffer_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iMaxInstance)
{
	VIBuffer_Instancing* pInstance = new VIBuffer_Instancing(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iMaxInstance)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

	return pInstance;
}

Component* Engine::VIBuffer_Instancing::Clone(void* arg)
{
	VIBuffer_Instancing* pInstance = new VIBuffer_Instancing(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Instancing 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Instancing::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

