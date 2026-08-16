#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_AttackStrong final : public Companion_State
{
private:
	explicit Yakumo_AttackStrong();
	explicit Yakumo_AttackStrong(const Yakumo_AttackStrong& Prototype) = delete;
	virtual ~Yakumo_AttackStrong() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Yakumo_AttackStrong* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END