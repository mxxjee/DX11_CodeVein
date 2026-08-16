#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_DamageEnd final : public Companion_State
{
private:
	explicit Yakumo_DamageEnd();
	explicit Yakumo_DamageEnd(const Yakumo_DamageEnd& Prototype) = delete;
	virtual ~Yakumo_DamageEnd() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Yakumo_DamageEnd* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END