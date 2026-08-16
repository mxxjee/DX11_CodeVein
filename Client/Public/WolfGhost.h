#pragma once

#include "Client_Define.h"
#include "BossBase.h"

NS_BEGIN(Client)

class WolfGhost final : public BossBase
{
private:
	explicit WolfGhost();
	explicit WolfGhost(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit WolfGhost(const WolfGhost& original);
	virtual ~WolfGhost();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;
	HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum);

	HRESULT Bind_ShaderResources();


private:
	HRESULT Ready_Controller();
	HRESULT Ready_Components();
	HRESULT Ready_PhysXEvent();
	HRESULT Ready_Event();
	HRESULT Ready_States();
	HRESULT Ready_Value();
	HRESULT Ready_Sockets();

	void Spawn_Attack();
	void Laser_After_FireBall();
	
	// 텔레포트 이펙트 투사체 소환용
	void Spawn_Effect();
	virtual void OnDamaged(const DAMAGE_EVENT& _damageEvent) override;
	void Change_Phase() override;

public:
	enum WolfGhostAnim {
		WG_IDLE, WG_AWAKE, WG_VOIDLASER, WG_TELEPORT, WG_BLOODBOOM = 4,
		WG_SOLARBEAM, WG_ICEBLAST, WG_SOLARLASER, WG_HELLFIREBOLT, WG_FIREBLAST = 9,
		WG_VOIDSPHERE, WG_THUNDER, WG_ICEHOMING, WG_PHASECHANGE, WG_DEAD,
		WG_END };


public:
	static WolfGhost* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

private:
	const _float4x4* m_pRightHandAMatrix = { nullptr };
	const _float4x4* m_pRightHandBMatrix = { nullptr };
	const _float4x4* m_pLeftHandAMatrix = { nullptr };
	const _float4x4* m_pLeftHandBMatrix = { nullptr };
	const _float4x4* m_pLeftForeArmAMatrix = { nullptr };
	const _float4x4* m_pLeftForeArmBMatrix = { nullptr };
	const _float4x4* m_pRightForeArmBMatrix = { nullptr };

	enum SolarLaserPhase { SR_LASER, SR_RIGHTHAND, SR_LEFTHAND };
	SolarLaserPhase m_eSolarLaserPhase = {};	// 패턴 사용 감지용(레이저 쏘고 타이밍 맞춰서 두번 쏘는게 힘들것 같아서(애니메이션 속도 바뀌면 또 다시 해야되고)
	_bool m_bFireBallCached = false;

	// 레이저 자르기 용도
	struct LASERSYSTEM_CACHE
	{
		ParticleSystem* pSystem = { nullptr };
		vector<pair<_uint, _float>> vecMeshScales{};					// <MeshEffect 인덱스, 원본 scale>
	};

	vector<LASERSYSTEM_CACHE>	m_vecLaserSystems{};
	class WolfGhost_Attack*		m_pLaserAttack = { nullptr };
	_float						m_fLaserRatio = { 1.f };
	_bool						m_bTeleported = false;
	_bool						m_bFlame = {};

	Alarm						m_RimAlarm = {};
	class MState_TransitionTable* m_pPhase2Table = { nullptr };
	class MState_TransitionTable* m_pPhase1Table = { nullptr };

public:
	virtual void    OnAwarePlayer();   //몬스터가 플레이어를  감지한 첫 순간만 발동하는함수(보통 보스를 위해 사용)
public:
	void Free() override final;


private:
};

NS_END
