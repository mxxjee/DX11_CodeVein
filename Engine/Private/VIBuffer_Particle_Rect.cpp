#include "Engine_Define.h"
#include "VIBuffer_Particle_Rect.h"

#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Particle_Rect::VIBuffer_Particle_Rect()
{
}

Engine::VIBuffer_Particle_Rect::VIBuffer_Particle_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer_Particle(pDevice, pContext)
{
}

Engine::VIBuffer_Particle_Rect::VIBuffer_Particle_Rect(const VIBuffer_Particle_Rect& original)
    : VIBuffer_Particle(original)
{
}

Engine::VIBuffer_Particle_Rect::~VIBuffer_Particle_Rect()
{
}

HRESULT Engine::VIBuffer_Particle_Rect::Create_ComputeBuffers()
{
#pragma region UAV_BUFFER
    D3D11_BUFFER_DESC UAVBufferDesc{};
    UAVBufferDesc.ByteWidth = m_iInstanceVertexStride * m_iNumInstance;
    UAVBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    UAVBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    UAVBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    UAVBufferDesc.StructureByteStride = m_iInstanceVertexStride;

    VTXPARTICLEGPU* pInitData = new VTXPARTICLEGPU[m_iNumInstance];
    for (_uint i = 0; i < m_iNumInstance; i++)
    {
        pInitData[i].vRight = m_pInstanceVerticesGPU[i].vRight;
        pInitData[i].vUp = m_pInstanceVerticesGPU[i].vUp;
        pInitData[i].vLook = m_pInstanceVerticesGPU[i].vLook;
        pInitData[i].vTranslation = m_pInstanceVerticesGPU[i].vTranslation;
        pInitData[i].vLifeTime = m_pInstanceVerticesGPU[i].vLifeTime;
        pInitData[i].vRotation = m_pInstanceVerticesGPU[i].vRotation;
    }

    D3D11_SUBRESOURCE_DATA subResourceUAV{};
    subResourceUAV.pSysMem = pInitData;

    MSG_FAIL(m_pDevice->CreateBuffer(&UAVBufferDesc, &subResourceUAV, &m_pParticleBuffer), L"UAV 버퍼 생성에 실패했습니다.", L"UAV 버퍼 생성 실패.", E_FAIL);

    Safe_Delete_Array(pInitData);

    MSG_FAIL(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance),
        L"Instance 버퍼 생성 실패", L"Instance 버퍼 생성 실패", E_FAIL);
#pragma endregion

#pragma region UAV
    D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
    UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
    UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    UAVDesc.Buffer.FirstElement = 0;
    UAVDesc.Buffer.NumElements = m_iNumInstance;

    MSG_FAIL(m_pDevice->CreateUnorderedAccessView(m_pParticleBuffer, &UAVDesc, &m_pParticleUAV), L"UAV 생성에 실패했습니다.", L"UAV 생성 실패.", E_FAIL);
#pragma endregion


#pragma region DEADPARTICLE_BUFFER
    D3D11_BUFFER_DESC DeadBufferDesc{};
    DeadBufferDesc.ByteWidth = sizeof(_float4x4) * m_iNumInstance;
    DeadBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    DeadBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    DeadBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    DeadBufferDesc.StructureByteStride = sizeof(_float4x4);

    MSG_FAIL(m_pDevice->CreateBuffer(&DeadBufferDesc, nullptr, &m_pDeadParticleBuffer), L"UAV 버퍼 생성에 실패했습니다.", L"UAV 버퍼 생성 실패.", E_FAIL);
#pragma endregion


#pragma region DEADPARTICLE_UAV
    // AppendStructuredBuffer는 생성시 플래그에 D3D11_BUFFER_UAV_FLAG_APPEND 선언을 해줘야 한다고 함.
    D3D11_UNORDERED_ACCESS_VIEW_DESC DeadUAVDesc{};
    DeadUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
    DeadUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    DeadUAVDesc.Buffer.FirstElement = 0;
    DeadUAVDesc.Buffer.NumElements = m_iNumInstance;
    DeadUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

    MSG_FAIL(m_pDevice->CreateUnorderedAccessView(m_pDeadParticleBuffer, &DeadUAVDesc, &m_pDeadParticleUAV), L"UAV 생성에 실패했습니다.", L"UAV 생성 실패.", E_FAIL);
#pragma endregion


#pragma region STAGING_BUFFER
    D3D11_BUFFER_DESC StagingDesc{};
    StagingDesc.ByteWidth = sizeof(_float4x4) * m_iNumInstance;
    StagingDesc.Usage = D3D11_USAGE_STAGING;
    StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    MSG_FAIL(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &m_pStagingBuffer), L"Staging 버퍼 생성에 실패했습니다.", L"Staging 버퍼 생성 실패.", E_FAIL);
#pragma endregion


#pragma region COUNT_BUFFER
    // 카운트 읽기용 버퍼 (4바이트면 충분하다함)
    D3D11_BUFFER_DESC countStagingDesc{};
    countStagingDesc.ByteWidth = sizeof(_uint);
    countStagingDesc.Usage = D3D11_USAGE_STAGING;
    countStagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    MSG_FAIL(m_pDevice->CreateBuffer(&countStagingDesc, nullptr, &m_pCountStagingBuffer), L"Count Staging 버퍼 생성에 실패했습니다.", L"Count Staging 버퍼 생성 실패.", E_FAIL)
#pragma endregion



#pragma region SRV_BUFFER
    /* 입력 버퍼 생성 (SRV용 - 읽기 전용) */
    D3D11_BUFFER_DESC SRVBufferDesc{};
    SRVBufferDesc.ByteWidth = sizeof(PARTICLE_INIT) * m_iNumInstance;
    SRVBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    SRVBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    SRVBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    SRVBufferDesc.StructureByteStride = sizeof(PARTICLE_INIT);

    PARTICLE_INIT* pInitDesc = new PARTICLE_INIT[m_iNumInstance];
    for (_uint i = 0; i < m_iNumInstance; i++)
    {
        pInitDesc[i].vInitTranslation = m_pInstanceVerticesGPU[i].vTranslation;
        pInitDesc[i].fMaxLifeTime = m_pInstanceVerticesGPU[i].vLifeTime.y;
        pInitDesc[i].fSpeed = m_pSpeeds[i];
        pInitDesc[i].fStartDelay = m_pGameInstance->RandomValue(m_vStartDelay.x, m_vStartDelay.y);
        pInitDesc[i].fGravity = m_fGravity;
    }

    D3D11_SUBRESOURCE_DATA subResourceSRV{};
    subResourceSRV.pSysMem = pInitDesc;

    MSG_FAIL(m_pDevice->CreateBuffer(&SRVBufferDesc, &subResourceSRV, &m_pSRVBuffer), L"SRV 버퍼 생성에 실패했습니다.", L"SRV 버퍼 생성 실패.", E_FAIL);

    Safe_Delete_Array(pInitDesc);
#pragma endregion

#pragma region SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVDesc.Buffer.FirstElement = 0;
    SRVDesc.Buffer.NumElements = m_iNumInstance;

    MSG_FAIL(m_pDevice->CreateShaderResourceView(m_pSRVBuffer, &SRVDesc, &m_pParticleSRV), L"SRV 생성에 실패했습니다.", L"SRV 생성 실패.", E_FAIL);
#pragma endregion



    return S_OK;
}

HRESULT Engine::VIBuffer_Particle_Rect::Recreate_GPUBuffers()
{
    // 기존 GPU 버퍼들 전부 해제
    Safe_Release(m_pVBInstance);
    Safe_Release(m_pParticleBuffer);
    Safe_Release(m_pParticleUAV);
    Safe_Release(m_pSRVBuffer);
    Safe_Release(m_pParticleSRV);
    Safe_Release(m_pDeadParticleBuffer);
    Safe_Release(m_pDeadParticleUAV);
    Safe_Release(m_pStagingBuffer);
    Safe_Release(m_pCountStagingBuffer);

    // Resize 된 개수로 재생성
    return Create_ComputeBuffers();
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Particle_Rect::Initialize_Prototype(const PARTICLE_DESC* _desc)
{
    m_iVertexStride = sizeof(VTXPOSTEX);	// 한 정점의 크기(바이트)
    m_iNumVertices = 4;		// 정점의 개수 (사각형이니까 4개)
    m_iIndexStride = 4;		// 인덱스 하나의 크기(바이트)

    m_iNumIndices = 6;		// 인덱스 개수
    m_iNumVertexBuffers = 2; // 버텍스 버퍼의 개수(메인 버텍스 버퍼와 버텍스 인스턴스 버퍼)
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


    const PARTICLE_RECT_DESC* particleDesc = CAST(const PARTICLE_RECT_DESC*)(_desc);

    m_bIsLoop = (_uint)particleDesc->IsLoop;
    m_bIsBillboard = (_uint)particleDesc->IsBillboard;
    m_iNumInstance = particleDesc->iNumInstance;
    m_iMaxInstance = m_iNumInstance;
    m_iInstanceVertexStride = sizeof(VTXPARTICLEGPU);
    m_iIndexCountPerInstance = m_iNumIndices;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// 버퍼 전체의 크기 = 정점 하나의 바이트 * 정점 갯수
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


#pragma region INSTANCE_BUFFER
    m_InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;  // 인스턴스 버퍼의 크기 = 인스턴스 개수 * 인스턴스 하나의 크기
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceBufferDesc.CPUAccessFlags = 0;
    m_InstanceBufferDesc.MiscFlags = 0;
    m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

    m_pInstanceVerticesGPU = new VTXPARTICLEGPU[m_iNumInstance]{};

    m_pSpeeds = new _float[m_iNumInstance]{};

    m_pRotations = new _float[m_iNumInstance]{};

    m_bIsLoop = particleDesc->IsLoop;
    m_bIsBillboard = particleDesc->IsBillboard;
    m_bIsVerticalFade = particleDesc->IsVerticalFade;

    m_vSize = particleDesc->vSize;
    m_vSpeed = particleDesc->vSpeed;
    m_vRotation = particleDesc->vRotation;
    m_vLifeTime = particleDesc->vLifeTime;
    m_vStartDelay = particleDesc->vStartDelay;
    m_vCenter = particleDesc->vCenter;
    m_vRange = particleDesc->vRange;
    m_vPivot = particleDesc->vPivot;
    m_fGravity = particleDesc->fGravity;
    m_fAlpha = particleDesc->fAlpha;
    m_fParticleScale = particleDesc->fParticleScale;

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        m_pSpeeds[i] = m_pGameInstance->RandomValue(m_vSpeed.x, m_vSpeed.y); // 대입한 값의 x~y 사이의 랜덤값으로 speed결정
        m_pRotations[i] = m_pGameInstance->RandomValue(m_vRotation.x, m_vRotation.y);

        m_pInstanceVerticesGPU[i].vRight = _float4(m_vSize.x, 0.f, 0.f, 0.f);
        m_pInstanceVerticesGPU[i].vUp    = _float4(0.f, m_vSize.y, 0.f, 0.f);
        m_pInstanceVerticesGPU[i].vLook  = _float4(0.f, 0.f, m_vSize.x, 0.f);
        m_pInstanceVerticesGPU[i].vTranslation = _float4(
            m_pGameInstance->RandomValue(m_vCenter.x - m_vRange.x * 0.5f, m_vCenter.x + m_vRange.x * 0.5f)
          , m_pGameInstance->RandomValue(m_vCenter.y - m_vRange.y * 0.5f, m_vCenter.y + m_vRange.y * 0.5f)
          , m_pGameInstance->RandomValue(m_vCenter.z - m_vRange.z * 0.5f, m_vCenter.z + m_vRange.z * 0.5f)
          , 1.f);

        _float fStartDelay = m_pGameInstance->RandomValue(m_vStartDelay.x, m_vStartDelay.y);
        m_pInstanceVerticesGPU[i].vLifeTime = _float2(-fStartDelay, m_pGameInstance->RandomValue(m_vLifeTime.x, m_vLifeTime.y));
        m_pInstanceVerticesGPU[i].vRotation = _float2(0.f, m_pGameInstance->RandomValue(m_vRotation.x, m_vRotation.y));

        _float3 vDirection = _float3(m_pInstanceVerticesGPU[i].vTranslation.x - m_vPivot.x,
                                     m_pInstanceVerticesGPU[i].vTranslation.y - m_vPivot.y,
                                     m_pInstanceVerticesGPU[i].vTranslation.z - m_vPivot.z);

        XMStoreFloat3(&vDirection, XMVector3Normalize(XMLoadFloat3(&vDirection)));
    }
#pragma endregion

    CHECK_FAILED(Create_ComputeBuffers(), E_FAIL);

    //if (m_bIsAnimated)
    //    *m_pSkinnedVertexPositions = *m_pVertexPositions;

    return S_OK;
}

HRESULT Engine::VIBuffer_Particle_Rect::Initialize(void* arg)
{
    CHECK_FAILED(Create_ComputeBuffers(), E_FAIL);

    return S_OK;
}

HRESULT Engine::VIBuffer_Particle_Rect::Bind_EffectResources(Shader* pShader)
{
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_fTransparency, &m_fTransparency, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_fAlpha, &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_fParticleScale, &m_fParticleScale, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iDirectionalScale, &m_iDirectionalScale, sizeof(_int)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_bIsBillboard, &m_bIsBillboard, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_bVerticalFade, &m_bIsVerticalFade, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_bVerticalShrink, &m_bIsVerticalShrink, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_bRadialRotation, &m_bRadialRotation, sizeof(_uint)), E_FAIL);
    _float3 vFinalPivot = Convert_LocalToWorld(m_vPivot);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_vEffectPivot, &vFinalPivot, sizeof(_float3)), E_FAIL);

    // 스프라이트 바인딩
    _float2 vScaleUV = _float2(1.f / (_float)m_iCountX, 1.f / (_float)m_iCountY);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_vScaleUV, &vScaleUV, sizeof(_float2)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iFrameCount, &m_iFrameCount, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iCountX, &m_iCountX, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iCountY, &m_iCountY, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iTransparentIndex, &m_iTransparentIndex, sizeof(_uint)), E_FAIL);

    // 마스크 스프라이트 바인딩
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iMaskStartFrame, &m_iMaskStartFrame, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iMaskFrameCount, &m_iMaskFrameCount, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iMaskCountX, &m_iMaskCountX, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_iMaskCountY, &m_iMaskCountY, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_ByHandle(g_vMaskUVScroll, &m_vMaskUVScroll, sizeof(_float2)), E_FAIL);

    return S_OK;
}

HRESULT Engine::VIBuffer_Particle_Rect::Bind_ComputeShaderResources(ComputeShader* pCShader, const _float& fTimeDelta)
{
    // 시간 누적
    m_fAccumulatedTime += fTimeDelta;

    // 상수 버퍼 바인딩
    _float3 vFinalPivot = Convert_LocalToWorld(m_vPivot);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_vPivot, &vFinalPivot, sizeof(_float3)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_fTimeDelta, &fTimeDelta, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_iNumParticles, &m_iNumInstance, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_bIsLoop, &m_bIsLoop, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_fAccumulatedTime, &m_fAccumulatedTime, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_fShakeStrength, &m_fShakeStrength, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_fConvergeMaxDist, &m_fConvergeMaxDist, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_RawValue_ByHandle(g_bFollowParent, &m_bFollowParent, sizeof(_uint)), E_FAIL);
    CHECK_FAILED(pCShader->Bind_Matrix_ByHandle(g_matParticleWorld, m_matWorldOffset), E_FAIL);
    CHECK_FAILED(pCShader->Bind_Matrix_ByHandle(g_matPrevParticleWorld, m_matPrevWorldOffset), E_FAIL);

    // UAV, SRV 바인딩
    CHECK_FAILED(pCShader->Bind_SRV_FullSlot(0, m_pParticleSRV), E_FAIL);
    ID3D11UnorderedAccessView* pUAVs[2] =
    {
        m_pParticleUAV,         // u0 : 파티클 데이터
        m_pDeadParticleUAV      // u1 : 사망한 파티클 (AppendBuffer)
    };

    _uint iInitialCount[2] = 
    { 
        (_uint)-1,              // u0 : 카운트를 변경 안함 (일반 RWStructuredBuffer)
        0                       // u1 : 카운트를 0으로 리셋 (매 프레임 새로 시작)
    };

    m_pContext->CSSetUnorderedAccessViews(0, 2, pUAVs, iInitialCount);

    // Dispatch
    _uint iGroupCount = (m_iNumInstance + 255) / 256;
    pCShader->Dispatch(iGroupCount, 1, 1);

    // CS 결과를 렌더링용 VB에 복사
    m_pContext->CopyResource(m_pVBInstance, m_pParticleBuffer);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Particle_Rect* Engine::VIBuffer_Particle_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC* _desc)
{
    VIBuffer_Particle_Rect* pInstance = new VIBuffer_Particle_Rect(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_desc), L"VIBuffer_Particle_Rect 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::VIBuffer_Particle_Rect::Clone(void* arg)
{
    VIBuffer_Particle_Rect* pInstance = new VIBuffer_Particle_Rect(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Particle_Rect 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Particle_Rect::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

