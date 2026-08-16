#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_BackStep final : public Companion_State
{
private:
	explicit Yakumo_BackStep();
	explicit Yakumo_BackStep(const Yakumo_BackStep& Prototype) = delete;
	virtual ~Yakumo_BackStep() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:

public:
	static Yakumo_BackStep* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END