#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_Damage final : public Companion_State
{
private:
	explicit Yakumo_Damage();
	explicit Yakumo_Damage(const Yakumo_Damage& Prototype) = delete;
	virtual ~Yakumo_Damage() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Yakumo_Damage* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END