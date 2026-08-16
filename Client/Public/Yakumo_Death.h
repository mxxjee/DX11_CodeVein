#pragma once

#include "Client_Define.h"
#include "Companion_State.h"
#include "DamageType.h"

NS_BEGIN(Client)

class Yakumo_Death final : public Companion_State
{
private:
	explicit Yakumo_Death();
	explicit Yakumo_Death(const Yakumo_Death& Prototype) = delete;
	virtual ~Yakumo_Death() = default;

public:
	void Set_DeathType(DEATHTYPE eType) { m_eDeathType = eType; }

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	DEATHTYPE	m_eDeathType;
	_bool		m_bFirst = { false };

public:
	static Yakumo_Death* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END