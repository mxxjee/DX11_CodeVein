#pragma once
#include "Client_Define.h"
#include "Projectile.h"

NS_BEGIN(Client)
class Homing_Heal final : public Projectile
{
public:
    typedef struct tagHealDesc : public Projectile::PROJECTILE_DESC
    {
        _float      fHealAmount = { 400.f }; // 플레이어 회복량
        _uint       iOwnerID = {}; //해당 투사체 발사한 객체 
        _float		fHomingTurnSpeed = { 0.f };	// 초당 회전 각도 (degree/sec)
        _float		fHomingDelay = { 0.f };		// 발사 후 유도 시작까지 딜레이 (초)
        _float		fHomingDuration = { 999.f };	// 유도 지속 시간 (초, 이후 직진)
        _float		fAcceleration = { 0.f };		// 가속도 (units/sec^2, 0이면 등속)
        _float		fMaxSpeed = { 0.f };		// 최대 속도 제한 (0이면 무제한)
        _float		fMinSpeed = { 0.f };		// 최소 속도 제한
        _float		fHomingLosAngle = { 0.f };	// 유도 시야각 (degree, 이 각도 밖이면 직진)
        _float		fArrivalRadius = { 0.0f };	// 타겟 도달 판정 반경
        _bool		bPredictTarget = { false };	// 타겟 예측 위치 사용 여부
        _float3		vTargetOffset = {};			// 타겟 위치 오프셋 (높이 보정 등)
    } HEAL_DESC;

private:
    explicit Homing_Heal();
    explicit Homing_Heal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Homing_Heal(const Homing_Heal& original);
    virtual ~Homing_Heal();

public:
    virtual HRESULT Initialize_Prototype(LEVEL _level);
    virtual HRESULT Initialize(void* pArg) override;
    virtual _int    Update_Priority(const _float fTimeDelta) override;
    virtual _int    Update_Parallel(const _float fTimeDelta) override;
    virtual _int    Update(const _float fTimeDelta) override;
    virtual _int    Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

public:
    void            OnSpawn(void* _arg) override;
    void            OnDespawn() override;

private:
    void	        Update_Homing(const _float fTimeDelta);
    void	        Update_Speed_Homing(const _float fTimeDelta);
    _float3	        Calculate_TargetPosition();
    void            Move_With_Sweep_Player(const _float fTimeDelta);

private:
    // 유도 파라미터
    _float		m_fHomingTurnSpeed = {};
    _float		m_fHomingDelay = {};
    _float		m_fHomingDuration = { 999.f };
    _float		m_fAcceleration = {};
    _float		m_fMaxSpeed = {};
    _float		m_fMinSpeed = {};
    _float		m_fHomingLosAngle = {};
    _float		m_fHomingLosCos = { -1.f };	// cosf(LosAngle * 0.5f) 캐싱
    _float		m_fArrivalRadius = {};
    _float		m_fArrivalRadiusSq = {};
    _bool		m_bPredictTarget = { false };
    _float3		m_vTargetOffset = {};

    // 런타임 상태
    _float		m_fElapsedTime = { 0.f };		// 스폰 후 경과 시간
    _bool		m_bHomingActive = { false };	// 현재 유도 중인지
    _bool		m_bArrived = { false };	// 도달 판정

    _uint       m_iOwnerID = {};
    _float      m_fHealAmount = { 400.f };

public:
    static Homing_Heal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override final;
};
NS_END