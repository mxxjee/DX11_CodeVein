#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ComputeShader; // 수정됨 : ComputeShader 클래스 전방선언
class StructuredBuffer;
class Shader;
class Model;
class Mesh;
class GameObject;

class GPUCulling final : public Base
{
private:
    explicit GPUCulling(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~GPUCulling();

public:
    HRESULT Initialize();

    /* 씬 로딩 시 1회 호출: 정적 오브젝트 등록 완료 후 GPU 버퍼 빌드 */
    HRESULT Build(
        const UMAP<GameObject*, STATIC_OBJ_INFO>& _umapInfos,
        _uint _maxOutputCount
    );

    /* 매 프레임: 컬링 Dispatch */
    void Execute(const _float4* _frustumPlanes, const _float4& _camPos);

    /* 매 프레임: 인스턴싱 렌더링 */
    void Render_Instanced(_float _timeDelta);

    /* 정리 */
    void Clear();

    /* GPU 버퍼 재빌드 필요 플래그 */
    void Set_Dirty() { m_bDirty = true; }
    _bool Is_Built() const { return m_bBuilt; }

private:
    // Compile_Shaders(), Create_CullCBuffer() 제거 (ComputeShader가 내부에서 처리)
    HRESULT Create_IndirectArgsBuffer(const vector<DRAW_INDEXED_INDIRECT_ARGS>& _args);
    void    Release_AllBuffers();

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

    /* ComputeShader 클래스 활용 (4 Pass) */
    ComputeShader* m_pCS_Cull = { nullptr };
    ComputeShader* m_pCS_Count = { nullptr };
    ComputeShader* m_pCS_BuildArgs = { nullptr };
    ComputeShader* m_pCS_Scatter = { nullptr };

    /* cbuffer 핸들 캐시 (CS_Cull 기준, 전역 Buffer 공유) */
    SHADERHANDLE    m_hCullPlanes = {};
    SHADERHANDLE    m_hCullCamPos = {};
    SHADERHANDLE    m_hCullNumObjects = {};
    SHADERHANDLE    m_hCullNumGroups = {};
    SHADERHANDLE    m_hCullThresholdSq = {};

    /* Instance Rendering Shader */
    Shader* m_pGPUInstanceShader = { nullptr };

    /* StructuredBuffer 기반 리소스 */
    StructuredBuffer* m_pSB_Objects = { nullptr };        // Input: 오브젝트 데이터 (IMMUTABLE)
    StructuredBuffer* m_pSB_GroupDesc = { nullptr };      // Input: 그룹 디스크립터 (IMMUTABLE)
    StructuredBuffer* m_pSB_Visibility = { nullptr };     // Visibility 플래그 (GPU_READWRITE)
    StructuredBuffer* m_pSB_Count = { nullptr };          // 그룹별 카운터 (GPU_READWRITE)
    StructuredBuffer* m_pSB_GroupOffset = { nullptr };    // 그룹별 출력 오프셋 (GPU_READWRITE)
    StructuredBuffer* m_pSB_OutputMatrix = { nullptr };   // Compact 월드 행렬 (GPU_READWRITE)

    /* IndirectArgs (직접 관리, RAW + DRAWINDIRECT) */
    ID3D11Buffer* m_pIndirectArgsBuffer = { nullptr };
    ID3D11UnorderedAccessView* m_pIndirectArgsUAV = { nullptr };

    /* CPU측 렌더링 정보 */
    vector<GPU_CULL_RENDER_GROUP>   m_vecRenderGroups;
    _uint                           m_iNumObjects = {};
    _uint                           m_iNumGroups = {};
    _uint                           m_iTotalArgsEntries = {};
    _uint                           m_iMaxOutputCount = {};
    _bool m_bBuilt = { false };
    _bool m_bDirty = { false };
    _float m_fAccTime = {};

public:
    static GPUCulling* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    void Free() override final;
};

NS_END