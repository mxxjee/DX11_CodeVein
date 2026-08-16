#pragma once
#include "Client_Define.h"
#include "State.h"
#include "DamageType.h"

NS_BEGIN(Engine)
class StateMachine;
class Transform;

NS_END

NS_BEGIN(Client)
class Player_Death final : public State
{
private:
    explicit Player_Death();
    explicit Player_Death(const Player_Death& Prototype) = delete;
    virtual ~Player_Death() = default;

public:
    void Set_DeathType(DEATHTYPE eType) { m_eDeathType = eType; }

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
	DEATHTYPE	m_eDeathType;
	_bool		m_bFirst = { false };
public:
    static Player_Death* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
    virtual void Free() override;

private:
    Alarm   CloseUIAlarm;

};

NS_END