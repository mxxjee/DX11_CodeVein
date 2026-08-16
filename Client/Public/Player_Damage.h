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
class Player_Damage final : public State
{
private:
	explicit Player_Damage();
	explicit Player_Damage(const Player_Damage& Prototype) = delete;
	virtual ~Player_Damage() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

public:
	_int Get_DamageAnimIndex(Player::DAMAGE_LEVEL eDamageLevel,INPUT_DIR eInputDir);
	_int Dir4Index(INPUT_DIR eInputDir); //4방향인덱스 맞춰주기

private:
	StateMachine* m_pUpperStateMachine = { nullptr };
	StateMachine* m_pStateMachine = { nullptr };
	Transform* m_pPlayerTransformCom = { nullptr };
	Player_MasterRig* m_pPlayerMasterRig = { nullptr };
	Player_Stat* m_pPlayerStatCom = { nullptr };
	class Player* m_pPlayer = { nullptr };

	_int m_iDamage1AnimIndex[Player::DAMAGE_LEVEL::DAMAGELEVEL_END][(_int)INPUT_DIR::INPUT_END] = {}; //데미지1 애니종류
	_int m_iDamage2AnimIndex[Player::DAMAGE_LEVEL::DAMAGELEVEL_END][4] = {}; //데미지2 애니종류
	_int m_iDamage3AnimIndex[Player::DAMAGE_LEVEL::DAMAGELEVEL_END][4] = {}; //데미지3 애니종류
	_int m_iDamage4AnimIndex[Player::DAMAGE_LEVEL::DAMAGELEVEL_END][4] = {}; //데미지4 애니종류

public:
	static Player_Damage* Create(class GameObject* pOwner, class Player_MasterRig* pPlayer_MasterRig);
	virtual void Free() override;
};

NS_END