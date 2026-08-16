#pragma once

#include "Client_Define.h"
#include "Projectile.h"

NS_BEGIN(Client)

class Projectile_WolfHomingIce final : public Projectile
{
private:
	explicit Projectile_WolfHomingIce();
	explicit Projectile_WolfHomingIce(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Projectile_WolfHomingIce(const Projectile_WolfHomingIce& original);
	virtual ~Projectile_WolfHomingIce();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	void OnSpawn(void* _arg) override;
	void OnDespawn() override;

private:
	enum class HOMING_PHASE { READY, ATTACK, WAIT, END };
	void Update_MoveDirection(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	HOMING_PHASE m_ePhase = { HOMING_PHASE::READY };
	_uint		m_iHomingCount = {};	// 몇 번째 방향 전환인지 체크하는 카운트
	_float		m_fPhaseElapsedTime = { 0.f };		// 페이즈 전환 후 경과 시간
	_float		m_fWaitTime = {};
	_float		m_fMoveSpeed = {};

public:
	static Projectile_WolfHomingIce* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
