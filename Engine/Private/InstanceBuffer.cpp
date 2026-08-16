#include "Engine_Define.h"
#include "InstanceBuffer.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::InstanceBuffer::InstanceBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

Engine::InstanceBuffer::~InstanceBuffer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
HRESULT Engine::InstanceBuffer::Initialize(_uint _maxInstanceCount)
{
    // IB의 최대 인스턴스 개수 지정
    m_iMaxInstanceCount = _maxInstanceCount;

    // 인스턴스 버퍼 생성
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(_float4x4) * m_iMaxInstanceCount;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;  // 매 프레임 갱신(매 프레임마다 카메라에 들어오는 객체가 다르므로)
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = sizeof(_float4x4);

    CHECK_FAILED(m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pInstanceBuffer), E_FAIL);

    return S_OK;
}
/******************************************************* 초기화 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
HRESULT Engine::InstanceBuffer::Update_InstanceData(const vector<_float4x4>& _worldMatrices)
{
    // 현재 그려질 인스턴스만을 확인
    m_iCurrentInstanceCount = min((_uint)_worldMatrices.size(), m_iMaxInstanceCount);

    // 없으면 너무좋네 ㅇㅋ
    if (m_iCurrentInstanceCount == 0)
        return S_OK;

    // GPU가 가지고 있는 인스턴스 버퍼를 잠시 꺼내와서 mappedResource에 담아준다
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
    CHECK_FAILED(m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource), E_FAIL);

    // 꺼내온거에 값 수정
    memcpy(mappedResource.pData, _worldMatrices.data(), sizeof(_float4x4) * m_iCurrentInstanceCount);

    // 다시 들어가
    m_pContext->Unmap(m_pInstanceBuffer, 0);

    return S_OK;
}

HRESULT Engine::InstanceBuffer::Update_RawData(const _float4x4* _matrices, _uint _count)
{
    // 개수 제한 (버퍼 크기보다 크면 잘라서 받음)
    m_iCurrentInstanceCount = (_count > m_iMaxInstanceCount) ? m_iMaxInstanceCount : _count;

    if (m_iCurrentInstanceCount == 0) return S_OK;

    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    // MAP_WRITE_DISCARD: 이전 데이터를 버리고 새 메모리를 받음 (고속, 리네이밍 기법)
    if (FAILED(m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
        return E_FAIL;

    // 포인터 위치에서 바로 복사
    memcpy(mappedResource.pData, _matrices, sizeof(_float4x4) * m_iCurrentInstanceCount);

    m_pContext->Unmap(m_pInstanceBuffer, 0);

    return S_OK;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
void Engine::InstanceBuffer::Bind_Buffer(_uint _vertexStride)
{
    // 슬롯 1번에 Instance Buffer 바인딩
    _uint stride = _vertexStride;
    _uint offset = 0;
    m_pContext->IASetVertexBuffers(1, 1, &m_pInstanceBuffer, &stride, &offset);
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 인스턴스 렌더 함수 ////////////////////////////////////////////////////////
void Engine::InstanceBuffer::Render_Instanced(_uint _indexCount)
{
    if (m_iCurrentInstanceCount == 0)
        return;

    // GPU Instancing Draw Call (이거할려고 이 클래스가 필요함)
    m_pContext->DrawIndexedInstanced(_indexCount, m_iCurrentInstanceCount, 0, 0, 0);
}
/******************************************************* 인스턴스 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
InstanceBuffer* Engine::InstanceBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _maxInstanceCount)
{
    InstanceBuffer* pInstance = new InstanceBuffer(pDevice, pContext);

    if (FAILED(pInstance->Initialize(_maxInstanceCount)))
    {
        MSG_BOX("Failed to Created : InstanceBuffer");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::InstanceBuffer::Free()
{
    __super::Free();

    Safe_Release(m_pInstanceBuffer);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
/******************************************************* 객체 반환 함수 *******************************************************/
