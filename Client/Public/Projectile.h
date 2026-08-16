#pragma once
#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)
class Projectile abstract : public GameObject
{
public:
    typedef struct tagProjectile : public GameObject::GAMEOBJECT_DESC
    {
        //PHYSX_CONTROLLER_DESC tControllerDesc;
        _bool                 bPiercing = { false }; //관통 여부 
        _uint                 iMaxHitCount = { 1 }; //최대히트개수
        _float                fLifeTime = {};
        _float                fAttackDamage = { 200.f };
        _float                fSpeed = {};
        _float                fAttackRadius = { 0.5f };
        _float                fStartUpElapseTime = {};
        DAMAGEPOWER           eProjectileDamagePower;
        POOL_ID               eSpanwEffectID = {};
        POOL_ID               eDeSpanwEffectID = {};
        _float3               vDir = {};
        _float3               vDebugColor = {};

        _string strSpawnSound;   // 발사될 때 날 소리
        _string strDespawnSound; // 터질 때 날 소리

    }PROJECTILE_DESC;

protected:
    explicit Projectile();
    explicit Projectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Projectile(const Projectile& original);
    virtual ~Projectile() = default;

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level);
    virtual HRESULT Initialize(void* _arg) override;
    virtual _int Update_Priority(const _float fTimeDelta) override;
    virtual _int Update_Parallel(const _float fTimeDelta) override;
    virtual _int Update(const _float fTimeDelta) override;
    virtual _int Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

protected: //초기화받는용도 , 공격력도 플레이어한테 받아와도될듯 추후 추가 

    _float                      m_fLifeTime = { 0.f }; // 풀링 생성시에 안넣어주면 디폴트값 사용
    _float                      m_fDefaultLifeTime = { 0.f }; ///수명(초기화 or 풀링생성시 받아올값)
    DAMAGEPOWER                 m_eProjectileDamagePower = { DAMAGEPOWER::POWER_END };

    physx::PxCapsuleController* m_pController = { nullptr };
    _float3                     m_vDir = {};
  
    _bool                       m_bPiercing = { false }; //관통 여부 
    _uint                       m_iMaxHitCount = { 1 }; //최대히트개수 / 관통은 0개로 소멸타임은 lifeTime으로 제어
    _float                      m_fSpeed = {}; // 속도 그냥 Desc으로 제어
protected:
    class ParticleSystem*       m_pFollowEffect = { nullptr };      // 따라가는 이펙트
    _bool                       m_bAttackSweepActive = { false };	// 공격중인지 
    _float3						m_vPrevProjectilePos = {};			// 이 전 프레임 총알 위치
    _float3						m_vCurrentProjectilePos = {};		// 현재 프레임 현재 총알 위치
    _float                      m_fAttackRadius = { 0.5f };			// 공격 범위
    _float                      m_fAttackDamage = { 100.f };		// 공격 데미지
    _float                      m_fAttackHalfHeight = { 0.f };		// 공격 반높이
    set<_uint>					m_setHitTargets = {};				// 이미 공격한 몬스터 저장
    _bool                       m_bEnablePhysics = false;
    _float                      m_fKnockbackForce = { 0.f };
    GameObject*                 m_pPlayer = { nullptr };
    _bool                       m_bIsHit = false;
    const _float4x4*            m_pBoneMatrix = { nullptr };

    _float3 m_vLaserEndPos = {};                                    // 레이저 끝점
    _float m_fLaserHitDistance = {};                                // 레이저 시작부터 끝까지 거리
    POOL_ID m_eSpawnEffectID = {};
    POOL_ID m_eDeSpawnEffectID = {};
    _float3 m_vDebugColor = {};

    _string m_strSpawnSound;   // 발사될 때 날 소리
    _string m_strDespawnSound; // 터질 때 날 소리


public:    
    void            OnSpawn(void* _arg) override;
    void            OnDespawn() override;
    void            Set_FollowEffect(class ParticleSystem* pFollowEffect) { m_pFollowEffect = pFollowEffect; }
    // 컨트롤러 없이 움직이는 투사체용
    void            Move_With_Sweep(const _float fTimeDelta);

    void            Move_With_Sweep_Monster(const _float fTimeDelta);

    _float          Get_LaserHitDistance() const { return m_fLaserHitDistance; }

protected:
    void                    Move(const _float fTimeDelta);
    void			        Process_AttackSweep();
    void                    Process_AttackOverlap();
    void			        Process_AttackSweep_Monster();
    void                    Process_AttackOverlap_Monster();
    void                    Process_AttackFanOverlap_Monster(_float _currentRadius, _float _halfAngle);
    void                    Process_AttackLaserSweep_Monster(_float _laserRadius, _float _maxDistance, _float3& _startPosition, _fvector _fireDirection);
    PxCapsuleController*    Create_Controller(PHYSX_CONTROLLER_DESC& _controllerDesc);
    void                    Update_ProjectilePosition();

    void                    Update_ProjectileBone();

    _bool                   IsInsideFanArea(const _float3& targetPos, _float currentRadius, _float _halfAngle);


public:
    void Free() override;

};
NS_END
