#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_AttackDodgeB final : public Companion_State
{
private:
	explicit Yakumo_AttackDodgeB();
	explicit Yakumo_AttackDodgeB(const Yakumo_AttackDodgeB& Prototype) = delete;
	virtual ~Yakumo_AttackDodgeB() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Yakumo_AttackDodgeB* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END