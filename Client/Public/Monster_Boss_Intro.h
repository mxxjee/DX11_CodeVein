#pragma once
#include "MonsterState.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Monster_Boss_Intro :
    public State
{
public:
	struct BossIntroEvent
	{
		bool bFinish = false;


	};
private:
	explicit Monster_Boss_Intro();
	explicit Monster_Boss_Intro(const Monster_Boss_Intro& Prototype) = delete;
	virtual ~Monster_Boss_Intro() = default;

public:	
	virtual HRESULT Initialize(class GameObject* pOwner, _float fSecond, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	StateMachine* m_pStateMachine = { nullptr };
	Transform* m_pMonsterTransformCom = { nullptr };
	class Monster* m_pMonster = { nullptr };

public:
	static Monster_Boss_Intro* Create(class GameObject* pOwner,_float fSecond, _float fSpeed);
	virtual void Free() override;

private:
	Alarm		m_AppearUIAlarm;		//UI이벤트가 호출될 시간( 보통 이상태 진입 이후 3초뒤??, 매개변수로 초 지정가능)
	_bool m_bIntroPlayed = false;
	_float m_fAnimSpeed = 1.f;
};
NS_END
