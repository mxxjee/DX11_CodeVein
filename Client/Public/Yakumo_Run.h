#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_Run final : public Companion_State
{
private:
	explicit Yakumo_Run();
	explicit Yakumo_Run(const Yakumo_Run& Prototype) = delete;
	virtual ~Yakumo_Run() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:

public:
	static Yakumo_Run* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END