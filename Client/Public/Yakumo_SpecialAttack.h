#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_SpecialAttack final : public Companion_State
{
private:
	explicit Yakumo_SpecialAttack();
	explicit Yakumo_SpecialAttack(const Yakumo_SpecialAttack& Prototype) = delete;
	virtual ~Yakumo_SpecialAttack() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Yakumo_SpecialAttack* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END