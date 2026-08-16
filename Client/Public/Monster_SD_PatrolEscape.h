#pragma once
#include "MonsterState.h"
NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Monster_SD_PatrolEscape :
    public MonsterState
{
private:
	explicit Monster_SD_PatrolEscape();
	explicit Monster_SD_PatrolEscape(const Monster_SD_PatrolEscape& Prototype) = delete;
	virtual ~Monster_SD_PatrolEscape() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;
private:
	StateMachine* m_pStateMachine = { nullptr };
	Transform* m_pMonsterTransformCom = { nullptr };
	class Monster* m_pMonster = { nullptr };

public:
	static Monster_SD_PatrolEscape* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

private:
	_vector vStartPos;
	_vector m_vEscapeDir;
};
NS_END


