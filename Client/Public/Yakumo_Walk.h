#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_Walk final : public Companion_State
{
private:
	explicit Yakumo_Walk();
	explicit Yakumo_Walk(const Yakumo_Walk& Prototype) = delete;
	virtual ~Yakumo_Walk() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:

public:
	static Yakumo_Walk* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END