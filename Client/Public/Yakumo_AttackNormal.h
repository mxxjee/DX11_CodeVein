#pragma once

#include "Client_Define.h"
#include "Companion_State.h"

NS_BEGIN(Client)

class Yakumo_AttackNormal final : public Companion_State
{
private:
	explicit Yakumo_AttackNormal();
	explicit Yakumo_AttackNormal(const Yakumo_AttackNormal& Prototype) = delete;
	virtual ~Yakumo_AttackNormal() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	_int	m_iNormalAttackCobmo = { 0 }; //ÄÞº¸ Áõ°¡¿ë
	void	Reset_NormalAttackCombo();

public:
	static Yakumo_AttackNormal* Create(class GameObject* pOwner);
	virtual void Free() override;

};

NS_END