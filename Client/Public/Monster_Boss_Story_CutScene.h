#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Monster_Boss_Story_CutScene final : public State
{
private:
	explicit Monster_Boss_Story_CutScene();
	explicit Monster_Boss_Story_CutScene(const Monster_Boss_Story_CutScene& Prototype) = delete;
	virtual ~Monster_Boss_Story_CutScene() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	StateMachine* m_pStateMachine = { nullptr };
	Transform* m_pMonsterTransformCom = { nullptr };
	class Monster* m_pMonster = { nullptr };
	_uint m_iStep = 0;
public:
	static Monster_Boss_Story_CutScene* Create(class GameObject* pOwner);
	virtual void Free() override;
};

NS_END