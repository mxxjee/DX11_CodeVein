#pragma once
#include "MonsterState.h"

NS_BEGIN(Client)
class Monster_Hit :
    public MonsterState
{
private:
	explicit Monster_Hit();
	explicit Monster_Hit(const Monster_Hit& Prototype) = delete;
	virtual ~Monster_Hit() = default;

public:
	virtual HRESULT Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	_float m_fTimeAcc = 0.f;

public:
	static Monster_Hit* Create(class GameObject* pOwner, _uint FL,_uint FR,_uint BL,_uint BR, _float fSpeed);
	virtual void Free() override;


private:
	_uint	FLIdx;
	_uint	FRIdx;
	_uint	BLIdx;
	_uint	BRIdx;


};

NS_END