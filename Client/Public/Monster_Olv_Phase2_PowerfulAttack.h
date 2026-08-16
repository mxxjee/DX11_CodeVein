#pragma once
#include "Client_Define.h"
#include "MonsterState.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Monster_Olv_Phase2_PowerfulAttack final : public MonsterState
{
private:
	explicit Monster_Olv_Phase2_PowerfulAttack();
	explicit Monster_Olv_Phase2_PowerfulAttack(const Monster_Olv_Phase2_PowerfulAttack& Prototype) = delete;
	virtual ~Monster_Olv_Phase2_PowerfulAttack() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	StateMachine* m_pStateMachine = { nullptr };
	Transform* m_pMonsterTransformCom = { nullptr };
	class Monster* m_pMonster = { nullptr };

public:
	static Monster_Olv_Phase2_PowerfulAttack* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;
};

NS_END