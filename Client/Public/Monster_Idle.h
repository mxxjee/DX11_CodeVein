#pragma once
#include "Client_Define.h"
#include "MonsterState.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Monster_Idle final : public MonsterState
{
private:
	explicit Monster_Idle();
	explicit Monster_Idle(const Monster_Idle& Prototype) = delete;
	virtual ~Monster_Idle() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	HRESULT Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime) override;

private:
	StateMachine*			m_pStateMachine = { nullptr };
	Transform*				m_pMonsterTransformCom = { nullptr };
	class Monster*			m_pMonster = { nullptr };
	_float m_minTime{0.1f}, m_maxTime{0.2f};

public:
	static Monster_Idle* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

private:
	Alarm			m_Alarm_To_Update;//IDLE->다른 대기상태 넘어가기위해서는 몇초대기 이후 넘어가도록 설계
	bool			m_bUpdate = false;
};

NS_END