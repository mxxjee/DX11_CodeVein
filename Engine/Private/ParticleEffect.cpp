#include "Engine_Define.h"
#include "ParticleEffect.h"
#include "VIBuffer_Particle.h"
#include "Shader.h"
#include "Texture.h"
#include "GameInstance.h"
#include "SubEmitter.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::ParticleEffect::ParticleEffect()
{
}

Engine::ParticleEffect::ParticleEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : GameObject{ pDevice, pContext }
{
}

Engine::ParticleEffect::ParticleEffect(const ParticleEffect& original)
    : GameObject(original)
    , m_matWorldOffset{ original.m_matWorldOffset }
    , m_vRotation{ original.m_vRotation }
    , m_vDiffuseUVScroll{ original.m_vDiffuseUVScroll }
    , m_vFadeInOut{ original.m_vFadeInOut }
    , m_fLifeTime{ original.m_fLifeTime }
    , m_strTextureName{ original.m_strTextureName }
    , m_strNoiseTextureName{ original.m_strNoiseTextureName }
    , m_strMaskTextureName{ original.m_strMaskTextureName }
    , m_strAlphaMaskTextureName{ original.m_strAlphaMaskTextureName }
    , m_strShaderName{ original.m_strShaderName }
    , m_strComputeShaderName{ original.m_strComputeShaderName }
    , m_bEffectLoop{ original.m_bEffectLoop }
    , m_bBackCulling{ original.m_bBackCulling }
    , m_bIsSubEmitter{ original.m_bIsSubEmitter }
    , m_bUseEdgeGlow{ original.m_bUseEdgeGlow }
    , m_iTextureNum{ original.m_iTextureNum }
    , m_iNoiseTextureNum{ original.m_iNoiseTextureNum }
    , m_iMaskTextureNum{ original.m_iMaskTextureNum }
    , m_iAlphaMaskTextureNum{ original.m_iAlphaMaskTextureNum }
    , m_iPassIndex{ original.m_iPassIndex }
    , m_iMaskSampler{ original.m_iMaskSampler }
    , m_fDistortionStrength{ original.m_fDistortionStrength }
    , m_fMaskIntensity{ original.m_fMaskIntensity }
    , m_fGlowStrength{ original.m_fGlowStrength }
    , m_vColor{ original.m_vColor }
    , m_vHotColor{ original.m_vHotColor }
    , m_fHotColor{ original.m_fHotColor }
    , m_bEmissive{ original.m_bEmissive }
{
    if (original.m_pSubEmitter)
        m_pSubEmitter = original.m_pSubEmitter->Clone();
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ParticleEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::ParticleEffect::Initialize(void* arg)
{
    if (arg)
    {
        EFFECT_DESC* pDesc = (EFFECT_DESC*)arg;
        m_pOwner = pDesc->pOwner;
    }

    GameObject::Initialize(arg);

    return S_OK;
}
/******************************************************* 초기화 함수 *******************************************************/





//////////////////////////////////////////////////////// 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ParticleEffect::Ready_Components()
{
    return S_OK;
}

SubEmitter* Engine::ParticleEffect::Create_SubEmitter()
{
    if (m_pSubEmitter != nullptr)
        return m_pSubEmitter;

    m_pSubEmitter = SubEmitter::Create();

    return m_pSubEmitter;
}
/******************************************************* 준비 함수 *******************************************************/

void Engine::ParticleEffect::Set_FollowParent(_bool bFollowParent)
{
    if (m_pVIBufferCom != nullptr)
        m_pVIBufferCom->Set_FollowParent(bFollowParent);
}

void Engine::ParticleEffect::Set_WorldMatrix(const _float4x4& vWorldOffset)
{
    m_matWorldOffset = vWorldOffset;
    
    if (m_pVIBufferCom != nullptr)
        m_pVIBufferCom->Set_WorldMatrix(vWorldOffset);
}

void Engine::ParticleEffect::Apply_WorldOffset()
{
    _matrix matWorld = XMLoadFloat4x4(&m_matWorldOffset);

    // 위치는 CS에서 적용하므로 회전만 적용
    matWorld.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

    // Scale 제거
    matWorld.r[0] = XMVector3Normalize(matWorld.r[0]);
    matWorld.r[1] = XMVector3Normalize(matWorld.r[1]);
    matWorld.r[2] = XMVector3Normalize(matWorld.r[2]);
    
    // 이펙트 자체의 회전 포함
    _matrix matEffectRotation = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(m_vRotation.x),
        XMConvertToRadians(m_vRotation.y),
        XMConvertToRadians(m_vRotation.z));

    m_pTransformCom->Set_Matrix(matEffectRotation * matWorld);
}

void Engine::ParticleEffect::Reset_Particles()
{
    m_fAccumulatedTime = 0.f;

    if (Get_IsMeshEffect())
        Reset_MeshSetting();

    if (m_pVIBufferCom != nullptr)
        m_pVIBufferCom->Reset_Particles();
}

void Engine::ParticleEffect::Reset_Position(const _float3& vPosition)
{
    if (m_pVIBufferCom != nullptr)
    {
        m_pVIBufferCom->Reset_Particles();
    }
}

//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ParticleEffect::Update_Priority(const _float fTimeDelta)
{
    if (m_pSubEmitter)
        m_pSubEmitter->Update_Priority(fTimeDelta);

    return 0;
}

_int Engine::ParticleEffect::Update(const _float fTimeDelta)
{
    Apply_WorldOffset();

    // 생명 주기 설정
    m_fAccumulatedTime += fTimeDelta;

    if (m_bEffectLoop && m_fLifeTime > 0.f && m_fAccumulatedTime >= m_fLifeTime)
    {
        m_fAccumulatedTime = 0.f;
        Reset_MeshSetting();
    }

    // SubEmitter 사망 파티클 처리
    if (m_pSubEmitter && m_bIsActive)
    {
        vector<_float4x4> vecDeadParticles = m_pVIBufferCom->ReadGPU_DeadParticles();

        if (vecDeadParticles.empty() == false)
        {
            m_pSubEmitter->Process_DeadParticles(vecDeadParticles);
        }
    }

    return 0;
}

_int Engine::ParticleEffect::Update_Late(const _float fTimeDelta)
{
    if (m_pSubEmitter)
        m_pSubEmitter->Update_Late(fTimeDelta);
    
    return 0;
}

HRESULT Engine::ParticleEffect::Render(const _float fTimeDelta)
{
    // 후면 컬링 옵션 여부
    if (m_bBackCulling == false)
        m_pGameInstance->Set_RasterizerState(RSTATE::SOLID_NONE);

    if (m_pSubEmitter)
        m_pSubEmitter->Render(fTimeDelta);
    
    return S_OK;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 해제 함수 ////////////////////////////////////////////////////////
void Engine::ParticleEffect::Free()
{
    __super::Free();

    Safe_Release(m_pSubEmitter);
}
/******************************************************* 해제 함수 *******************************************************/
