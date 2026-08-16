#include "Engine_Define.h"
#include "VIBuffer_Particle.h"

#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Particle::VIBuffer_Particle()
{
}

Engine::VIBuffer_Particle::VIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Particle::VIBuffer_Particle(const VIBuffer_Particle& original)
    : VIBuffer(original)
    , m_InstanceBufferDesc{ original.m_InstanceBufferDesc }
    , m_bIsLoop{ original.m_bIsLoop }
    , m_bIsVerticalFade{ original.m_bIsVerticalFade }
    , m_bIsVerticalShrink{ original.m_bIsVerticalShrink }
    , m_bRadialRotation{ original.m_bRadialRotation }
    , m_bIsBillboard{ original.m_bIsBillboard }
    , m_iNumInstance{ original.m_iNumInstance }
    , m_iMaxInstance{ original.m_iMaxInstance }
    , m_iInstanceVertexStride{ original.m_iInstanceVertexStride }
    , m_iIndexCountPerInstance{ original.m_iIndexCountPerInstance }
    , m_pSpeeds{ nullptr }
    , m_vSize{ original.m_vSize }
    , m_vSpeed{ original.m_vSpeed }
    , m_vRotation{ original.m_vRotation }
    , m_vLifeTime{ original.m_vLifeTime }
    , m_vStartDelay{ original.m_vStartDelay }
    , m_vCenter{ original.m_vCenter }
    , m_vRange{ original.m_vRange }
    , m_vPivot{ original.m_vPivot }
    , m_fAlpha{ original.m_fAlpha }
    , m_fParticleScale{original.m_fParticleScale}
    , m_iDirectionalScale{original.m_iDirectionalScale}
    , m_fGravity{ original.m_fGravity }
    , m_fTransparency { original.m_fTransparency }
    , m_fShakeStrength{ original.m_fShakeStrength }
    , m_fConvergeMaxDist{ original.m_fConvergeMaxDist }
    , m_pInstanceVertices{ nullptr }
    , m_pRotations{ nullptr }
    , m_pRotationSpeeds{ nullptr }
    , m_pAngles{ nullptr }
    , m_matWorldOffset{original.m_matWorldOffset}
{
    if (original.m_pInstanceVertices)
    {
        m_pInstanceVertices = new VTXPARTICLE[m_iNumInstance]{};
        memcpy(m_pInstanceVertices, original.m_pInstanceVertices, sizeof(VTXPARTICLE) * m_iNumInstance);
    }

    if (original.m_pInstanceVerticesGPU)
    {
        m_pInstanceVerticesGPU = new VTXPARTICLEGPU[m_iNumInstance]{};
        memcpy(m_pInstanceVerticesGPU, original.m_pInstanceVerticesGPU, sizeof(VTXPARTICLEGPU) * m_iNumInstance);
    }

    if (original.m_pSpeeds)
    {
        m_pSpeeds = new _float[m_iNumInstance]{};
        memcpy(m_pSpeeds, original.m_pSpeeds, sizeof(_float) * m_iNumInstance);
    }

    if (original.m_pRotations)
    {
        m_pRotations = new _float[m_iNumInstance]{};
        memcpy(m_pRotations, original.m_pRotations, sizeof(_float) * m_iNumInstance);
    }

    if (original.m_pAngles)
    {
        m_pAngles = new _float[m_iNumInstance]{};
        memcpy(m_pAngles, original.m_pAngles, sizeof(_float) * m_iNumInstance);
    }
}

Engine::VIBuffer_Particle::~VIBuffer_Particle()
{
}

VIBuffer_Particle::PARTICLE_DESC Engine::VIBuffer_Particle::Get_ParticleDesc()
{
    PARTICLE_DESC   particleDesc{};
    particleDesc.fParticleScale = m_fParticleScale;
    particleDesc.iDirectionalScale = m_iDirectionalScale;
    particleDesc.fAlpha = m_fAlpha;
    particleDesc.fGravity = m_fGravity;
    particleDesc.iCountX = m_iCountX;
    particleDesc.iCountY = m_iCountY;
    particleDesc.iFrameCount = m_iFrameCount;
    particleDesc.iNumInstance = m_iNumInstance;
    particleDesc.IsBillboard = m_bIsBillboard;
    particleDesc.IsLoop = m_bIsLoop;
    particleDesc.IsVerticalFade = m_bIsVerticalFade;
    particleDesc.IsVerticalShrink = m_bIsVerticalShrink;
    particleDesc.IsRadialRotation = m_bRadialRotation;
    particleDesc.iTransparentIndex = m_iTransparentIndex;
    particleDesc.vCenter = m_vCenter;
    particleDesc.vLifeTime = m_vLifeTime;
    particleDesc.vStartDelay = m_vStartDelay;
    particleDesc.vPivot = m_vPivot;
    particleDesc.vRange = m_vRange;
    particleDesc.vSize = m_vSize;
    particleDesc.vSpeed = m_vSpeed;
    particleDesc.vRotation = m_vRotation;
    particleDesc.fShakeStrength = m_fShakeStrength;
    particleDesc.fConvergeMaxDist = m_fConvergeMaxDist;

    particleDesc.iMaskStartFrame = m_iMaskStartFrame;
    particleDesc.iMaskFrameCount = m_iMaskFrameCount;
    particleDesc.iMaskCountX = m_iMaskCountX;
    particleDesc.iMaskCountY = m_iMaskCountY;
    particleDesc.vMaskUVScroll = m_vMaskUVScroll;

    return particleDesc;
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Particle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::VIBuffer_Particle::Initialize(void* arg)
{
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 버퍼 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Particle::Bind_Resource()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
        m_pVBInstance
    };

    _uint iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride
    };

    _uint iOffsets[] = {
        0,
        0
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);
       
    return S_OK;
}
/******************************************************* 버퍼 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Particle::Render(const _float fTimeDelta)
{
    m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

void Engine::VIBuffer_Particle::Update_Particles(const PARTICLE_DESC& desc)
{
    _uint iMaxInstance = m_InstanceBufferDesc.ByteWidth / m_iInstanceVertexStride;
    _uint iNewCount = desc.iNumInstance;

    _bool bNeedResize = iNewCount != iMaxInstance;
    if (bNeedResize)
    {
        Safe_Delete_Array(m_pInstanceVerticesGPU);
        m_pInstanceVerticesGPU = new VTXPARTICLEGPU[iNewCount]{};

        Safe_Delete_Array(m_pSpeeds);
        m_pSpeeds = new _float[iNewCount]{};

        Safe_Delete_Array(m_pRotations);
        m_pRotations = new _float[iNewCount]{};

        if (m_pAngles)
        {
            Safe_Delete_Array(m_pAngles);
            m_pAngles = new _float[iNewCount]{};
        }

        // IB 갱신
        m_InstanceBufferDesc.ByteWidth = iNewCount * m_iInstanceVertexStride;
        m_iMaxInstance = iNewCount;
    }

    m_iNumInstance = iNewCount;

    m_bIsLoop = (_uint)desc.IsLoop;
    m_bIsBillboard = (_uint)desc.IsBillboard;
    m_bRadialRotation = (_uint)desc.IsRadialRotation;
    m_bIsVerticalFade = (_uint)desc.IsVerticalFade;
    m_bIsVerticalShrink = (_uint)desc.IsVerticalShrink;

    m_vSize = desc.vSize;
    m_vSpeed = desc.vSpeed;
    m_vRotation = desc.vRotation;
    m_vLifeTime = desc.vLifeTime;
    m_vStartDelay = desc.vStartDelay;
    m_vCenter = desc.vCenter;
    m_vRange = desc.vRange;
    m_vPivot = desc.vPivot;
    m_fGravity = desc.fGravity;
    m_fAlpha = desc.fAlpha;
    m_fParticleScale = desc.fParticleScale;
    m_iDirectionalScale = desc.iDirectionalScale;
    m_fConvergeMaxDist = desc.fConvergeMaxDist;
    
    // CS_DUST 변수 설정
    m_fShakeStrength = desc.fShakeStrength;

    // 스프라이트 이미지 변수 설정
    m_iFrameCount = desc.iFrameCount;
    m_iCountX = desc.iCountX;
    m_iCountY = desc.iCountY;
    m_fSizeX = 1.f / (_float)m_iCountX;
    m_fSizeY = 1.f / (_float)m_iCountY;
    m_iTransparentIndex = desc.iTransparentIndex;

    // 마스크 스프라이트 변수 설정
    m_iMaskStartFrame = desc.iMaskStartFrame;
    m_iMaskFrameCount = desc.iMaskFrameCount;
    m_iMaskCountX = desc.iMaskCountX;
    m_iMaskCountY = desc.iMaskCountY;
    m_vMaskUVScroll = desc.vMaskUVScroll;

    for (_uint i = 0; i < m_iNumInstance; i++)
    {
        // 런타임에 전달 받은 구조체 정보를 바탕으로 재설정
        m_pSpeeds[i] = m_pGameInstance->RandomValue(m_vSpeed.x, m_vSpeed.y);
        m_pRotations[i] = m_pGameInstance->RandomValue(m_vRotation.x, m_vRotation.y);

        m_pInstanceVerticesGPU[i].vRight.x = m_vSize.x;
        m_pInstanceVerticesGPU[i].vUp.y = m_vSize.y;
        m_pInstanceVerticesGPU[i].vLook.z = m_vSize.x;

        _float3 vLocalPos = _float3(
            m_pGameInstance->RandomValue(m_vCenter.x - m_vRange.x * 0.5f, m_vCenter.x + m_vRange.x * 0.5f),
            m_pGameInstance->RandomValue(m_vCenter.y - m_vRange.y * 0.5f, m_vCenter.y + m_vRange.y * 0.5f),
            m_pGameInstance->RandomValue(m_vCenter.z - m_vRange.z * 0.5f, m_vCenter.z + m_vRange.z * 0.5f));

        _float3 vWorldPos = Convert_LocalToWorld(vLocalPos);
        m_pInstanceVerticesGPU[i].vTranslation = _float4(vWorldPos.x, vWorldPos.y, vWorldPos.z, 1.f);
        
        _float fStartDelay = m_pGameInstance->RandomValue(m_vStartDelay.x, m_vStartDelay.y);
        m_pInstanceVerticesGPU[i].vLifeTime = _float2(-fStartDelay, m_pGameInstance->RandomValue(m_vLifeTime.x, m_vLifeTime.y));
        m_pInstanceVerticesGPU[i].vRotation = _float2(0.f, m_pGameInstance->RandomValue(m_vRotation.x, m_vRotation.y));

        _float3 vWorldPivot = Convert_LocalToWorld(m_vPivot);

        _float3 vDirection = _float3(m_pInstanceVerticesGPU[i].vTranslation.x - vWorldPivot.x,
                                     m_pInstanceVerticesGPU[i].vTranslation.y - vWorldPivot.y,
                                     m_pInstanceVerticesGPU[i].vTranslation.z - vWorldPivot.z);

        if (m_fConvergeMaxDist > 0.f)
        {
            _float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vDirection)));
            _float fDistRatio = min(fDist / m_fConvergeMaxDist, 1.f);
            m_pInstanceVerticesGPU[i].vRight.x *= fDistRatio;
            m_pInstanceVerticesGPU[i].vUp.y *= fDistRatio;
            m_pInstanceVerticesGPU[i].vLook.z *= fDistRatio;
        }

        XMStoreFloat3(&vDirection, XMVector3Normalize(XMLoadFloat3(&vDirection)));
    }

    // Resize 했으면 기존 버퍼 해제 하고 재생성
    if (bNeedResize)
        Recreate_GPUBuffers();
    else
    {
        // GPU 버퍼에 업데이트
        m_pContext->UpdateSubresource(m_pParticleBuffer, 0, nullptr, m_pInstanceVerticesGPU, 0, 0);

        PARTICLE_INIT* pInitData = new PARTICLE_INIT[m_iMaxInstance];
        for (_uint i = 0; i < m_iMaxInstance; i++)
        {
            pInitData[i].vInitTranslation = m_pInstanceVerticesGPU[i].vTranslation;
            pInitData[i].fSpeed = m_pSpeeds[i];
            pInitData[i].fMaxLifeTime = m_pInstanceVerticesGPU[i].vLifeTime.y;
            pInitData[i].fGravity = m_fGravity;
        }

        m_pContext->UpdateSubresource(m_pSRVBuffer, 0, nullptr, pInitData, 0, 0);
        Safe_Delete_Array(pInitData);
    }
}

void Engine::VIBuffer_Particle::Reset_Particles()
{
    // 모든 파티클들의 LifeTime.x 를 0으로 초기화 하고 위치도 처음걸로 복구시키기
    for (_uint i = 0; i < m_iNumInstance; i++)
    {
        // 위치 복원
        _float3 vLocalPos = _float3(
            m_pGameInstance->RandomValue(m_vCenter.x - m_vRange.x * 0.5f, m_vCenter.x + m_vRange.x * 0.5f),
            m_pGameInstance->RandomValue(m_vCenter.y - m_vRange.y * 0.5f, m_vCenter.y + m_vRange.y * 0.5f),
            m_pGameInstance->RandomValue(m_vCenter.z - m_vRange.z * 0.5f, m_vCenter.z + m_vRange.z * 0.5f));

        _float3 vWorldPos = Convert_LocalToWorld(vLocalPos);
        m_pInstanceVerticesGPU[i].vTranslation = _float4(vWorldPos.x, vWorldPos.y, vWorldPos.z, 1.f);

        // LifeTime 복원
        _float fStartDelay = m_pGameInstance->RandomValue(m_vStartDelay.x, m_vStartDelay.y);
        m_pInstanceVerticesGPU[i].vLifeTime = _float2(-fStartDelay, m_pGameInstance->RandomValue(m_vLifeTime.x, m_vLifeTime.y));

        // Rotation 랜덤
        m_pInstanceVerticesGPU[i].vRotation = _float2(0.f, m_pGameInstance->RandomValue(m_vRotation.x, m_vRotation.y));

        // Speed 랜덤
        m_pSpeeds[i] = m_pGameInstance->RandomValue(m_vSpeed.x, m_vSpeed.y);
    }

    // GPU 버퍼에 업데이트
    m_pContext->UpdateSubresource(m_pParticleBuffer, 0, nullptr, m_pInstanceVerticesGPU, 0, 0);
}

vector<_float4x4> Engine::VIBuffer_Particle::ReadGPU_DeadParticles()
{
    vector<_float4x4> vecDeadParticles;

    // AppendBuffer 카운트 복사,
    // m_pDeadParticleUAV로 부터 m_pCountStagingBuffer에 카운터값(32비트 _uint) 하나가 복사되어 들어간다.
    m_pContext->CopyStructureCount(m_pCountStagingBuffer, 0, m_pDeadParticleUAV);

    // 카운트 읽기
    D3D11_MAPPED_SUBRESOURCE SubResource{};
    CHECK_FAILED(m_pContext->Map(m_pCountStagingBuffer, 0, D3D11_MAP_READ, 0, &SubResource), vecDeadParticles);

    _uint iDeadParticleCount = *static_cast<_uint*>(SubResource.pData);
    m_pContext->Unmap(m_pCountStagingBuffer, 0);

    if (iDeadParticleCount == 0)
        return vecDeadParticles;

    // 데이터 복사
    m_pContext->CopyResource(m_pStagingBuffer, m_pDeadParticleBuffer);

    // 데이터 읽기
    D3D11_MAPPED_SUBRESOURCE MappedData{};
    CHECK_FAILED(m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &MappedData), vecDeadParticles);

    _float4x4* pMatrices = static_cast<_float4x4*>(MappedData.pData);
    vecDeadParticles.reserve(iDeadParticleCount);

    for (_uint i = 0; i < iDeadParticleCount; i++)
        vecDeadParticles.push_back(pMatrices[i]);
    
    m_pContext->Unmap(m_pStagingBuffer, 0);

    return vecDeadParticles;
}

_float3 Engine::VIBuffer_Particle::Convert_LocalToWorld(const _float3& vLocal)
{
    _vector vLocalPos = XMVectorSet(vLocal.x, vLocal.y, vLocal.z, 1.f);
    _matrix matWorld = XMLoadFloat4x4(&m_matWorldOffset);
    _vector vWorldPos = XMVector3TransformCoord(vLocalPos, matWorld);

    _float3 vFinalPos;
    XMStoreFloat3(&vFinalPos, vWorldPos);

    return vFinalPos;
}

HRESULT Engine::VIBuffer_Particle::Bind_EffectResources(Shader* pShader) 
{
    return S_OK;
}

HRESULT Engine::VIBuffer_Particle::Bind_ComputeShaderResources(ComputeShader* pCShader, const _float& fTimeDelta)
{
    return S_OK;
}

/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Particle* Engine::VIBuffer_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    //VIBuffer_Particle* pInstance = new VIBuffer_Particle(pDevice, pContext);

    //MSG_FAIL(pInstance->Initialize_Prototype(), L"VIBuffer_Particle 생성 실패", L"Caution!!!", nullptr);

    return nullptr;
}

Component* Engine::VIBuffer_Particle::Clone(void* arg)
{
    /*VIBuffer_Particle* pInstance = new VIBuffer_Particle(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Particle 복사 실패", L"Caution!!!", nullptr);*/

    return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Particle::Free()
{
    __super::Free();

    Safe_Delete_Array(m_pSpeeds);
    Safe_Delete_Array(m_pInstanceVertices);
    Safe_Delete_Array(m_pInstanceVerticesGPU);
    Safe_Delete_Array(m_pRotations);
    Safe_Delete_Array(m_pRotationSpeeds);
    Safe_Delete_Array(m_pAngles);

    Safe_Release(m_pVBInstance);
    Safe_Release(m_pParticleBuffer);
    Safe_Release(m_pSRVBuffer);
    Safe_Release(m_pParticleSRV);
    Safe_Release(m_pParticleUAV);
    Safe_Release(m_pDeadParticleBuffer);
    Safe_Release(m_pDeadParticleUAV);
    Safe_Release(m_pStagingBuffer);
    Safe_Release(m_pCountStagingBuffer);
}
/******************************************************* 객체 반환 함수 *******************************************************/

