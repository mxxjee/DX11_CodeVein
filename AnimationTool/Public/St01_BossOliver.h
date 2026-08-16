#pragma once
#include "BossBase.h"
#include "Monster_Weapon.h"
#include "MWeapon_Hammer.h"

NS_BEGIN(Engine)
class TrailEffect;
NS_END

class St01_BossOliver final : public BossBase
{
private:
    explicit St01_BossOliver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit St01_BossOliver(const St01_BossOliver& original);
    virtual ~St01_BossOliver() = default;

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level) override;
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;
    virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
    virtual HRESULT Ready_Components() override;
    virtual HRESULT Ready_PartObjects();
    //virtual HRESULT Set_Animation() override;
    HRESULT     Ready_Event();
    HRESULT     Ready_PhysXEvent();
    void Update_WeaponPosition();

private:
    _int                 m_iAttackPattern = 0;
    EventHandle m_iAnimEventHandle = {};
    const _float4x4* m_pCachedHandMatrix = nullptr;
    const _float4x4* m_pCachedHipMatrix = nullptr;
    // ParticleEffect 관련 변수


    // Sword Trail 관련 변수
    vector<TrailEffect*>		m_vecTrailEffects;
    _bool						m_bTrailActive = {};

public:
    static St01_BossOliver* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    virtual GameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
