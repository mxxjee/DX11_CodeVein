#include "Engine_Define.h"
#include "VIBuffer_Cell.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Cell::VIBuffer_Cell()
{
}

Engine::VIBuffer_Cell::VIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Cell::VIBuffer_Cell(const VIBuffer_Cell& original)
    : VIBuffer(original)
{
}

Engine::VIBuffer_Cell::~VIBuffer_Cell()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Cell::Initialize_Prototype(const _float3* pPoints)
{
    m_iVertexStride = sizeof(VTXPOS);	// 한 정점의 크기(바이트)
    m_iNumVertices = 3;		// 정점의 개수 (사각형이니까 4개)
    m_iIndexStride = 4;		// 인덱스 하나의 크기(바이트)

    m_iNumIndices = 4;		// 인덱스 개수
    m_iNumVertexBuffers = 1;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// 정점 하나의 바이트 * 정점 갯수
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;			// 정점의 CPU/GPU접근방식
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 바인딩 방식(정점으로)
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;	// 정점 하나의 크기

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices]{};

    m_pVertexPositions = new _float3[m_iNumVertices]{};

    memcpy(pVertices, pPoints, sizeof(VTXPOS) * m_iNumVertices);
    memcpy(m_pVertexPositions, pPoints, sizeof(VTXPOS) * m_iNumVertices);

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"버텍스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);

    Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC IndexBufferDesc = {};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    m_pIndices = new _uint[m_iNumIndices]{};

    m_pIndices[0] = 0;
    m_pIndices[1] = 1;
    m_pIndices[2] = 2;
    m_pIndices[3] = 0;

    D3D11_SUBRESOURCE_DATA IndexInitialData{};
    IndexInitialData.pSysMem = m_pIndices;

    MSG_FAIL(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB), L"인덱스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);
#pragma endregion

    //if(m_bIsAnimated)
    //    *m_pSkinnedVertexPositions = *m_pVertexPositions;

    return S_OK;
}

HRESULT Engine::VIBuffer_Cell::Initialize(void* arg)
{
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Cell* Engine::VIBuffer_Cell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints)
{
    VIBuffer_Cell* pInstance = new VIBuffer_Cell(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(pPoints), L"VIBuffer_Cell 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::VIBuffer_Cell::Clone(void* arg)
{
    VIBuffer_Cell* pInstance = new VIBuffer_Cell(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Cell 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Cell::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

