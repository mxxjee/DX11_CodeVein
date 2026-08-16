#include "Engine_Define.h"
#include "VIBuffer_Cube.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Cube::VIBuffer_Cube()
{
}

Engine::VIBuffer_Cube::VIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Cube::VIBuffer_Cube(const VIBuffer_Cube& original)
    : VIBuffer(original)
{
}

Engine::VIBuffer_Cube::~VIBuffer_Cube()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Cube::Initialize_Prototype()
{
    m_iVertexStride = sizeof(VTXCUBE);	// 한 정점의 크기(바이트)
    m_iNumVertices = 8;		// 정점의 개수 (사각형이니까 4개)
    m_iIndexStride = 4;		// 인덱스 하나의 크기(바이트)

    m_iNumIndices = 36;		// 인덱스 개수
    m_iNumVertexBuffers = 1;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// 정점 하나의 바이트 * 정점 갯수
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;			// 정점의 CPU/GPU접근방식
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 바인딩 방식(정점으로)
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;	// 정점 하나의 크기

    VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices]{};

    m_pVertexPositions = new _float3[m_iNumVertices]{};

    m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
    pVertices[0].vTexcoord = m_pVertexPositions[0];

    m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
    pVertices[1].vTexcoord = m_pVertexPositions[1];

    m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
    pVertices[2].vTexcoord = m_pVertexPositions[2];

    m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
    pVertices[3].vTexcoord = m_pVertexPositions[3];

    m_pVertexPositions[4] = pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
    pVertices[4].vTexcoord = m_pVertexPositions[4];

    m_pVertexPositions[5] = pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
    pVertices[5].vTexcoord = m_pVertexPositions[5];

    m_pVertexPositions[6] = pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
    pVertices[6].vTexcoord = m_pVertexPositions[6];

    m_pVertexPositions[7] = pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
    pVertices[7].vTexcoord = m_pVertexPositions[7];

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

    m_pIndices[0] = 1; m_pIndices[1] = 5; m_pIndices[2] = 6;
    m_pIndices[3] = 1; m_pIndices[4] = 6; m_pIndices[5] = 2;

    m_pIndices[6] = 4; m_pIndices[7] = 0; m_pIndices[8] = 3;
    m_pIndices[9] = 4; m_pIndices[10] = 3; m_pIndices[11] = 7;


    m_pIndices[12] = 4; m_pIndices[13] = 5; m_pIndices[14] = 1;
    m_pIndices[15] = 4; m_pIndices[16] = 1; m_pIndices[17] = 0;


    m_pIndices[18] = 3; m_pIndices[19] = 6; m_pIndices[20] = 2;
    m_pIndices[21] = 3; m_pIndices[22] = 7; m_pIndices[23] = 6;


    m_pIndices[24] = 7; m_pIndices[25] = 6; m_pIndices[26] = 5;
    m_pIndices[27] = 7; m_pIndices[28] = 5; m_pIndices[29] = 4;


    m_pIndices[30] = 0; m_pIndices[31] = 1; m_pIndices[32] = 2;
    m_pIndices[33] = 0; m_pIndices[34] = 2; m_pIndices[35] = 3;

    D3D11_SUBRESOURCE_DATA IndexInitialData{};
    IndexInitialData.pSysMem = m_pIndices;

    MSG_FAIL(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB), L"인덱스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);
#pragma endregion

    //if(m_bIsAnimated)
    //    *m_pSkinnedVertexPositions = *m_pVertexPositions;

    return S_OK;
}

HRESULT Engine::VIBuffer_Cube::Initialize(void* arg)
{
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Cube* Engine::VIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    VIBuffer_Cube* pInstance = new VIBuffer_Cube(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(), L"VIBuffer_Cube 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::VIBuffer_Cube::Clone(void* arg)
{
    VIBuffer_Cube* pInstance = new VIBuffer_Cube(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Cube 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Cube::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

