#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_Dash final : public Companion_State
{
private:
	explicit Yakumo_Dash();
	explicit Yakumo_Dash(const Yakumo_Dash& Prototype) = delete;
	virtual ~Yakumo_Dash() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	_float			m_fDashTime = {};

public:
	static Yakumo_Dash* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END