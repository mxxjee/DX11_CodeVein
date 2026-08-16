#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Player_BackStab final : public State
{
private:
	explicit Player_BackStab();
	explicit Player_BackStab(const Player_BackStab& Prototype) = delete;
	virtual ~Player_BackStab() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	StateMachine* m_pStateMachine = { nullptr };
	StateMachine* m_pUpperStateMachine = { nullptr };
	Transform* m_pPlayerTransformCom = { nullptr };
	Player_MasterRig* m_pPlayerMasterRig = { nullptr };
	class Player* m_pPlayer = { nullptr };

public:
	static Player_BackStab* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END