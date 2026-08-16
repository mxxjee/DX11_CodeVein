#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Player_Skill final : public State
{
private:
	explicit Player_Skill();
	explicit Player_Skill(const Player_Skill& Prototype) = delete;
	virtual ~Player_Skill() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);

	virtual void		Enter_State() override;
	virtual void		Update_State(_float fTimeDelta) override;
	virtual void		Exit_State() override;

	virtual void		On_ProjectileEvent() override;
public:
	void				Set_CurrentSkill(class Skill_Base* pSkill) { m_pCurSkill = pSkill; }
	class Skill_Base*	Get_CurrentSkill() { return m_pCurSkill; }
private:
	StateMachine*		m_pStateMachine = { nullptr };
	StateMachine*		m_pUpperStateMachine = { nullptr };
	Transform*			m_pPlayerTransformCom = { nullptr };
	Player_MasterRig*	m_pPlayerMasterRig = { nullptr };
	class Player*		m_pPlayer = { nullptr };

private:
	class Skill_Base*	m_pCurSkill = { nullptr };

public:
	static Player_Skill* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END