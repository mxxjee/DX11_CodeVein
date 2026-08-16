#pragma once
#include "Client_Define.h"
#include "State.h"
#include "Player.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
class Player_Stat;
NS_END

NS_BEGIN(Client)
class Player_CheckPoint final : public State
{
public:
	enum CHECKPOINTTYPE
	{
		START, 
		LOOP,
		END, 
	};

private:
	explicit Player_CheckPoint();
	explicit Player_CheckPoint(const Player_CheckPoint& Prototype) = delete;
	virtual ~Player_CheckPoint() = default;

public:
	void Set_CheckPointType(CHECKPOINTTYPE eType) { m_eCheckPointType = eType; }

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
	class Player* m_pPlayer = { nullptr };
	class Player_Stat* m_pPlayerStatCom = { nullptr };

private:
	CHECKPOINTTYPE	m_eCheckPointType = {};
	_bool			m_bHealStat = { false };
public:
	static Player_CheckPoint* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END