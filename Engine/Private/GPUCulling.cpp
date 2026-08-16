#include "Engine_Define.h"
#include "GPUCulling.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "StructuredBuffer.h"

static constexpr _uint THREAD_GROUP_SIZE = 256;
static constexpr _float DEFAULT_CULL_THRESHOLD_SQ = 0.0004f;

//////////////////////////////////////////////////////// 생성, 소멸 ////////////////////////////////////////////////////////
Engine::GPUCulling::GPUCulling(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

Engine::GPUCulling::~GPUCulling()
{
}
/******************************************************* 생성, 소멸 *******************************************************/



////////////////////////////////////////////////////////  초기화  ////////////////////////////////////////////////////////
HRESULT Engine::GPUCulling::Initialize()
{
    const _wstring shaderPath = L"../../Shader/Shader_GPUCull.hlsl";

    m_pCS_Cull = ComputeShader::Create(m_pDevice, m_pContext, shaderPath, "CS_Cull");
    CHECK_NULLPTR(m_pCS_Cull);

    m_pCS_Count = ComputeShader::Create(m_pDevice, m_pContext, shaderPath, "CS_Count");
    CHECK_NULLPTR(m_pCS_Count);

    m_pCS_BuildArgs = ComputeShader::Create(m_pDevice, m_pContext, shaderPath, "CS_BuildArgs");
    CHECK_NULLPTR(m_pCS_BuildArgs);

    m_pCS_Scatter = ComputeShader::Create(m_pDevice, m_pContext, shaderPath, "CS_Scatter");
    CHECK_NULLPTR(m_pCS_Scatter);

    // cbuffer 핸들 캐시 (전역 Buffer 공유이므로 CS_Cull 하나에서만 조회)
    m_hCullPlanes = m_pCS_Cull->Get_Handle("g_CullPlanes");
    m_hCullCamPos = m_pCS_Cull->Get_Handle("g_CullCamPos");
    m_hCullNumObjects = m_pCS_Cull->Get_Handle("g_CullNumObjects");
    m_hCullNumGroups = m_pCS_Cull->Get_Handle("g_CullNumGroups");
    m_hCullThresholdSq = m_pCS_Cull->Get_Handle("g_CullThresholdSq");

    // GPU 인스턴싱 전용 셰이더 생성
    SHADERENTRY entries[2] = {
        {"VS_MAIN", "PS_MAIN"},
        {"VS_MAIN", "PS_MAIN"}
    };
    SHADERENTRIES shaderEntries = {};
    shaderEntries.pEntries = entries;
    shaderEntries.iNumpass = 2;

    m_pGPUInstanceShader = Shader::Create(
        m_pDevice, m_pContext,
        L"../../Shader/Shader_VtxGPUInstance.hlsl",
        VTXMESH::Elements, VTXMESH::iNumElements,
        shaderEntries
    );
    CHECK_NULLPTR(m_pGPUInstanceShader);

    return S_OK;
}
/*******************************************************  초기화  *******************************************************/



////////////////////////////////////////////////////////  빌드 (씬 로딩 후 1회)  ////////////////////////////////////////////////////////
HRESULT Engine::GPUCulling::Build(
    const UMAP<GameObject*, STATIC_OBJ_INFO>& _umapInfos,
    _uint _maxOutputCount)
{
    Release_AllBuffers();
    m_vecRenderGroups.clear();

    if (_umapInfos.empty())
        return S_OK;

    /* ====== 1단계: ModelID 기준 그룹핑 ====== */
    UMAP<_uint, _uint> umapModelToGroup;
    vector<vector<const STATIC_OBJ_INFO*>> vecGroupedInfos;

    for (auto& pair : _umapInfos)
    {
        const STATIC_OBJ_INFO* pInfo = &pair.second;
        if (pInfo->pModel == nullptr)
            continue;

        _uint modelID = pInfo->iModelTypeID;
        auto it = umapModelToGroup.find(modelID);

        _uint groupIndex = {};
        if (it == umapModelToGroup.end())
        {
            groupIndex = (_uint)vecGroupedInfos.size();
            umapModelToGroup[modelID] = groupIndex;
            vecGroupedInfos.push_back({});
        }
        else
        {
            groupIndex = it->second;
        }

        vecGroupedInfos[groupIndex].push_back(pInfo);
    }

    m_iNumGroups = (_uint)vecGroupedInfos.size();
    if (m_iNumGroups == 0)
        return S_OK;

    /* ====== 2단계: GPU 데이터 구성 ====== */
    vector<GPU_OBJECT_DATA> vecObjectData;
    vector<GPU_GROUP_DESC>  vecGroupDescs;
    vector<DRAW_INDEXED_INDIRECT_ARGS> vecArgs;

    m_vecRenderGroups.resize(m_iNumGroups);
    _uint objectOffset = 0;
    _uint argsOffset = 0;

    for (_uint g = 0; g < m_iNumGroups; ++g)
    {
        auto& groupInfos = vecGroupedInfos[g];
        Model* pModel = groupInfos[0]->pModel;
        _uint meshCount = pModel->Get_NumMeshes();

        // 그룹 디스크립터
        GPU_GROUP_DESC desc = {};
        desc.iFirstObject = objectOffset;
        desc.iObjectCount = (_uint)groupInfos.size();
        desc.iFirstArgsEntry = argsOffset;
        desc.iMeshCount = meshCount;
        vecGroupDescs.push_back(desc);

        // CPU측 렌더링 정보
        m_vecRenderGroups[g].pModel = pModel;
        m_vecRenderGroups[g].iFirstArgsEntry = argsOffset;
        m_vecRenderGroups[g].iMeshCount = meshCount;

        // 오브젝트 데이터
        for (auto& pInfo : groupInfos)
        {
            GPU_OBJECT_DATA objData = {};
            objData.vCenter = pInfo->tAABB.Center;
            objData.vExtents = pInfo->tAABB.Extents;
            objData.fMaxExtent = max(pInfo->tAABB.Extents.x,
                max(pInfo->tAABB.Extents.y, pInfo->tAABB.Extents.z));
            objData.iGroupIndex = g;

            XMStoreFloat4x4(&objData.matWorld, pInfo->pObject->Get_WorldMatrix());
            objData.matWorld._14 = _float(pInfo->pObject->Get_ObjectID());

            vecObjectData.push_back(objData);
        }

        // IndirectArgs (메시별 1개씩)
        for (_uint m = 0; m < meshCount; ++m)
        {
            Mesh* pMesh = pModel->Get_Meshes()[m];

            DRAW_INDEXED_INDIRECT_ARGS args = {};
            args.IndexCountPerInstance = pMesh->Get_NumIndices();
            args.InstanceCount = 0;
            args.StartIndexLocation = 0;
            args.BaseVertexLocation = 0;
            args.StartInstanceLocation = 0;
            vecArgs.push_back(args);
        }

        objectOffset += (_uint)groupInfos.size();
        argsOffset += meshCount;
    }

    m_iNumObjects = (_uint)vecObjectData.size();
    m_iTotalArgsEntries = (_uint)vecArgs.size();
    m_iMaxOutputCount = max(_maxOutputCount, m_iNumObjects);

    /* ====== 3단계: StructuredBuffer 생성 ====== */

    // StructuredBuffer 클래스 활용
    m_pSB_Objects = StructuredBuffer::Create(
        m_pDevice, m_pContext,
        SBUSAGE::IMMUTABLE,
        m_iNumObjects,
        sizeof(GPU_OBJECT_DATA),
        vecObjectData.data()
    );
    CHECK_NULLPTR(m_pSB_Objects);

    m_pSB_GroupDesc = StructuredBuffer::Create(
        m_pDevice, m_pContext,
        SBUSAGE::IMMUTABLE,
        m_iNumGroups,
        sizeof(GPU_GROUP_DESC),
        vecGroupDescs.data()
    );
    CHECK_NULLPTR(m_pSB_GroupDesc);

    m_pSB_Visibility = StructuredBuffer::Create(
        m_pDevice, m_pContext,
        SBUSAGE::GPU_READWRITE,
        m_iNumObjects,
        sizeof(_uint),
        nullptr
    );
    CHECK_NULLPTR(m_pSB_Visibility);

    m_pSB_Count = StructuredBuffer::Create(
        m_pDevice, m_pContext,
        SBUSAGE::GPU_READWRITE,
        m_iNumGroups,
        sizeof(_uint),
        nullptr
    );
    CHECK_NULLPTR(m_pSB_Count);

    m_pSB_GroupOffset = StructuredBuffer::Create(
        m_pDevice, m_pContext,
        SBUSAGE::GPU_READWRITE,
        m_iNumGroups,
        sizeof(_uint),
        nullptr
    );
    CHECK_NULLPTR(m_pSB_GroupOffset);

    m_pSB_OutputMatrix = StructuredBuffer::Create(
        m_pDevice, m_pContext,
        SBUSAGE::GPU_READWRITE,
        m_iMaxOutputCount,
        sizeof(_float4x4),
        nullptr
    );
    CHECK_NULLPTR(m_pSB_OutputMatrix);

    // IndirectArgs만 직접 생성 (RAW + DRAWINDIRECT)
    CHECK_FAILED(Create_IndirectArgsBuffer(vecArgs), E_FAIL);

    m_bBuilt = true;
    m_bDirty = false;

    return S_OK;
}
/*******************************************************  빌드  *******************************************************/



////////////////////////////////////////////////////////  매 프레임 실행  ////////////////////////////////////////////////////////
void Engine::GPUCulling::Execute(const _float4* _frustumPlanes, const _float4& _camPos)
{
    if (!m_bBuilt || m_iNumObjects == 0)
        return;

    // ComputeShader의 Buffer 시스템으로 cbuffer 값 설정
    // 전역 Buffer 공유이므로 m_pCS_Cull 하나에만 설정하면 4개 CS 모두 동일한 값 사용
    m_pCS_Cull->Bind_RawValue_ByHandle(m_hCullPlanes, _frustumPlanes, sizeof(_float4) * 6);
    m_pCS_Cull->Bind_RawValue_ByHandle(m_hCullCamPos, &_camPos, sizeof(_float4));
    m_pCS_Cull->Bind_RawValue_ByHandle(m_hCullNumObjects, &m_iNumObjects, sizeof(_uint));
    m_pCS_Cull->Bind_RawValue_ByHandle(m_hCullNumGroups, &m_iNumGroups, sizeof(_uint));
    _float threshold = DEFAULT_CULL_THRESHOLD_SQ;
    m_pCS_Cull->Bind_RawValue_ByHandle(m_hCullThresholdSq, &threshold, sizeof(_float));

    _uint dispatchCount = (m_iNumObjects + THREAD_GROUP_SIZE - 1) / THREAD_GROUP_SIZE;

    /* ====== Clear ====== */
    UINT clearZero[4] = { 0, 0, 0, 0 };
    m_pContext->ClearUnorderedAccessViewUint(m_pSB_Visibility->Get_UAV(), clearZero);
    m_pContext->ClearUnorderedAccessViewUint(m_pSB_Count->Get_UAV(), clearZero);

    /* ====== Pass 1: Frustum Cull ====== */
    // HLSL register 기준 (t0=Objects, u0=VisibilityOut)
    {
        m_pSB_Objects->Bind_SRV(0, stage_CS);       // t0 : g_Objects
        m_pSB_Visibility->Bind_UAV(0);               // u0 : g_VisibilityOut

        // ComputeShader의 None_UnbiendDispatch 사용 (cbuffer 자동 바인딩 + CS 자동 설정)
        m_pCS_Cull->None_UnbiendDispatch(dispatchCount, 1, 1);

        m_pSB_Visibility->Unbind_UAV(0);
    }

    /* ====== Pass 2: Group Count ====== */
    // HLSL register 기준 (t0=Objects, t2=VisibilityIn, u1=CountBuffer)
    {
        // t0(Objects)은 Pass1에서 이미 바인딩됨, 유지
        m_pSB_Visibility->Bind_SRV(2, stage_CS);    // t2 : g_VisibilityIn
        m_pSB_Count->Bind_UAV(1);                    // u1 : g_CountBuffer

        m_pCS_Count->None_UnbiendDispatch(dispatchCount, 1, 1);

        m_pSB_Visibility->Unbind_SRV(2, stage_CS);
        m_pSB_Count->Unbind_UAV(1);
    }

    /* ====== Pass 3: PrefixSum + IndirectArgs ====== */
    // HLSL register 기준 (t1=GroupDesc, t3=CountRead, u2=GroupOffset, u3=IndirectArgs)
    {
        m_pSB_Objects->Unbind_SRV(0, stage_CS);     // Pass3에서 t0 미사용, 정리

        m_pSB_GroupDesc->Bind_SRV(1, stage_CS);      // t1 : g_GroupDesc
        m_pSB_Count->Bind_SRV(3, stage_CS);          // t3 : g_CountRead
        m_pSB_GroupOffset->Bind_UAV(2);               // u2 : g_GroupOffset

        // u3 : g_IndirectArgs (RAW 버퍼라 직접 바인딩)
        m_pContext->CSSetUnorderedAccessViews(3, 1, &m_pIndirectArgsUAV, nullptr);

        m_pCS_BuildArgs->None_UnbiendDispatch(1, 1, 1);

        m_pSB_GroupDesc->Unbind_SRV(1, stage_CS);
        m_pSB_Count->Unbind_SRV(3, stage_CS);
        m_pSB_GroupOffset->Unbind_UAV(2);

        ID3D11UnorderedAccessView* nullUAV = { nullptr };
        m_pContext->CSSetUnorderedAccessViews(3, 1, &nullUAV, nullptr);
    }

    /* ====== Pass 3-4 사이: CountBuffer 클리어 (scatter 카운터 재사용) ====== */
    m_pContext->ClearUnorderedAccessViewUint(m_pSB_Count->Get_UAV(), clearZero);

    /* ====== Pass 4: Scatter ====== */
    // HLSL register 기준 (t0=Objects, t2=VisibilityIn, t4=OffsetRead, u1=CountBuffer, u4=OutMatrices)
    {
        m_pSB_Objects->Bind_SRV(0, stage_CS);        // t0 : g_Objects
        m_pSB_Visibility->Bind_SRV(2, stage_CS);     // t2 : g_VisibilityIn
        m_pSB_GroupOffset->Bind_SRV(4, stage_CS);    // t4 : g_OffsetRead
        m_pSB_Count->Bind_UAV(1);                     // u1 : g_CountBuffer (scatter 카운터)
        m_pSB_OutputMatrix->Bind_UAV(4);               // u4 : g_OutMatrices

        m_pCS_Scatter->None_UnbiendDispatch(dispatchCount, 1, 1);

        m_pSB_Objects->Unbind_SRV(0, stage_CS);
        m_pSB_Visibility->Unbind_SRV(2, stage_CS);
        m_pSB_GroupOffset->Unbind_SRV(4, stage_CS);
        m_pSB_Count->Unbind_UAV(1);
        m_pSB_OutputMatrix->Unbind_UAV(4);
    }

    /* ====== CS 정리 ====== */
    m_pContext->CSSetShader(nullptr, nullptr, 0);
}
/*******************************************************  매 프레임 실행  *******************************************************/



////////////////////////////////////////////////////////  인스턴싱 렌더링  ////////////////////////////////////////////////////////
void Engine::GPUCulling::Render_Instanced(_float _timeDelta)
{
    if (!m_bBuilt || m_iNumGroups == 0)
        return;

    m_fAccTime += _timeDelta;

    m_pSB_OutputMatrix->Bind_SRV(11, stage_VS);   // t11 : g_InstanceMatrices
    m_pSB_GroupOffset->Bind_SRV(12, stage_VS);     // 추가됨 : t12 : g_GroupOffsets

    m_pGPUInstanceShader->Begin(0);

    for (_uint g = 0; g < m_iNumGroups; ++g)
    {
        GPU_CULL_RENDER_GROUP& renderGroup = m_vecRenderGroups[g];
        Model* pModel = renderGroup.pModel;

        if (pModel == nullptr)
            continue;

        m_pGPUInstanceShader->Bind_RawValue_FullSlot(1, "g_GroupIndex", &g, sizeof(_uint));

        for (_uint m = 0; m < renderGroup.iMeshCount; ++m)
        {
            _uint bitFlag = 0;
            pModel->Bind_Material_FullSlot(m_pGPUInstanceShader, 0, m, aiTextureType_DIFFUSE, 0, &bitFlag);
            pModel->Bind_Material_FullSlot(m_pGPUInstanceShader, 1, m, aiTextureType_NORMALS, 0, &bitFlag);
            pModel->Bind_Material_FullSlot(m_pGPUInstanceShader, 2, m, aiTextureType_METALNESS, 0, &bitFlag);
            pModel->Bind_Material_FullSlot(m_pGPUInstanceShader, 10, m, aiTextureType_EMISSIVE, 0, &bitFlag);
            pModel->Bind_Material_FullSlot(m_pGPUInstanceShader, 3, m, aiTextureType_OPACITY, 0, &bitFlag);
            pModel->Bind_Material_FullSlot(m_pGPUInstanceShader, 6, m, aiTextureType_SHININESS, 0, &bitFlag);

            m_pGPUInstanceShader->Bind_RawValue_ByHandle(g_bitFlag, &bitFlag, sizeof(_uint));
            m_pGPUInstanceShader->Bind_RawValue_ByHandle(g_fTime, &m_fAccTime, sizeof(_float));
            m_pGPUInstanceShader->Commit(0);

            Mesh* pMesh = pModel->Get_Meshes()[m];
            pMesh->Bind_Resource();

            _uint argsIndex = renderGroup.iFirstArgsEntry + m;
            m_pContext->DrawIndexedInstancedIndirect(
                m_pIndirectArgsBuffer,
                argsIndex * sizeof(DRAW_INDEXED_INDIRECT_ARGS)
            );
        }
    }

    m_pSB_OutputMatrix->Unbind_SRV(11, stage_VS);
    m_pSB_GroupOffset->Unbind_SRV(12, stage_VS);   // 추가됨
}
/*******************************************************  인스턴싱 렌더링  *******************************************************/



////////////////////////////////////////////////////////  IndirectArgs 버퍼 (직접 관리)  ////////////////////////////////////////////////////////
HRESULT Engine::GPUCulling::Create_IndirectArgsBuffer(const vector<DRAW_INDEXED_INDIRECT_ARGS>& _args)
{
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(DRAW_INDEXED_INDIRECT_ARGS) * (_uint)_args.size();
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = _args.data();

    CHECK_FAILED(m_pDevice->CreateBuffer(&desc, &initData, &m_pIndirectArgsBuffer), E_FAIL);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = (_uint)_args.size() * 5;
    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

    CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pIndirectArgsBuffer, &uavDesc, &m_pIndirectArgsUAV), E_FAIL);

    return S_OK;
}
/*******************************************************  버퍼 생성  *******************************************************/



////////////////////////////////////////////////////////  정리  ////////////////////////////////////////////////////////
void Engine::GPUCulling::Clear()
{
    Release_AllBuffers();
    m_vecRenderGroups.clear();
    m_iNumObjects = 0;
    m_iNumGroups = 0;
    m_iTotalArgsEntries = 0;
    m_bBuilt = false;
}

void Engine::GPUCulling::Release_AllBuffers()
{
    // 수정됨 : StructuredBuffer Safe_Release
    Safe_Release(m_pSB_Objects);
    Safe_Release(m_pSB_GroupDesc);
    Safe_Release(m_pSB_Visibility);
    Safe_Release(m_pSB_Count);
    Safe_Release(m_pSB_GroupOffset);
    Safe_Release(m_pSB_OutputMatrix);

    // IndirectArgs 직접 해제
    Safe_Release(m_pIndirectArgsUAV);
    Safe_Release(m_pIndirectArgsBuffer);
}
/*******************************************************  정리  *******************************************************/



////////////////////////////////////////////////////////  생성, 소멸  ////////////////////////////////////////////////////////
GPUCulling* Engine::GPUCulling::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    GPUCulling* pInstance = new GPUCulling(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : GPUCulling");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void Engine::GPUCulling::Free()
{
    __super::Free();

    Clear();

    // 수정됨 : ComputeShader Safe_Release (기존 ID3D11ComputeShader* + m_pCullCB 제거)
    Safe_Release(m_pCS_Cull);
    Safe_Release(m_pCS_Count);
    Safe_Release(m_pCS_BuildArgs);
    Safe_Release(m_pCS_Scatter);

    Safe_Release(m_pGPUInstanceShader);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
/*******************************************************  생성, 소멸  *******************************************************/