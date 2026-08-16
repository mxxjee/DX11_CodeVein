#pragma once
#include "BossBase.h"

NS_BEGIN(Engine)
class TrailEffect;
NS_END

NS_BEGIN(Client)

class St01_BossOliver final : public BossBase
{
public:
    enum OLIVER_PATTERN
    {
        //페이즈 1
        ATTACK_P1_JUMP = BossBase::BOSS_END,
        ATTACK_P1_SMASH,
        ATTACK_P1_COMBO,
        //페이즈 2
        ATTACK_P2_KETSUGI,
        ATTACK_P2_POWERFUL,
        ATTACK_P2_TRIPLE,
        ATTACK_P2_STRONG_COMBOA,
        ATTACK_P2_STRONG_COMBOB,
        ATTACK_P2_POWERFUL_COMBO,//24
        ATTACK_P2_STRONG,
        ATTACK_P2_SPECIAL_SECOND
    };

private:
    explicit St01_BossOliver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit St01_BossOliver(const St01_BossOliver& original);
    virtual ~St01_BossOliver() = default;

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level) override;
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update_Priority(const _float fTimeDelta) override final;
    virtual _int Update_Parallel(const _float fTimeDelta) override final;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;
    virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
    //void Update_WeaponPosition_Oliv();

public:
    virtual _uint Select_NextPattern() override;
    HRESULT Change_To_Giant_Phase();

private:
    virtual HRESULT Ready_Components() override;
    virtual HRESULT Ready_PartObjects();
    virtual HRESULT Ready_States() override;
    HRESULT Ready_Phase2_States();
    HRESULT Ready_Value();

    HRESULT		Ready_PhysXEvent();
    HRESULT     Ready_Event();

    HRESULT PlayTestKey();
    void OnDamaged(const DAMAGE_EVENT& _damageEvent);
    virtual void OnSpawn(void* _arg) override;
    void Process_AttackOverlap_Monster();
private:
    _int    m_iAttackPattern = 0;
    _float  m_fDistance = {}; // 몬스터와 플레이어의 거리
    _float  m_fTimeDelta = 0.f;

    // Sword Trail 관련 변수
    vector<TrailEffect*>		m_vecTrailEffects;
    _bool						m_bTrailActive = {};

    _uint m_iLastAttackPattern = 0;

public:
    static St01_BossOliver* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    virtual GameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END