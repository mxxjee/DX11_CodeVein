#include "Engine_Define.h"
#include "VIBuffer_Rect.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Rect::VIBuffer_Rect()
{
}

Engine::VIBuffer_Rect::VIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Rect::VIBuffer_Rect(const VIBuffer_Rect& original)
	: VIBuffer(original)
{
}

Engine::VIBuffer_Rect::~VIBuffer_Rect()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Rect::Initialize_Prototype()
{
	m_iVertexStride = sizeof(VTXPOSTEX);	// 한 정점의 크기(바이트)
	m_iNumVertices = 4;		// 정점의 개수 (사각형이니까 4개)
	m_iIndexStride = 4;		// 인덱스 하나의 크기(바이트)

	m_iNumIndices = 6;		// 인덱스 개수
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

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices]{};

	m_pVertexPositions = new _float3[m_iNumVertices]{};

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexCoord = _float2(0.f, 0.f);

	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexCoord = _float2(1.f, 0.f);

	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexCoord = _float2(1.f, 1.f);

	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexCoord = _float2(0.f, 1.f);

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

	/* 우상단 삼각형 */
    m_pIndices[0] = 0;
    m_pIndices[1] = 1;
    m_pIndices[2] = 2;
	
	/* 좌하단 삼각형 */
    m_pIndices[3] = 0;
    m_pIndices[4] = 2;
    m_pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = m_pIndices;

	MSG_FAIL(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB), L"인덱스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);
#pragma endregion

    //if(m_bIsAnimated)
    //    *m_pSkinnedVertexPositions = *m_pVertexPositions;

	return S_OK;
}

HRESULT Engine::VIBuffer_Rect::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Rect* Engine::VIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	VIBuffer_Rect* pInstance = new VIBuffer_Rect(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"VIBuffer_Rect 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::VIBuffer_Rect::Clone(void* arg)
{
	VIBuffer_Rect* pInstance = new VIBuffer_Rect(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Rect 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Rect::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

