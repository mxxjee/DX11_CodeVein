#pragma once
#include "MonsterState.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)

/*몬스터 turn이 이밍 ㅣㅆ지만 보스가쓰고있고 바라보는지점이 player로
//고정되어있어서 바꾸기 애매해서 그냥 따로만듬.*/
class Monster_SD_PatrolTurn :
    public MonsterState
{
private:
	explicit Monster_SD_PatrolTurn();
	explicit Monster_SD_PatrolTurn(const Monster_SD_PatrolTurn& Prototype) = delete;
	virtual ~Monster_SD_PatrolTurn() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;
private:
	StateMachine* m_pStateMachine = { nullptr };
	Transform* m_pMonsterTransformCom = { nullptr };
	class Monster* m_pMonster = { nullptr };

	_float m_fTimeAcc = 0.f;

public:
	static Monster_SD_PatrolTurn* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

private:
	_vector m_vStartLookDir;
	_vector m_vTargetDir;
};
NS_END
