#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class MState_Awake final : public MState
{
private:
	explicit MState_Awake();
	explicit MState_Awake(const MState_Awake& Prototype) = delete;
	virtual ~MState_Awake() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static MState_Awake* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END

