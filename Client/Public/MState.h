#pragma once
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)

class MState abstract : public State
{
protected:
	explicit MState();
	explicit MState(const MState& original) = delete;
	virtual ~MState() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	_uint Set_AnimState(_uint iAnimIndex, _float fAnimSpeed)
	{
		m_iAnimIdx = iAnimIndex;
		m_fAnimSpeed = fAnimSpeed;
	}

	/* 몬스터들이 공통적으로 할 행동들 */
	_bool Check_To_FallBack();	// 플레이어가 추격 범위 밖으로 도망갔니?(원래 위치로 복귀 트리거)
	_bool Detect_Player();		// 플레이어가 탐지 범위 안에 들어왔니?
	_bool In_Attack_Range_Melee();	// 플레이어가 근접공격 범위 안으로 들어왔니?
	_bool In_Attack_Range_Far();	// 플레이어가 원거리 공격 범위 안으로 들어왔니?(In_Attack_Range_Range는 이상해서 Far로 함)
	_bool In_Walk_Range();

	void Set_AnimSpeed(_float _speed) override { m_fAnimSpeed = _speed; }
	void Set_AnimIndex(_uint _index) override { m_iAnimIdx = _index; }
	void Set_AnimStruct(_uint _index) {}

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
