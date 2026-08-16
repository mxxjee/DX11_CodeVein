#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class MState_Melee1 final : public MState
{
private:
	explicit MState_Melee1();
	explicit MState_Melee1(const MState_Melee1& Prototype) = delete;
	virtual ~MState_Melee1() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:


public:
	static MState_Melee1* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END

