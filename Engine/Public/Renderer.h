#pragma once

/* 그릴 녀석들을 프레임마다 컨테이너에 저장 */
/* 그리는 타이밍에 그릴 순서에 따라서 객체들을 그려줌 */
/* 순서는 PRIORITY, NON_BLEND, BLEND, UI */
/* BLEND 그릴때는 멀리 있는것부터 그림 */
/* 그리기가 끝난 이후에는 컨테이너를 비워줌 */
/* 프로그램 종료시에 컨테이너에 최대 몇개의 오브젝트가 들어갔는지 출력(추후 사용용) */

/* Priority Group(SkyBox) : Z-Test (상관없음), Z-Write (OFF) */
/* NON_BLEND Group : Z-Test(ON), Z-Write(ON) */
/* BLEND Group : Z-Test(ON), Z-Write(OFF) */
/* UI Group : Z-Test(상관없음), Z-Write(상관없음) */

#ifdef _DEBUG
#include <profileapi.h>

struct CpuTimer
{
    LARGE_INTEGER start = {};
    LARGE_INTEGER end = {};
    LARGE_INTEGER frequency = {};

    void Begin()
    {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
    }

    /* ms 단위로 반환 */
    double End()
    {
        QueryPerformanceCounter(&end);
        return (double)(end.QuadPart - start.QuadPart) / (double)frequency.QuadPart * 1000.0;
    }
};

struct GpuProfiler
{
    ID3D11Query* pDisjoint = {};
    ID3D11Query* pStart = {};
    ID3D11Query* pEnd = {};
    ID3D11Device* pDevice = {};
    ID3D11DeviceContext* pContext = {};

    /* 초기화 Renderer::Initialize()에서 호출 */
    HRESULT Init(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    {
        pDevice = _pDevice;
        pContext = _pContext;

        D3D11_QUERY_DESC desc = {};

        desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        pDevice->CreateQuery(&desc, &pDisjoint);

        desc.Query = D3D11_QUERY_TIMESTAMP;
        pDevice->CreateQuery(&desc, &pStart);
        pDevice->CreateQuery(&desc, &pEnd);

        return S_OK;
    }

    void Begin()
    {
        pContext->Begin(pDisjoint);
        pContext->End(pStart); // timestamp는 End로 기록
    }

    /* GPU ms 반환. -1이면 아직 결과 없음 */
    double End()
    {
        pContext->End(pEnd);
        pContext->End(pDisjoint);

        // [AI] GPU가 완료될 때까지 대기 (프로파일링 전용)
        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData = {};
        while (pContext->GetData(pDisjoint, &disjointData,
            sizeof(disjointData), 0) == S_FALSE)
        {
            Sleep(0);
        }

        if (disjointData.Disjoint)
            return -1.0;

        UINT64 startTime = 0, endTime = 0;
        pContext->GetData(pStart, &startTime, sizeof(UINT64), 0);
        pContext->GetData(pEnd, &endTime, sizeof(UINT64), 0);

        return (double)(endTime - startTime) / (double)disjointData.Frequency * 1000.0;
    }

    void Release()
    {
        if (pDisjoint) { pDisjoint->Release(); pDisjoint = nullptr; }
        if (pStart) { pStart->Release(); pStart = nullptr; }
        if (pEnd) { pEnd->Release(); pEnd = nullptr; }
    }
};
#endif // _DEBUG


#include "Base.h"

NS_BEGIN(Engine)

class Occlusion;
class HiZBuffer;
class GPUCulling;

class Renderer final : public Base
{
private:
    explicit Renderer();
    explicit Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Renderer();

    struct Camera_Buffer {
        _float4x4 g_ViewMatrix = {};
        _float4x4 g_ProjMatrix = {};
        _float4x4 g_PrevViewMatrix = {};
        _float4x4 g_PrevProjMatrix = {};
        _float4x4 g_InverseViewMatrix = {};
        _float4x4 g_InverseProjMatrix = {};
        _float4 g_vCamPosition = {};

        //SSAO
        _float4 g_Samples[32] = {};
        _float2 g_NoiseScale = {};
        _float g_SSAORadius = {};
        _float g_SSAOBias = {};
        _float4x4 g_CamViewMatrix = {};
        _float4x4 g_CamProjMatrix = {};
    };

    typedef struct LineDescription {
        _float3 vStartPos = {};
        _float3 vEndPos = {};
        _float4 vColor = { 1.f, 1.f, 1.f, 1.f };
    }LINE_DESC;

public:
    HRESULT Initialize();
    HRESULT Ready_DepthStencilView();
    HRESULT Ready_DepthStencilViewCasCade();
    HRESULT Ready_Shader();
    HRESULT Ready_TextureSRV(const _float fWinCX, const _float fWinCY);
    HRESULT Ready_TextureSRV_For_BlurUI(const _float fWinCX, const _float fWinCY);

    HRESULT Add_RenderObject(RENDER_GROUP _rendergroup, class GameObject* _gameobject);

    HRESULT Render(const _float fTimeDelta);
    HRESULT Clear_Renderer();
    // 렌더타겟 크기 변경
    HRESULT ResizeBuffers(_float _width, _float _height);

    void Add_InstanceBatch(const wstring& strProtoTag, class Model* pModel, _float4x4 WorldMatrix);

    //쉐이더 값 조절용
    void Set_HDROption(_float4 _vHDROption) { m_vHDROption = _vHDROption; }
    //SSAO값조절용
    void Set_SSAOOption(_float4 _vSSAOOption) { m_vSSAOOption = _vSSAOOption; }
    //ColorGrading값 조절용
    void Set_ShaderDesc(ShaderDesc _ShaderDesc) { m_vShaderDesc = _ShaderDesc; }
    //void Set_FogOption(_float4 _vFogOption) { m_vFogOption = _vFogOption; }

    void Remove_RenderObject(class GameObject* pGameObject);

    //맵 바꿔주는거
    void ChangeMap(_float _t); //지하 -> 지상 변경
    void ChangeBaseMap(); //베이스씬
    void ChangeCaveMap(_float _t); //동굴 -> 빛있는곳
    void ChangeCustomize(); //커마씬
    void ChangeMainMap(); //처음시작(동굴)
    void ChangeChurch();
    void ChangeChurchBoss();
    void ChangeEnd();
    _float3 LerpFloat3(const _float3& _a, const _float3& _b, float _t);

    //플레이어 스킨컬러가져오기
    _float4 Get_PlayerSkinColor() { return _float4(m_vShaderDesc.vSkinTint.x,m_vShaderDesc.vSkinTint.y, m_vShaderDesc.vSkinTint.z,1.f); }
    void Set_PlayerSkinColor(_float3 vColor) { m_vShaderDesc.vSkinTint = vColor; }
private:
    // 가장 먼저 그려질 객체들(스카이박스, 배경 등)
    void Render_Priority(const _float fTimeDelta);
    // 그림자를 그리기 위한 RT을 먼저 그려줌
    void Render_Shadow(const _float fTimeDelta);
    void Render_Shadow_Bake(const _float fTimeDelta);
    // 블렌딩이 들어가지 않을 객체들
    void Render_NonBlend(const _float fTimeDelta);
    // 노말 텍스쳐를 기준으로 빛을을 계산할 객체들
    void Render_Lights(const _float fTimeDelta);
    //갓레이
    void Render_GodRayCopy(const _float fTimeDelta);
    void Render_GodRay(const _float fTimeDelta);
    void Render_GodRayMesh(const _float fTImeDelta);

    // 합성 단계
    void Render_Combined(const _float fTimeDelta);
    void Render_NonLight(const _float fTimeDelta);
    void Render_BlobShadow(const _float fTimeDelta);
    void Render_Decal(const _float fTimeDelta);
    void Render_Effect_NonBlend(const _float fTimeDelta);
    void Render_Effect_Blend(const _float fTimeDelta);
    void Render_Blend(const _float fTimeDelta);
    void Render_UI(const _float fTimeDelta);
    void Render_Camera(const _float fTimeDelta);
    void Render_Instancing(const _float fTimeDelta);
    void Render_WorldUI(const _float fTimeDelta);

    //톤맵핑전에 HDR가져와서 먹이기
    void Render_CamVelocity(const _float fTimeDelta);
    void Render_CamMotionBLur(const _float fTimeDelta); //이후 11번자리 원래 HDR인데 CamMotionBlur로 교체

    void Render_HDR_Copy(const _float fTimeDelta);
    void Render_HDR_BrightExtract(const _float fTimeDelta);
    void Render_HDR_DownSampling(const _float fTimeDelta);
    void Render_HDR_BlurHorizontal(const _float fTimeDelta);
    void Render_HDR_BlurVertical(const _float fTimeDelta);
    void Render_HDR_UPSamplingPlus(const _float fTimeDelta);
    void Render_HDR_UpSampling(const _float fTimeDelta);
    void Render_HDR(const _float fTimeDelta);
        
    void Render_SSAO(const _float fTimeDelta);
    void Render_SSAO_Blur(const _float fTimeDelta);

    void Setup_ViewportDesc(_float _width, _float _height);

    void Ready_SSAOSetUp();

public:
    void Render_Grid();
    void Set_Capture(_bool _bCapture)
    {
        m_bCapture = _bCapture;
    }
#ifdef _DEBUG
public:
    HRESULT Initialize_DebugDraw();
    HRESULT Add_DebugComponent(class Component* pComponent);

    // Collider나 Debug 렌더 여기서 모아서 할거임
    void Debug_Render_Begin();
    void Debug_Render_End();
    PrimitiveBatch<VertexPositionColor>* Get_DebugBatch() { return m_pDebugBatch; }

private:
    void Render_Debug(const _float fTimeDelta);
    void Render_DebugRTV(const _float fTimeDelta);
    void Render_DebugSphere();
    void Render_DebugLookLine();
    void Render_Debug_PhysX();

public:
    struct DebugDraw {
        BoundingSphere Sphere;
        _float3 vColor = {};
    };

    void Add_Debug_Sphere(BoundingSphere _sphere, _float3 _color);
    
    void Render_DebugCapsule();
    void Add_Debug_Capsule(CAPSULE_DESC _capsule);
    void Add_Debug_Capsule(_float3& _center, _float _radius, _float _halfHeight, _float4& _quaternion, _float3& _color);
    _float4 PhysX_Color_To_Float4(PxU32 _color);
    void Add_Debug_LookLine(_float3& _pos, _float3& _target, _float4& _color);
    void Add_Debug_Fan(const DebugFan& _fan);
    void Render_DebugFan();


private:
    vector<class Component*> m_DebugComponents;

    // 디버그 Draw용 리소스
    // 이거 하나로 다 그릴거임 븅슨같이 Collider마다 가지고 있게 할게 아니라
    PrimitiveBatch<VertexPositionColor>* m_pDebugBatch = { nullptr };
    BasicEffect* m_pDebugEffect = { nullptr };
    ID3D11InputLayout* m_pDebugInputLayout = { nullptr };
    vector<DebugDraw> m_vecDebugSphere;
    vector<DEBUG_CAPSULE_DESC> m_vecDebugCapsule;
    vector<LINE_DESC> m_vecDebugLine;
    vector<DebugFan> m_vecDebugFan;
#endif // _DEBUG

#pragma region OcTree(옥트리)
public:
    // 옥트리 생성 + 초기화(레벨 시작 시)
    HRESULT Build_OcTree(const _float3& _worldCenter, _float _worldHalfSize, _uint _maxDepth);

    // 정적 오브젝트 OcTree에 추가(레벨 로드할 때 한 번만 호출{등록}함)
    HRESULT Add_StaticObject(RENDER_GROUP _group, class GameObject* _staticobject, const BoundingBox& _worldAABB);

    // 정적 오브젝트 개별 삭제용
    HRESULT Remove_StaticObject(GameObject* _staticObject);

    /* 범위 쿼리: 특정 AABB와 겹치는 오브젝트들 반환 */
    void Query_AABB(const BoundingBox& _aabb, vector<const STATIC_OBJ_INFO*>& _outInfos) const;


    // 옥트리 정리
    void Clear_OcTree();

private:
    // 매 프레임: Octree에서 프러스텀 안의 정적 오브젝트만 렌더 리스트에 추가
    void Query_StaticObjects_ForInstancing();

    void Render_NonBlend_Instanced(const _float fTimeDelta);
#pragma endregion OcTree(옥트리)
public:
    struct INSTANCE_BATCH_DATA
    {
        Model* pMainModel = nullptr;
        vector<_float4x4>   vecInstanceMatrices;
    };



private:
    vector<GameObject*> m_vecObjects[UINT(RENDER_GROUP::END)];
    _uint               m_iMaxElementCount = {};
    _uint               m_iMaxGroupElementCount[UINT(RENDER_GROUP::END)] = {};

    // 그리기용
    class Shader*                   m_pShader = { nullptr };
    class VIBuffer_Rect*            m_pVIBuffer = { nullptr };
    class Shader*                   m_pOnlyBindingShader = { nullptr };
    _float4x4                       m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
    ID3D11ShaderResourceView*       m_pSRV = { nullptr };
    ID3D11DepthStencilView*         m_pDSV = { nullptr };
    vector<ID3D11DepthStencilView*> m_pDSVCascade = { nullptr };
    
    _uint   m_iViewportWidth = {};
    _uint   m_iViewportHeight = {};

    _float  m_fShadowMapOffset = { 1.f };

    Camera_Buffer m_tCameraBuffer;

    /* 옥트리 컬링 적용용 */
    class OcTree* m_pOctree = { nullptr };

    // 정적 오브젝트 구조체(동적 오브젝트는 옥트리 적용 X)
    UMAP<GameObject*, STATIC_OBJ_INFO> m_umapStaticObjectInfo;

    UMAP<wstring, INSTANCE_BATCH_DATA> m_InstanceBatches;

    /* 옥트리 + 인스턴싱 */
    class InstanceBuffer*   m_pInstanceBuffer = { nullptr };
    class Shader*           m_pInstanceShader = { nullptr };

    struct InstanceGroup
    {
        Model* pModel = { nullptr };           // 이 그룹의 Model (1개만)
        vector<GameObject*> vecObjects = {};   // 오브젝트들
    };
    UMAP<_uint, InstanceGroup> m_umapInstanceGroup;
    vector<InstanceGroup> m_vecInstanceGroup;
    vector<GameObject*> m_vecVisibleObjects;
    vector<const STATIC_OBJ_INFO*> m_vecQueryInfos;
    vector<_float4x4> m_vecMatrices;
    _uint m_iMaxInstanceNum = {};

    /* 오클루전 */
    Occlusion* m_pOcclusion = { nullptr };
    _float m_fOccluderAreaThreshold = { 20.f }; // 튜닝값
    _uint m_iocclusionFrameCount = {};

    // HiZBuffer* m_pHiZBuffer = { nullptr };

    //WGpuProfiler m_gpuProfiler; /* 성능 프로파일링용 */
    //CpuTimer m_CpuProfiler;

    //그림자용ㄹ
    _int        m_iNumDSV = {};
    _float      m_iShadowSize = {};
    _float      m_iShodowBakeSize = {};
    _bool m_bCapture = {};
    ID3D11DepthStencilView** m_pDSVShadow = { nullptr };
    Shader* m_pShaderShadowCom = { nullptr };
    
    //HDR용
    _float4 m_vHDROption = {};

    // Scene Capture 용
    ID3D11Texture2D*            m_pSceneTexture = { nullptr };      // 백 버퍼 복사용
    ID3D11ShaderResourceView*   m_pSceneSRV = { nullptr };          // 쉐이더에서 읽은 SRV

    //Emission Animation용
    _float m_fAccTime = {};

    //SSAO용
    _float4 ssaoKernel[32];
    _int m_iSSAO_Size;
    default_random_engine generator; //난수엔진(값넣으면 실제 랜던 값)
    uniform_real_distribution<_float> dist; //균등하게 실수를 뽑아줌(기존랜덤보다 성능좋음)
    _float4 m_vSSAOOption = {};

    //ColorGrading용
    ShaderDesc m_vShaderDesc;

    //전환용
    ZoneDesc m_ZonDesc[3];  


private:
     /*ui블러시 저장용*/
    ID3D11Texture2D*            pBackTex2D = nullptr;
    ID3D11Texture2D*        m_pSceneTexture_ForUI = nullptr;
    ID3D11Resource*         pBackRes = nullptr;
    ID3D11RenderTargetView* m_pBackRTV = nullptr;
    ID3D11ShaderResourceView* m_pSceneSRV_ForUI = nullptr;

private:
    ID3D11Device*               m_pDevice = { nullptr };
    ID3D11DeviceContext*        m_pContext = { nullptr };
    class GameInstance*         m_pGameInstance = { nullptr };

public:
    static Renderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override final;



private:
    void        Captured_Blur_At_OneFrame();
    _bool       m_bCaptured_Blur_At_OneFrame = false;
    //딱한프레임에!!!!블러 처리된 HDR, 다른 ui들 까지 그려진 화면을 가져오도록하기 위한 불변수
};

NS_END
