#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class MState_PhaseChange final : public MState
{
private:
	explicit MState_PhaseChange();
	explicit MState_PhaseChange(const MState_PhaseChange& Prototype) = delete;
	virtual ~MState_PhaseChange() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	static MState_PhaseChange* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END

