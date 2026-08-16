#pragma once

#include "Client_Define.h"
#include "Companion_State.h"
#include "DamageType.h"

NS_BEGIN(Client)

class Yakumo_DamageBlow final : public Companion_State
{
private:
	explicit Yakumo_DamageBlow();
	explicit Yakumo_DamageBlow(const Yakumo_DamageBlow& Prototype) = delete;
	virtual ~Yakumo_DamageBlow() = default;

public:
	DOWN_POSTURE Get_DownPosture() const { return m_eDownPosture; }

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	DAMAGEBLOWTYPE	m_eDamageBlowType;
	DOWN_POSTURE	m_eDownPosture;
	_float			m_fLoopEndTime = {};
	_bool			m_bFirstDamage = { false };
	_bool			m_bDead = { false };

public:
	static Yakumo_DamageBlow* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END