#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
class Player_Stat;
NS_END

NS_BEGIN(Client)
class Player_AttackStrongCharge final : public State
{
private:
	explicit Player_AttackStrongCharge();
	explicit Player_AttackStrongCharge(const Player_AttackStrongCharge& Prototype) = delete;
	virtual ~Player_AttackStrongCharge() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	virtual void On_ProjectileEvent() override;

public:
	void Fire_BayonetChargeBullet();

private:
	StateMachine* m_pStateMachine = { nullptr };
	StateMachine* m_pUpperStateMachine = { nullptr };
	Transform* m_pPlayerTransformCom = { nullptr };
	Player_MasterRig* m_pPlayerMasterRig = { nullptr };
	class Player* m_pPlayer = { nullptr };
	Player_Stat* m_pPlayerStat = { nullptr };

public:
	static Player_AttackStrongCharge* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END