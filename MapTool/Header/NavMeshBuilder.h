#pragma once
#include "NavInputGeo.h"
#include "Recast/Recast.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshBuilder.h"

struct NavMeshConfig
{
    // 에이전트크기
    float agentHeight = 2.0f;       // 키
    float agentRadius = 0.6f;       // 반지름 (뚱뚱한 정도)
    float agentMaxClimb = 0.9f;     // 계단 등반 높이
    float agentMaxSlope = 45.0f;    // 등반 가능한 경사각 (도)

    // 정밀도 관련
    float cellSize = 0.3f;          // 복셀 크기
    float cellHeight = 0.2f;        // 복셀 높이
};

class CNavMeshBuilder
{
public:
    CNavMeshBuilder();
    ~CNavMeshBuilder();

    bool Build(class CNavInputGeo* pInputGeo, const NavMeshConfig& config);

    unsigned char* GetNavMeshData() const { return m_navData; }
    int GetNavMeshDataSize() const { return m_navDataSize; }

private:
    void InitializeConfig(rcConfig& cfg, CNavInputGeo* pInputGeo, const NavMeshConfig& Config);

    void Cleanup();

private:
    unsigned char* m_navData = nullptr;
    int m_navDataSize = 0;

    class rcContext* m_ctx = nullptr;
};