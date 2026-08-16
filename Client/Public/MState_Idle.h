#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class MState_Idle final : public MState
{
private:
	explicit MState_Idle();
	explicit MState_Idle(const MState_Idle& Prototype) = delete;
	virtual ~MState_Idle() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	Alarm m_Alarm_To_Update; // 다음 행동까지의 대기시간
	_bool m_bUpdate = false;

public:
	static MState_Idle* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END