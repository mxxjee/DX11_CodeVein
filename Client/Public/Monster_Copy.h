#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class Monster_Copy final: public MState
{
private:
	explicit Monster_Copy();
	explicit Monster_Copy(const Monster_Copy& Prototype) = delete;
	virtual ~Monster_Copy() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Monster_Copy* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END

