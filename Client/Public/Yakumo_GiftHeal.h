#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_GiftHeal final : public Companion_State
{
private:
	explicit Yakumo_GiftHeal();
	explicit Yakumo_GiftHeal(const Yakumo_GiftHeal& Prototype) = delete;
	virtual ~Yakumo_GiftHeal() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;
	
	virtual void On_ProjectileEvent() override;

public:
	static Yakumo_GiftHeal* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END