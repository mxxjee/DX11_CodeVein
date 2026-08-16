#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ParticleEffect;
class SubEmitter;

class ENGINE_DLL ParticleSystem final : public GameObject
{
public:
    typedef struct tagEffectEntry
    {
        ParticleEffect* pEffect = { nullptr };
        _wstring strEffectName = {};
        _float fStartDelay = {};
        _float fCurrentTime = {};
        _bool bHasStarted = {};
        EFFECTTYPE eEffectType = {};
    }EFFECT_ENTRY;

    typedef struct tagParticleSystemDesc : public GAMEOBJECT_DESC
    {
        _bool bIsAutoPlay = {};
        _bool bIsLoop = {};
        _float fTotalDuration = {};
        _float3 vRotationSpeed = {};
        _float fDelayedTime = {};       // 딜레이용 타이머
    }PARTICLESYSTEM_DESC;

private:
    explicit ParticleSystem();
    explicit ParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit ParticleSystem(const ParticleSystem& original);
    virtual ~ParticleSystem() = default;

public:
    // Getter / Setter
    _bool                   Is_Playing() const { return m_bIsPlaying; }
    _bool                   Is_Paused() const { return m_bIsPaused; }
    _bool                   Is_Finished() const;
    _bool                   Is_AutoPlay() const { return m_bIsAutoPlay; }
    _bool                   Is_Loop() const { return m_bIsLoop; }

    vector<EFFECT_ENTRY>    Get_Effects() { return m_vecEffects; }
    _float                  Get_CurrentTime() const { return m_fCurrentTime; }
    _float                  Get_TotalDuration() const { return m_fTotalDuration; }
    _float                  Get_StartDelay(_uint iEffectIndex) { return m_vecEffects[iEffectIndex].fStartDelay; }
    _float                  Get_DelayedTime() { return m_fDelayedTime; }
    _float3                 Get_RotationSpeed() { return m_vRotationSpeed; }
    _uint                   Get_EffectCount() const { return _UINT((m_vecEffects.size())); }
    _wstring                Get_EffectNameByIndex(_uint iEffectIndex);

    void                    Set_StartDelay(_uint iEffectIndex, _float fStartDelay) { m_vecEffects[iEffectIndex].fStartDelay = fStartDelay; }
    void                    Set_AutoPlay(_bool bAutoPlay) { m_bIsAutoPlay = bAutoPlay; }
    void                    Set_Loop(_bool bLoop) { m_bIsLoop = bLoop; }
    void                    Set_TotalDuration(_float fDuration) { m_fTotalDuration = fDuration; }
    void                    Set_WorldMatrix(_float4x4 matWorld) { m_matWorld = matWorld; }
    void                    Set_WorldPosition(_float3 vPosition) { m_matWorld._41 = vPosition.x; m_matWorld._42 = vPosition.y; m_matWorld._43 = vPosition.z; }
    void                    Set_SystemRotation(_float3 vRotationSpeed) { m_vRotationSpeed = vRotationSpeed; }
    void                    Set_DelayedTime(_float fDelayedTime) { m_fDelayedTime = fDelayedTime; }

    // 추적 콜백 관련 함수
    void                    Set_FollowCallback(function<_float4x4()> fn) { m_fnFollowCallback = fn; }
    void                    Clear_FollowCallback() { m_fnFollowCallback = nullptr; }
    void                    Set_FollowParent(_bool bFollowParent);

public:
    virtual HRESULT         Initialize_Prototype() override;
    virtual HRESULT         Initialize(void* pArg) override;
    virtual _int	        Update_Priority(const _float fTimeDelta) override;
    virtual _int	        Update(const _float fTimeDelta) override;
    virtual _int	        Update_Late (const _float fTimeDelta) override;
    virtual HRESULT         Render(const _float fTimeDelta) override;

public:
    // ParticleSystem이 가지고 있는 이펙트 관리
    HRESULT                 Add_Effect(const _wstring& effectName, ParticleEffect* pEffect, EFFECTTYPE eType, _float fDelay = 0.f);
    HRESULT                 Remove_Effect(const _wstring& effectName);
    ParticleEffect*         Find_Effect(const _wstring& effectName);
    ParticleEffect*         Find_EffectByIndex(_uint iEffectIndex);

public:
    // 이펙트 풀링 관련
    virtual void OnSpawn(void* _arg = nullptr) override;
    virtual void OnDespawn() override;

    // ParticleSystem 자체의 재생 제어
    void Play();
    void Stop();
    void Pause();
    void Resume();
    void Reset();

private:
    // 이펙트들을 보관하는 컨테이너
    vector<EFFECT_ENTRY> m_vecEffects;

    // ParticleSystem의 상태 변수들
    _bool       m_bIsPlaying = {};
    _bool       m_bIsPaused = {};
    _bool       m_bIsLoop = {};
    _bool       m_bIsAutoPlay = {};
    _float      m_fCurrentTime = {};
    _float      m_fTotalDuration = {};
    _float3     m_vRotationSpeed = {};
    _float3     m_vSystemRotationAcc = {};
    _float      m_fDelayedTime = {};

    // 외부에서 받아올 월드 행렬
    _float4x4 m_matWorld = { 1.f, 0.f, 0.f, 0.f,
                             0.f, 1.f, 0.f, 0.f,
                             0.f, 0.f, 1.f, 0.f,
                             0.f, 0.f, 0.f, 1.f};

    // 특정 행렬 따라가는 콜백 함수
    function<_float4x4()> m_fnFollowCallback = { nullptr };

public:
    static ParticleSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ParticleSystem* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END
