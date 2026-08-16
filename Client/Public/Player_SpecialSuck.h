#pragma once
#include "Client_Define.h"
#include "State.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;
class UIObject;
NS_END

NS_BEGIN(Client)
class Player_SpecialSuck final : public State
{
private:
	explicit Player_SpecialSuck();
	explicit Player_SpecialSuck(const Player_SpecialSuck& Prototype) = delete;
	virtual ~Player_SpecialSuck() = default;

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

	/* DrapeWolf 전용 */ 
	vector<CINEMATIC_KEYFRAME> m_vecDrapeWolfKeyFrames; // 키프레임 데이터 모음

	// 몬스터 언제 플레이어 앞으로 텔레포트 시킬지
	_bool  m_bTeleported = { false };
	_float m_fTeleportTimer = {};
	_float m_fTeleportLimit = {};


	Alarm	m_BackStabScreenAlarm;
	UIObject* m_pBackStabScreen = nullptr;

public:
	static Player_SpecialSuck* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END