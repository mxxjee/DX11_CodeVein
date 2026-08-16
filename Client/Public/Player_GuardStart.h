#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Player_GuardStart final : public State
{
private:
	explicit Player_GuardStart();
	explicit Player_GuardStart(const Player_GuardStart& Prototype) = delete;
	virtual ~Player_GuardStart() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	StateMachine*		m_pUpperStateMachine = { nullptr };
	StateMachine*		m_pStateMachine = { nullptr };
	Transform*			m_pPlayerTransformCom = { nullptr };
	Player_MasterRig*	m_pPlayerMasterRig = { nullptr };
	class Player*		m_pPlayer = { nullptr };

public:
	static Player_GuardStart* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END