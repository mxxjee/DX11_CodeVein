#pragma once
#include "Client_Define.h"
#include "State.h"
#include "Player.h"
#include "DamageType.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
class Player_Stat;
NS_END

NS_BEGIN(Client)
class Player_DamageBlow final : public State
{
private:
	explicit Player_DamageBlow();
	explicit Player_DamageBlow(const Player_DamageBlow& Prototype) = delete;
	virtual ~Player_DamageBlow() = default;

public:
	DOWN_POSTURE Get_DownPosture() const { return m_eDownPosture; }
public:
	virtual HRESULT Initialize(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	StateMachine* m_pStateMachine = { nullptr };
	StateMachine* m_pUpperStateMachine = { nullptr };
	Transform* m_pPlayerTransformCom = { nullptr };
	Player_MasterRig* m_pPlayerMasterRig = { nullptr };
	Player_Stat* m_pPlayerStatCom = { nullptr };
	class Player* m_pPlayer = { nullptr };

private:
	DAMAGEBLOWTYPE	m_eDamageBlowType;
	DOWN_POSTURE	m_eDownPosture;
	_float			m_fLoopEndTime = {0.f};
	_bool			m_bFirstDamage = { false };
public:
	static Player_DamageBlow* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END