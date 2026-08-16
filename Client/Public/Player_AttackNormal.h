#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Player_AttackNormal final : public State
{
private:
	explicit Player_AttackNormal();
	explicit Player_AttackNormal(const Player_AttackNormal& Prototype) = delete;
	virtual ~Player_AttackNormal() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	StateMachine*		m_pStateMachine = { nullptr };
	StateMachine*		m_pUpperStateMachine = { nullptr };
	Transform*			m_pPlayerTransformCom = { nullptr };
	Player_MasterRig*	m_pPlayerMasterRig = { nullptr };
	class Player*		m_pPlayer = { nullptr };
	_int				m_iNormalAttackCobmo = {0}; //ÄÞº¸ Áõ°¡¿ë

private:
	void	Reset_NormalAttackCombo();
public:
	static Player_AttackNormal* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END