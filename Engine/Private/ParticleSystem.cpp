#include "Engine_Define.h"
#include "ParticleSystem.h"
#include "GameInstance.h"
#include "ParticleEffect.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::ParticleSystem::ParticleSystem()
{
}

Engine::ParticleSystem::ParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : GameObject{ pDevice, pContext }
{
}

Engine::ParticleSystem::ParticleSystem(const ParticleSystem& original)
    : GameObject(original)
    , m_bIsLoop{original.m_bIsLoop}
    , m_bIsAutoPlay{original.m_bIsAutoPlay}
    , m_fTotalDuration{original.m_fTotalDuration}
    , m_fDelayedTime{original.m_fDelayedTime}
    , m_matWorld{original.m_matWorld}
    , m_vRotationSpeed{original.m_vRotationSpeed}
{
    for (const auto& EffectDesc : original.m_vecEffects)
    {
        EFFECT_ENTRY newEntryDesc{};
        newEntryDesc.pEffect = CAST(ParticleEffect*)(EffectDesc.pEffect->Clone(nullptr));
        newEntryDesc.strEffectName = EffectDesc.strEffectName;
        newEntryDesc.fStartDelay = EffectDesc.fStartDelay;
        newEntryDesc.fCurrentTime = EffectDesc.fCurrentTime;
        newEntryDesc.bHasStarted = EffectDesc.bHasStarted;
        newEntryDesc.eEffectType = EffectDesc.eEffectType;
        m_vecEffects.push_back(newEntryDesc);
    }
}
/******************************************************* 생성자, 소멸자 *******************************************************/

_bool Engine::ParticleSystem::Is_Finished() const
{
    if (m_bIsLoop)
        return false;

    if (m_fTotalDuration > 0.f)
        return m_fCurrentTime >= m_fTotalDuration;

    for (const auto& EntryDesc : m_vecEffects)
    {
        if (EntryDesc.bHasStarted == false)
            return false;
    }

    return true;
}

_wstring Engine::ParticleSystem::Get_EffectNameByIndex(_uint iEffectIndex)
{
    if (iEffectIndex > m_vecEffects.size())
        return L"";

    return m_vecEffects[iEffectIndex].strEffectName;
}


void Engine::ParticleSystem::Set_FollowParent(_bool bFollowParent)
{
    // 각 이펙트마다 타겟 행렬을 설정
    for (auto& EntryDesc : m_vecEffects)
        EntryDesc.pEffect->Set_FollowParent(bFollowParent);
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::ParticleSystem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::ParticleSystem::Initialize(void* pArg)
{
    // 외부에서 받아올 월드 행렬을 단위 행렬로 초기화
    XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

    CHECK_FAILED(__super::Initialize(pArg), E_FAIL);

    if (pArg != nullptr)
    {
        const PARTICLESYSTEM_DESC* pParticleSystemDesc = CAST(const PARTICLESYSTEM_DESC*)(pArg);

        m_wstrName = pParticleSystemDesc->wstrName;
        m_bIsLoop = pParticleSystemDesc->bIsLoop;
        m_bIsAutoPlay = pParticleSystemDesc->bIsAutoPlay;
        m_fTotalDuration = pParticleSystemDesc->fTotalDuration;
        m_fDelayedTime = pParticleSystemDesc->fDelayedTime;
        m_vRotationSpeed = pParticleSystemDesc->vRotationSpeed;
    }

    if (m_bIsAutoPlay)
        Play();

    m_bIsActive = false;

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::ParticleSystem::Update_Priority(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    if (m_bIsPlaying && !m_bIsPaused)
    {
        m_fCurrentTime += fTimeDelta;

        if (m_fTotalDuration > 0.f && m_fCurrentTime >= m_fTotalDuration)
        {
            if (m_bIsLoop)
                OnSpawn(nullptr);
            else
                OnDespawn();
        }

        for (auto& EntryDesc : m_vecEffects)
        {
            if (!EntryDesc.bHasStarted &&
                m_fCurrentTime >= EntryDesc.fStartDelay)
            {
                EntryDesc.bHasStarted = true;

                // StartDelay가 있는 이펙트는 활성화 시점의 월드 행렬로 위치 재설정
                if (EntryDesc.fStartDelay > 0.f)
                {
                    EntryDesc.pEffect->Set_WorldMatrix(m_matWorld);
                    EntryDesc.pEffect->Reset_Particles();
                }

                if (EntryDesc.pEffect->Is_SubEmitterOnly() == false)
                    EntryDesc.pEffect->Set_Active(true);
            }

            if (EntryDesc.pEffect->Is_Active())
                EntryDesc.pEffect->Update_Priority(fTimeDelta);
        }
    }

    return 0;
}

_int Engine::ParticleSystem::Update(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    // Follow 콜백이 있다면 매 프레임 행렬 갱신
    if (m_fnFollowCallback)
    {
        // 특정 객체의 이벤트에서 콜백 함수에서 계산한 결과를 가져와서 멤버로 저장
        _float4x4 matWorld = m_fnFollowCallback();
        m_matWorld = matWorld;

        // 각 이펙트마다 타겟 행렬을 설정
        for (auto& EntryDesc : m_vecEffects)
        {
            if (EntryDesc.pEffect->Is_Active())
                EntryDesc.pEffect->Set_WorldMatrix(m_matWorld);
        }
    }

    // ParticleSystem 자체 회전
    if (m_fCurrentTime >= m_fDelayedTime &&
        (m_vRotationSpeed.x != 0.f || m_vRotationSpeed.y != 0.f || m_vRotationSpeed.z != 0.f))
    {
        m_vSystemRotationAcc.x += XMConvertToRadians(m_vRotationSpeed.x) * fTimeDelta;
        m_vSystemRotationAcc.y += XMConvertToRadians(m_vRotationSpeed.y) * fTimeDelta;
        m_vSystemRotationAcc.z += XMConvertToRadians(m_vRotationSpeed.z) * fTimeDelta;

        _matrix matRotation = XMMatrixRotationRollPitchYaw(m_vSystemRotationAcc.x, m_vSystemRotationAcc.y, m_vSystemRotationAcc.z);
        _matrix matParent = XMLoadFloat4x4(&m_matWorld);

        _float4x4 matCombined = {};
        XMStoreFloat4x4(&matCombined, matRotation * matParent);

        // 각 이펙트마다 회전 행렬을 설정
        for (auto& EntryDesc : m_vecEffects)
        {
            if (EntryDesc.pEffect->Is_Active())
                EntryDesc.pEffect->Set_WorldMatrix(matCombined);
        }
    }

    for (auto& EntryDesc : m_vecEffects)
    {
        if (EntryDesc.pEffect->Is_Active())
            EntryDesc.pEffect->Update(fTimeDelta);
    }

    return 0;
}

_int Engine::ParticleSystem::Update_Late(const _float fTimeDelta)
{
    if (m_bIsActive == false)
        return -1;

    _bool bActiveFlag = false;

    for (auto& EntryDesc : m_vecEffects)
    {
        if (EntryDesc.pEffect->Is_Active())
        {
            EntryDesc.pEffect->Update_Late(fTimeDelta);
            bActiveFlag = true;
        }
    }

    if (bActiveFlag)
        Add_RenderGroup(RENDER_GROUP::EFFECT_BLEND);

    return 0;
}

HRESULT Engine::ParticleSystem::Render(const _float fTimeDelta)
{
    for (auto& EntryDesc : m_vecEffects)
    {
        if (EntryDesc.pEffect->Is_Active())
            EntryDesc.pEffect->Render(fTimeDelta);
    }

    return S_OK;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 이펙트 관리 ////////////////////////////////////////////////////////
HRESULT Engine::ParticleSystem::Add_Effect(const _wstring& effectName, ParticleEffect* pEffect, EFFECTTYPE eType, _float fDelay)
{
    // effectName이라는 이펙트가 있을 경우 추가하지 못한다.
    if (pEffect == nullptr || Find_Effect(effectName) != nullptr)
        return E_FAIL;

    EFFECT_ENTRY newEntryDesc{};
    newEntryDesc.pEffect = pEffect;
    newEntryDesc.strEffectName = effectName;
    newEntryDesc.fStartDelay = fDelay;
    newEntryDesc.fCurrentTime = 0.f;
    newEntryDesc.bHasStarted = false;
    newEntryDesc.eEffectType = eType;

    m_vecEffects.push_back(newEntryDesc);
    Safe_AddRef(pEffect);

    return S_OK;
}

HRESULT Engine::ParticleSystem::Remove_Effect(const _wstring& effectName)
{
    for (auto iter = m_vecEffects.begin(); iter != m_vecEffects.end(); iter++)
    {
        if (iter->strEffectName == effectName)
        {
            Safe_Release(iter->pEffect);
            m_vecEffects.erase(iter);
            return S_OK;
        }
    }

    return E_FAIL;
}

ParticleEffect* Engine::ParticleSystem::Find_Effect(const _wstring& effectName)
{
    for (const auto& EffectDesc : m_vecEffects)
    {
        if (EffectDesc.strEffectName == effectName)
            return EffectDesc.pEffect;
    }

    return nullptr;
}

ParticleEffect* Engine::ParticleSystem::Find_EffectByIndex(_uint iEffectIndex)
{
    if (iEffectIndex > m_vecEffects.size())
        return nullptr;

    return m_vecEffects[iEffectIndex].pEffect;
}

void Engine::ParticleSystem::OnSpawn(void* _arg)
{
    Play();
}

void Engine::ParticleSystem::OnDespawn()
{
    Stop();
}

void Engine::ParticleSystem::Play()
{
    for (auto& entry : m_vecEffects)
        entry.pEffect->Set_WorldMatrix(m_matWorld);

    Reset();

    m_bIsPlaying = true;
    m_bIsPaused = false;
    m_bIsActive = true;
}

void Engine::ParticleSystem::Stop()
{
    m_bIsPlaying = false;
    m_bIsPaused = false;
    m_fnFollowCallback = nullptr;
    
    for (auto& Entry : m_vecEffects)
        Entry.pEffect->Set_Active(false);

    m_bIsActive = false;
}

void Engine::ParticleSystem::Pause()
{
    if (m_bIsPlaying)
        m_bIsPaused = true;
}

void Engine::ParticleSystem::Resume()
{
    if (m_bIsPlaying)
        m_bIsPaused = false;
}

void Engine::ParticleSystem::Reset()
{
    m_fCurrentTime = 0.f;
    m_bIsPlaying = false;
    m_bIsPaused = false;
    m_fnFollowCallback = nullptr;
    m_vSystemRotationAcc = { 0.f, 0.f, 0.f };

    for (auto& EntryDesc : m_vecEffects)
    {
        EntryDesc.fCurrentTime = 0.f;
        EntryDesc.bHasStarted = false;
        EntryDesc.pEffect->Set_Active(false);
        EntryDesc.pEffect->Reset_Particles();
    }
}
/******************************************************* 이펙트 관리 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
ParticleSystem* Engine::ParticleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    ParticleSystem* pInstance = new ParticleSystem(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(), L"ParticleSystem 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

ParticleSystem* Engine::ParticleSystem::Clone(void* pArg)
{
    ParticleSystem* pInstance = new ParticleSystem(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"ParticleSystem 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::ParticleSystem::Free()
{
    __super::Free();

    for (auto& EffectEntry : m_vecEffects)
        Safe_Release(EffectEntry.pEffect);
    m_vecEffects.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/

