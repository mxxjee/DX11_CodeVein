#pragma once
#include "Base.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

struct VTX_NAV
{
    _float3 vPosition;
    _float3 vColor;
};

class CNavMeshAccessor : public dtNavMesh
{
public:
    const dtMeshTile* GetTile_Public(int i) const
    {
        return getTile(i);
    }
};

class CNavigationMgr : public Base
{
    DECLARE_SINGLETON(CNavigationMgr)

private:
    CNavigationMgr();
    virtual ~CNavigationMgr() = default;

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pNavMeshPath);

    void Render_Debug(ID3D11DeviceContext* pContext);
    void FindPath(const _float3& vStart, const _float3& vEnd, vector<_float3>& outPath);

    void Free() override final;
private:
    HRESULT LoadNavMesh(const char* path);
    void MakeDebugMesh(); 

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    GameInstance* m_pGameInstance = { nullptr };

    dtNavMesh* m_pNavMesh = nullptr;
    dtNavMeshQuery* m_pNavQuery = nullptr;

    ID3D11Buffer* m_pVB = nullptr;
    ID3D11Buffer* m_pIB = nullptr;
    _uint               m_iNumIndices = 0;
};