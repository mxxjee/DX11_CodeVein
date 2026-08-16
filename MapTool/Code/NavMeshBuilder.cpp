#include "MT_Defines.h"
#include "NavMeshBuilder.h"

void LogNav(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, format, args);
    va_end(args);

    OutputDebugStringA(buffer);
    std::cout << buffer;
}

CNavMeshBuilder::CNavMeshBuilder()
{
    m_ctx = new rcContext();
}

CNavMeshBuilder::~CNavMeshBuilder()
{
    Cleanup();
    if (m_ctx) delete m_ctx;
}

void CNavMeshBuilder::Cleanup()
{
    if (m_navData)
    {
        dtFree(m_navData);
        m_navData = nullptr;
    }
    m_navDataSize = 0;
}

void CNavMeshBuilder::InitializeConfig(rcConfig& cfg, CNavInputGeo* pInputGeo, const NavMeshConfig& Config)
{
    memset(&cfg, 0, sizeof(cfg));

    cfg.cs = Config.cellSize;
    cfg.ch = Config.cellHeight;

    cfg.walkableHeight = (int)ceilf(Config.agentHeight / cfg.ch);
    cfg.walkableClimb = (int)floorf(Config.agentMaxClimb / cfg.ch);
    cfg.walkableRadius = (int)ceilf(Config.agentRadius / cfg.cs);
    cfg.walkableSlopeAngle = Config.agentMaxSlope;

    cfg.minRegionArea = (int)rcSqr(20);
    cfg.mergeRegionArea = (int)rcSqr(20);
    cfg.maxSimplificationError = 1.3f;
    cfg.maxVertsPerPoly = 6;

    cfg.detailSampleDist = 6.0f;
    cfg.detailSampleMaxError = 1.0f;

    const float* bmin = pInputGeo->getMeshBMin();
    const float* bmax = pInputGeo->getMeshBMax();
    rcVcopy(cfg.bmin, bmin);
    rcVcopy(cfg.bmax, bmax);

    rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);
}

bool CNavMeshBuilder::Build(CNavInputGeo* pInputGeo, const NavMeshConfig& config)
{
    Cleanup();

    if (!pInputGeo || pInputGeo->getMeshVertCount() == 0)
    {
        LogNav("[NavError] 입력된 정점이 없습니다.\n");
        return false;
    }

    rcConfig cfg;
    InitializeConfig(cfg, pInputGeo, config);

    // 1. 설정값 확인
    LogNav("\n[Step 1] 설정 확인\n");
    LogNav(" - Grid: %d x %d\n", cfg.width, cfg.height);
    LogNav(" - Walkable Height: %d voxels (%.2f units)\n", cfg.walkableHeight, cfg.walkableHeight * cfg.ch);
    LogNav(" - Walkable Radius: %d voxels (%.2f units)\n", cfg.walkableRadius, cfg.walkableRadius * cfg.cs);
    LogNav(" - Walkable Climb: %d voxels (%.2f units)\n", cfg.walkableClimb, cfg.walkableClimb * cfg.ch);

    // 2. Heightfield 생성
    rcHeightfield* hf = rcAllocHeightfield();
    if (!hf || !rcCreateHeightfield(m_ctx, *hf, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
    {
        LogNav("[NavError] Heightfield 생성 실패 (메모리 부족?)\n");
        return false;
    }
    LogNav("[Step 2] Heightfield 생성 완료\n");

    // 3. 래스터화 (삼각형 -> 복셀)
    const float* verts = pInputGeo->getMeshVerts();
    const int* tris = pInputGeo->getMeshTris();
    int ntris = pInputGeo->getMeshTriCount();

    unsigned char* triAreas = new unsigned char[ntris];
    memset(triAreas, RC_WALKABLE_AREA, ntris * sizeof(unsigned char));

    rcRasterizeTriangles(m_ctx, verts, pInputGeo->getMeshVertCount(), tris, triAreas, ntris, *hf, cfg.walkableClimb);
    delete[] triAreas;
    LogNav("[Step 3] 래스터화 완료 (Triangles: %d)\n", ntris);

    // 4. 필터링
    rcFilterLowHangingWalkableObstacles(m_ctx, cfg.walkableClimb, *hf);
    rcFilterLedgeSpans(m_ctx, cfg.walkableHeight, cfg.walkableClimb, *hf);
    rcFilterWalkableLowHeightSpans(m_ctx, cfg.walkableHeight, *hf);
    LogNav("[Step 4] 필터링 완료\n");

    // 5. Compact Heightfield 생성
    rcCompactHeightfield* chf = rcAllocCompactHeightfield();
    if (!chf || !rcBuildCompactHeightfield(m_ctx, cfg.walkableHeight, cfg.walkableClimb, *hf, *chf))
    {
        LogNav("[NavError] CompactHeightfield 빌드 실패\n");
        rcFreeHeightField(hf);
        return false;
    }
    rcFreeHeightField(hf);
    LogNav("[Step 5] Compact HF 완료 (Spans: %d)\n", chf->spanCount);

    if (chf->spanCount == 0)
    {
        LogNav("[NavError] 걸을 수 있는 바닥이 하나도 없습니다! (설정값 문제)\n");
        LogNav(" -> 팁: Agent Height/Radius를 줄이거나 Max Slope를 높여보세요.\n");
        return false;
    }

    // 6. 영역 침식 및 생성
    if (!rcErodeWalkableArea(m_ctx, cfg.walkableRadius, *chf))
    {
        LogNav("[NavError] 영역 침식 실패\n");
        return false;
    }
    if (!rcBuildDistanceField(m_ctx, *chf) || !rcBuildRegions(m_ctx, *chf, 0, cfg.minRegionArea, cfg.mergeRegionArea))
    {
        LogNav("[NavError] 영역 생성 실패\n");
        return false;
    }
    LogNav("[Step 6] 영역 생성 완료\n");

    // 7. 윤곽선(Contour) 생성
    rcContourSet* cset = rcAllocContourSet();
    if (!cset || !rcBuildContours(m_ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset))
    {
        LogNav("[NavError] Contour 생성 실패\n");
        return false;
    }
    LogNav("[Step 7] Contour 생성 완료 (Contours: %d)\n", cset->nconts);

    if (cset->nconts == 0)
    {
        LogNav("[NavError] 생성된 외곽선이 없습니다. 바닥 면적이 너무 작거나 파편화됨.\n");
        return false;
    }

    // 8. PolyMesh 생성
    rcPolyMesh* pmesh = rcAllocPolyMesh();
    if (!pmesh || !rcBuildPolyMesh(m_ctx, *cset, cfg.maxVertsPerPoly, *pmesh))
    {
        LogNav("[NavError] PolyMesh 생성 실패\n");
        return false;
    }
    LogNav("[Step 8] PolyMesh 생성 완료 (Polys: %d)\n", pmesh->npolys);

    // 9. Detail Mesh 생성
    rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail();
    if (!dmesh || !rcBuildPolyMeshDetail(m_ctx, *pmesh, *chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *dmesh))
    {
        LogNav("[NavError] Detail Mesh 생성 실패\n");
        return false;
    }
    LogNav("[Step 9] Detail Mesh 완료\n");

    rcFreeCompactHeightfield(chf);
    rcFreeContourSet(cset);

    // 10. Detour 데이터 생성
    if (cfg.maxVertsPerPoly > DT_VERTS_PER_POLYGON)
    {
        LogNav("[NavError] VertsPerPoly 설정 오류 (> %d)\n", DT_VERTS_PER_POLYGON);
        return false;
    }

    dtNavMeshCreateParams params;
    memset(&params, 0, sizeof(params));

    params.verts = pmesh->verts;
    params.vertCount = pmesh->nverts;
    params.polys = pmesh->polys;
    params.polyAreas = pmesh->areas;
    params.polyFlags = pmesh->flags;
    params.polyCount = pmesh->npolys;
    params.nvp = pmesh->nvp;

    params.detailMeshes = dmesh->meshes;
    params.detailVerts = dmesh->verts;
    params.detailVertsCount = dmesh->nverts;
    params.detailTris = dmesh->tris;
    params.detailTriCount = dmesh->ntris;

    params.walkableHeight = config.agentHeight;
    params.walkableRadius = config.agentRadius;
    params.walkableClimb = config.agentMaxClimb;
    rcVcopy(params.bmin, pmesh->bmin);
    rcVcopy(params.bmax, pmesh->bmax);
    params.cs = cfg.cs;
    params.ch = cfg.ch;
    params.buildBvTree = true;

    LogNav("[Step 10] Detour 데이터 생성 시도...\n");

    if (!dtCreateNavMeshData(&params, &m_navData, &m_navDataSize))
    {
        LogNav("[NavError] dtCreateNavMeshData 실패! (결과물이 비어있거나 파라미터 오류)\n");
        // 실패 원인 추적
        if (pmesh->nverts == 0 || pmesh->npolys == 0) LogNav("  -> 원인: 생성된 폴리곤이 0개입니다.\n");

        return false;
    }

    rcFreePolyMesh(pmesh);
    rcFreePolyMeshDetail(dmesh);

    LogNav("[Success] NavMesh 빌드 성공! (Size: %d bytes)\n", m_navDataSize);
    return true;
}