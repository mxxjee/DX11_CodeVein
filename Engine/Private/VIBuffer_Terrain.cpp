#include "Engine_Define.h"
#include "VIBuffer_Terrain.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Terrain::VIBuffer_Terrain()
{
}

Engine::VIBuffer_Terrain::VIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Terrain::VIBuffer_Terrain(const VIBuffer_Terrain& original)
	: VIBuffer(original), m_iNumVerticesX(original.m_iNumVerticesX), m_iNumVerticesZ(original.m_iNumVerticesZ)
{
}

Engine::VIBuffer_Terrain::~VIBuffer_Terrain()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Terrain::Initialize_Prototype(const _wstring& heightfilepath)
{
	_ulong dwByte = {};
	HANDLE hfile = CreateFile(heightfilepath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hfile == 0)
	{
		MSG_ON(L"높이맵 파일 로드에 실패했습니다.", L"파일 로드 실패!");
		BREAK;
		return E_FAIL;
	}

	BITMAPFILEHEADER fileheader{};
	BITMAPINFOHEADER infoheader{};
	_uint* pixels = { nullptr };

	_bool result = ReadFile(hfile, &fileheader, sizeof(fileheader), &dwByte, nullptr);
	result = ReadFile(hfile, &infoheader, sizeof(infoheader), &dwByte, nullptr);
	
	m_iNumVerticesX = infoheader.biWidth;
	m_iNumVerticesZ = infoheader.biHeight;
	_uint pixelcount = m_iNumVerticesX * m_iNumVerticesZ;

	pixels = new _uint[pixelcount];
	result = ReadFile(hfile, pixels, sizeof(_uint) * pixelcount, &dwByte, nullptr);

	CloseHandle(hfile);

	
	m_iVertexStride = sizeof(VTXNORTEX);	// 한 정점의 크기(바이트)
	m_iNumVertices = pixelcount;		// 정점의 개수 (사각형이니까 4개)
	m_iIndexStride = 4;		// 인덱스 하나의 크기(바이트)

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;		// 인덱스 개수
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

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices]{};

	m_pVertexPositions = new _float3[m_iNumVertices]{};

	for (_uint z = 0; z < m_iNumVerticesZ; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX; ++x)
		{
			_uint index = (m_iNumVerticesX * z) + x;

			m_pVertexPositions[index] = pVertices[index].vPosition = _float3(_float(x), (pixels[index] & BLUE) * 0.1f, _float(z));
			pVertices[index].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[index].vTexcoord = _float2(x / (m_iNumVerticesX - 1.f), z / (m_iNumVerticesZ - 1.f));
		}
	}

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



	_uint indexnum = {};

	for (_uint z = 0; z < m_iNumVerticesZ - 1; ++z)
	{
		for (_uint x = 0; x < m_iNumVerticesX - 1; ++x)
		{
			_uint index = z * m_iNumVerticesX + x;

			_uint indices[4] = {
				index + m_iNumVerticesX,		/* 0, Left_Top */
				index + m_iNumVerticesX + 1,	/* 1, Right_Top */
				index + 1,						/* 2, Right_Bottom */
				index							/* 3, Left_Bottom */
			};

			// 법선벡터 구하기 위한 임시 벡터
			_vector src{}, temp{}, normal{};

            m_pIndices[indexnum++] = indices[0];
            m_pIndices[indexnum++] = indices[1];
            m_pIndices[indexnum++] = indices[2];

			src = XMLoadFloat3(&pVertices[indices[1]].vPosition) - XMLoadFloat3(&pVertices[indices[0]].vPosition);
			temp = XMLoadFloat3(&pVertices[indices[2]].vPosition) - XMLoadFloat3(&pVertices[indices[1]].vPosition);
			normal = XMVector3Normalize(XMVector3Cross(src, temp));
		
			// 초기값 0, 0, 0인 법선벡터에 정점마다 돌면서 방향을 더해준다
			XMStoreFloat3(&pVertices[indices[0]].vNormal, XMLoadFloat3(&pVertices[indices[0]].vNormal) + normal);
			XMStoreFloat3(&pVertices[indices[1]].vNormal, XMLoadFloat3(&pVertices[indices[1]].vNormal) + normal);
			XMStoreFloat3(&pVertices[indices[2]].vNormal, XMLoadFloat3(&pVertices[indices[2]].vNormal) + normal);
			
            m_pIndices[indexnum++] = indices[0];
            m_pIndices[indexnum++] = indices[2];
            m_pIndices[indexnum++] = indices[3];

			src = XMLoadFloat3(&pVertices[indices[2]].vPosition) - XMLoadFloat3(&pVertices[indices[0]].vPosition);
			temp = XMLoadFloat3(&pVertices[indices[3]].vPosition) - XMLoadFloat3(&pVertices[indices[2]].vPosition);
			normal = XMVector3Normalize(XMVector3Cross(src, temp));

			XMStoreFloat3(&pVertices[indices[0]].vNormal, XMLoadFloat3(&pVertices[indices[0]].vNormal) + normal);
			XMStoreFloat3(&pVertices[indices[2]].vNormal, XMLoadFloat3(&pVertices[indices[2]].vNormal) + normal);
			XMStoreFloat3(&pVertices[indices[3]].vNormal, XMLoadFloat3(&pVertices[indices[3]].vNormal) + normal);
		}
	}

	// 다 더해진 법선벡터를 정규화(단위 벡터화)
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vNormal, (XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal))));
	}


	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"버텍스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pixels);


	D3D11_SUBRESOURCE_DATA IndexInitialData{};
	IndexInitialData.pSysMem = m_pIndices;

	MSG_FAIL(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB), L"인덱스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);
#pragma endregion

    //if(m_bIsAnimated)
    //    *m_pSkinnedVertexPositions = *m_pVertexPositions;

	return S_OK;
}

HRESULT Engine::VIBuffer_Terrain::Initialize(void* arg)
{
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Terrain* Engine::VIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& heightfilepath)
{
	VIBuffer_Terrain* pInstance = new VIBuffer_Terrain(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(heightfilepath), L"VIBuffer_Terrain 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::VIBuffer_Terrain::Clone(void* arg)
{
	VIBuffer_Terrain* pInstance = new VIBuffer_Terrain(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Terrain 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Terrain::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

