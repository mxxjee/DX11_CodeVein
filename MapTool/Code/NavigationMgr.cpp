#include "MT_Defines.h"
#include "NavigationMgr.h"
#include "GameInstance.h" 

IMPLEMENT_SINGLETON(CNavigationMgr)

CNavigationMgr::CNavigationMgr()
{
}

HRESULT CNavigationMgr::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pNavMeshPath)
{
    m_pGameInstance = GameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    m_pDevice = pDevice;
    m_pContext = pContext;
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
    if (m_pNavMesh) dtFreeNavMesh(m_pNavMesh);
    if (m_pNavQuery) dtFreeNavMeshQuery(m_pNavQuery);

    if (FAILED(LoadNavMesh(pNavMeshPath)))
        return E_FAIL;

    MakeDebugMesh();

    return S_OK;
}

HRESULT CNavigationMgr::LoadNavMesh(const char* path)
{
    FILE* fp = nullptr;

    if (fopen_s(&fp, path, "rb") != 0 || !fp) 
        return E_FAIL;

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* navData = (unsigned char*)dtAlloc(size, DT_ALLOC_PERM);
    if (!navData) { fclose(fp); return E_FAIL; }

    fread(navData, size, 1, fp);
    fclose(fp);

    m_pNavMesh = dtAllocNavMesh();
    if (!m_pNavMesh) { dtFree(navData); return E_FAIL; }

    dtStatus status = m_pNavMesh->init(navData, size, DT_TILE_FREE_DATA);
    if (dtStatusFailed(status)) { dtFree(navData); return E_FAIL; }

    m_pNavQuery = dtAllocNavMeshQuery();
    m_pNavQuery->init(m_pNavMesh, 2048);

    return S_OK;
}

void CNavigationMgr::MakeDebugMesh()
{
    if (!m_pNavMesh) return;

    vector<VTX_NAV> vertices;
    vector<_uint> indices;
    _uint iVertCount = 0;

    for (int i = 0; i < m_pNavMesh->getMaxTiles(); ++i)
    {
        const dtMeshTile* tile = ((CNavMeshAccessor*)m_pNavMesh)->GetTile_Public(i);
        if (!tile || !tile->header) continue;

        for (int j = 0; j < tile->header->polyCount; ++j)
        {
            const dtPoly* p = &tile->polys[j];
            if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;

            const dtPolyDetail* pd = &tile->detailMeshes[j];

            for (int k = 2; k < p->vertCount; ++k)
            {
                _float3 v[3];
                for(int m=0; m<3; ++m) {
                    int idx = (m==0)? 0 : (m==1)? k-1 : k;
                    const float* pos = &tile->verts[p->verts[idx] * 3];
                    v[m] = _float3(pos[0], pos[1] + 0.2f, pos[2]);
                }

                vertices.push_back({ v[0], _float3(0.f, 1.f, 1.f) }); 
                vertices.push_back({ v[1], _float3(0.f, 1.f, 1.f) }); 
                vertices.push_back({ v[2], _float3(0.f, 1.f, 1.f) });

                indices.push_back(iVertCount++);
                indices.push_back(iVertCount++);
                indices.push_back(iVertCount++);
            }
        }
    }

    m_iNumIndices = indices.size();
    if (m_iNumIndices == 0) return;

    D3D11_BUFFER_DESC tBufferDesc;
    ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));
    tBufferDesc.ByteWidth = sizeof(VTX_NAV) * vertices.size();
    tBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA tData;
    ZeroMemory(&tData, sizeof(D3D11_SUBRESOURCE_DATA));
    tData.pSysMem = vertices.data();

    m_pDevice->CreateBuffer(&tBufferDesc, &tData, &m_pVB);

    tBufferDesc.ByteWidth = sizeof(_uint) * indices.size();
    tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    tData.pSysMem = indices.data();
    
#ifdef _DEBUG
    char szLog[256] = "";

    sprintf_s(szLog, "================ [NavMesh Debug Log] ================\n");
    OutputDebugStringA(szLog);

    sprintf_s(szLog, "Total Vertices: %d, Total Indices: %d\n", (int)vertices.size(), (int)indices.size());
    OutputDebugStringA(szLog);

    int iCheckCount = 0; 
    for (auto& vtx : vertices)
    {
        if (iCheckCount++ > 5) break;

        sprintf_s(szLog, "Vtx[%d]: (%.2f, %.2f, %.2f)\n", 
            iCheckCount - 1, vtx.vPosition.x, vtx.vPosition.y, vtx.vPosition.z);
        OutputDebugStringA(szLog);
    }
    sprintf_s(szLog, "=====================================================\n");
    OutputDebugStringA(szLog);

#endif

    m_pDevice->CreateBuffer(&tBufferDesc, &tData, &m_pIB);
}

void CNavigationMgr::Render_Debug(ID3D11DeviceContext* pContext)
{
    if (!m_pVB || !m_pIB) return;

    Shader* pShader = m_pGameInstance->Get_Shader_Prototype(L"Prototype_Component_Shader_Nav");
    if (!pShader) return;

    _float4x4 matWorld, matView, matProj;
    XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
    matView = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_VIEW);
    matProj = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_PROJ);

    pShader->Bind_Matrix("g_WorldMatrix", matWorld);
    pShader->Bind_Matrix("g_ViewMatrix", matView);
    pShader->Bind_Matrix("g_ProjMatrix", matProj);

    pShader->Begin(0);

    pShader->Bind_Resources(0); 

    _uint iStride = sizeof(VTX_NAV);
    _uint iOffset = 0;
    pContext->IASetVertexBuffers(0, 1, &m_pVB, &iStride, &iOffset);
    pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->DrawIndexed(m_iNumIndices, 0, 0);
}

void CNavigationMgr::FindPath(const _float3& vStart, const _float3& vEnd, vector<_float3>& outPath)
{
    outPath.clear();

    if (!m_pNavQuery || !m_pNavMesh) 
        return;

    float startPos[3] = { vStart.x, vStart.y, vStart.z };
    float endPos[3]   = { vEnd.x, vEnd.y, vEnd.z };

    dtPolyRef startRef, endRef;
    float startNearestPt[3];
    float endNearestPt[3];

    float extents[3] = { 2.0f, 4.0f, 2.0f }; 
    dtQueryFilter filter;
    m_pNavQuery->findNearestPoly(startPos, extents, &filter, &startRef, startNearestPt);
    m_pNavQuery->findNearestPoly(endPos, extents, &filter, &endRef, endNearestPt);

    if (!startRef || !endRef) 
        return;

    static const int MAX_POLYS = 256;
    dtPolyRef pathPolys[MAX_POLYS];
    int pathCount = 0;

    m_pNavQuery->findPath(startRef, endRef, startNearestPt, endNearestPt, &filter, 
        pathPolys, &pathCount, MAX_POLYS);

    if (pathCount <= 0) 
        return; 

    static const int MAX_SMOOTH = 256;
    float straightPath[MAX_SMOOTH * 3];     // 결과 좌표들 (x, y, z)
    unsigned char straightPathFlags[MAX_SMOOTH]; // 정점 타입 (시작, 끝, 중간)
    dtPolyRef straightPathRefs[MAX_SMOOTH];      // 각 정점이 속한 폴리곤
    int straightPathCount = 0;

    m_pNavQuery->findStraightPath(startNearestPt, endNearestPt, pathPolys, pathCount,
        straightPath, straightPathFlags, straightPathRefs,
        &straightPathCount, MAX_SMOOTH);

    outPath.reserve(straightPathCount);
    for (int i = 0; i < straightPathCount; ++i)
    {
        _float3 point(straightPath[i * 3], straightPath[i * 3 + 1], straightPath[i * 3 + 2]);
        outPath.push_back(point);
    }
}

void CNavigationMgr::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
