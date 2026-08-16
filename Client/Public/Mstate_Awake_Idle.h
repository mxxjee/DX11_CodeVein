#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)
class Mstate_Awake_Idle :
    public MState
{
private:
	explicit Mstate_Awake_Idle();
	explicit Mstate_Awake_Idle(const Mstate_Awake_Idle& Prototype) = delete;
	virtual ~Mstate_Awake_Idle() = default;
public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Mstate_Awake_Idle* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;
};

NS_END