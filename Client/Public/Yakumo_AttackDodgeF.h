#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_AttackDodgeF final : public Companion_State
{
private:
	explicit Yakumo_AttackDodgeF();
	explicit Yakumo_AttackDodgeF(const Yakumo_AttackDodgeF& Prototype) = delete;
	virtual ~Yakumo_AttackDodgeF() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Yakumo_AttackDodgeF* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END