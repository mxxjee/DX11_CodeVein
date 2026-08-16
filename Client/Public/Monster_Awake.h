#pragma once

#include "Client_Define.h"
#include "MonsterState.h"

NS_BEGIN(Client)
class Monster_Awake : public MonsterState
{
private:
	explicit Monster_Awake();
	explicit Monster_Awake(const Monster_Awake& Prototype) = delete;
	virtual ~Monster_Awake() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static Monster_Awake* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END

