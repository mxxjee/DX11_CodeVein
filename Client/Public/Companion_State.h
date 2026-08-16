#pragma once
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
class Player_Stat;
NS_END

NS_BEGIN(Client)
class Companion_State abstract : public State
{
protected:
	explicit Companion_State();
	explicit Companion_State(const Companion_State& original) = delete;
	virtual ~Companion_State() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

protected:
	void				Check_SameStateCommandAI(); //Idle,Walk,Run,Dash 전용으로 사용중

protected:
	StateMachine*		m_pStateMachine = { nullptr };
	Transform*			m_pYakumoTransformCom = { nullptr };
	class Player_Stat*	m_pYakumoStatCom = { nullptr };
	class Yakumo*		m_pYakumo = { nullptr };

public:
	virtual void Free() override;
};

NS_END