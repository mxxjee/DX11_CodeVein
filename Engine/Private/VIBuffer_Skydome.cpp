// VIBuffer_Skydome.cpp
#include "Engine_Define.h"
#include "VIBuffer_Skydome.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Skydome::VIBuffer_Skydome()
{
}

Engine::VIBuffer_Skydome::VIBuffer_Skydome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Skydome::VIBuffer_Skydome(const VIBuffer_Skydome& original)
    : VIBuffer(original)
    , m_iSliceCount(original.m_iSliceCount)
    , m_iStackCount(original.m_iStackCount)
    , m_fRadius(original.m_fRadius)
{
}

Engine::VIBuffer_Skydome::~VIBuffer_Skydome()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Skydome::Initialize_Prototype(_uint iSliceCount, _uint iStackCount, _float fRadius)
{
    m_iSliceCount = iSliceCount;
    m_iStackCount = iStackCount;
    m_fRadius = fRadius;

    /* 반구(상반구)를 생성 - 내부에서 바라보는 구조 */
    /* Stack: 위에서 아래로 (북극 -> 적도) */
    /* Slice: 수평 방향 분할 */

    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iNumVertices = (iSliceCount + 1) * (iStackCount + 1);
    m_iIndexStride = 4;
    m_iNumIndices = iSliceCount * iStackCount * 6;
    m_iNumVertexBuffers = 1;
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices]{};
    m_pVertexPositions = new _float3[m_iNumVertices]{};

    _uint vertexIndex = 0;

    for (_uint stack = 0; stack <= iStackCount; ++stack)
    {
        /* 반구이므로 phi는 0(북극) ~ PI/2(적도)까지만 */
        //_float phi = (XM_PI * 0.5f) * ((_float)stack / (_float)iStackCount);
        _float phi = (XM_PI) * ((_float)stack / (_float)iStackCount);
        _float sinPhi = sinf(phi);
        _float cosPhi = cosf(phi);

        for (_uint slice = 0; slice <= iSliceCount; ++slice)
        {
            _float theta = XM_2PI * ((_float)slice / (_float)iSliceCount);
            _float sinTheta = sinf(theta);
            _float cosTheta = cosf(theta);

            /* 구 좌표 -> 직교 좌표 */
            /* Y-up 좌표계: Y가 위쪽 */
            _float3 position = {};
            position.x = fRadius * sinPhi * cosTheta;
            position.y = fRadius * cosPhi;              // 위쪽이 양수
            position.z = fRadius * sinPhi * sinTheta;

            /* UV 좌표 */
            /* U: 수평 방향 (0~1) */
            /* V: 수직 방향 (0=북극, 1=적도) */
            _float2 texcoord = {};
            texcoord.x = (_float)slice / (_float)iSliceCount;
            texcoord.y = (_float)stack / (_float)iStackCount;

            pVertices[vertexIndex].vPosition = position;
            pVertices[vertexIndex].vTexCoord = texcoord;
            m_pVertexPositions[vertexIndex] = position;

            ++vertexIndex;
        }
    }

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB),
        L"Skydome 버텍스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);

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

    _uint index = 0;
    _uint ringVertexCount = iSliceCount + 1;

    for (_uint stack = 0; stack < iStackCount; ++stack)
    {
        for (_uint slice = 0; slice < iSliceCount; ++slice)
        {
            /* 현재 정점 기준으로 사각형의 4개 정점 인덱스 */
            _uint topLeft = stack * ringVertexCount + slice;
            _uint topRight = topLeft + 1;
            _uint bottomLeft = (stack + 1) * ringVertexCount + slice;
            _uint bottomRight = bottomLeft + 1;

            /* 내부에서 볼 때 CCW가 되도록 와인딩 순서 변경 */
            /* 삼각형 1 (외부 기준 CW -> 내부 기준 CCW) */
            m_pIndices[index++] = topLeft;
            m_pIndices[index++] = bottomLeft; //topRight
            m_pIndices[index++] = topRight; //bottomLeft

            /* 삼각형 2 */
            m_pIndices[index++] = topRight;
            m_pIndices[index++] = bottomRight; //bottomRight
            m_pIndices[index++] = bottomLeft; //bottomRight
        }
    }

    D3D11_SUBRESOURCE_DATA IndexInitialData{};
    IndexInitialData.pSysMem = m_pIndices;

    MSG_FAIL(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB),
        L"Skydome 인덱스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);
#pragma endregion

    return S_OK;
}

HRESULT Engine::VIBuffer_Skydome::Initialize(void* arg)
{
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Skydome* Engine::VIBuffer_Skydome::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
    _uint iSliceCount, _uint iStackCount, _float fRadius)
{
    VIBuffer_Skydome* pInstance = new VIBuffer_Skydome(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iSliceCount, iStackCount, fRadius),
        L"VIBuffer_Skydome 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::VIBuffer_Skydome::Clone(void* arg)
{
    VIBuffer_Skydome* pInstance = new VIBuffer_Skydome(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Skydome 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Skydome::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
