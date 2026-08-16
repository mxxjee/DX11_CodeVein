#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class MState_Special0 final :
    public MState
{
private:
	explicit MState_Special0();
	explicit MState_Special0(const MState_Special0& Prototype) = delete;
	virtual ~MState_Special0() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:


public:
	static MState_Special0* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};
NS_END
