#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
NS_END

NS_BEGIN(Client)
class Player_Ladder final : public State
{
public:
	enum LADDER_HAND
	{
		HAND_LFET,
		HAND_RIGHT
	};

	enum LADDER_DIRECTION
	{
		LADDER_UP,
		LADDER_DOWN
	};

private:
	explicit Player_Ladder();
	explicit Player_Ladder(const Player_Ladder& Prototype) = delete;
	virtual ~Player_Ladder() = default;

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
private:
	LADDER_HAND			m_eLadderHand;  //현재 손 위치 (더 높은곳에 위치한 손 기준) 
	LADDER_DIRECTION	m_eLadderDirection; //현재 사다리 방향
	_bool				m_bWaitAction = { false }; //움직이는 행동 판단 (Wait 상태는 루프애니메이션이라) 제어
	_bool				m_bLadderEnd = { false };
public:
	static Player_Ladder* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END