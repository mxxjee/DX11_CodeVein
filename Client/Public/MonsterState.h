#pragma once
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)

class MonsterState abstract : public State
{
protected:
	explicit MonsterState();
	explicit MonsterState(const MonsterState& original) = delete;
	virtual ~MonsterState() = default;

public:
	HRESULT Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	_uint Set_AnimState(_uint iAnimIndex, _float fAnimSpeed)
	{
		m_iAnimIdx = iAnimIndex;
		m_fAnimSpeed = fAnimSpeed;
	}

protected:
	StateMachine* m_pStateMachine = { nullptr };
	Transform* m_pMonsterTransformCom = { nullptr };
	class Monster* m_pMonster = { nullptr };

protected:
	//재생할애니메이션과 속도.
	_uint		m_iAnimIdx = 0;
	_float		m_fAnimSpeed = 1.f;

public:
	virtual void Free() override;

};

NS_END
