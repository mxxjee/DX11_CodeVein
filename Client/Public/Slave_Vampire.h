#pragma once

#include "Client_Define.h"
#include "Monster.h"

NS_BEGIN(Client)

class Slave_Vampire final : public Monster
{
private:
	explicit Slave_Vampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Slave_Vampire(const Slave_Vampire& original);
	virtual ~Slave_Vampire();


public:
	HRESULT Initialize_Prototype(LEVEL _level) override final;
	HRESULT Initialize(void* _arg) override final;
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update_Priority(const _float fTimeDelta) override final; 
	_int Update(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta) override final;
	HRESULT Render(const _float fTimeDelta)	override final;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
	HRESULT Ready_Controller();
	HRESULT Ready_Components();
	HRESULT Ready_PhysXEvent();
	HRESULT Ready_Event();
	HRESULT Ready_States();
	HRESULT Ready_Value();
	HRESULT Ready_Socket();

	HRESULT Bind_ShaderResources();

	_uint Select_NextPattern() override;

	enum ANIMINDEX {
		IDLE, DEATH, MELEE0, DAMAGE_L, DAMAGE_R,
		TURN180L, TURN180R, TURN90_L, TURN90_R, RUN_F,
		WALK_F, WALK_L, WALK_R, REPEL, STUN_N,
		STUN_E, BACKSTAB_N, DRAPESUCK_B, DRAPESUCK_B_S, DRAPESUCK_F,
		DRAPESUCK_F_S, DOWN_S, DOWN_P,								// DOWN_S가 누웠다 일어나는거, P가 엎어졌다 일어나는거

		ANIMINDEX_END
	};

public:
	// 디버깅 하려고 override뺀것
	void OnSpawn(void* _arg) override;
	void OnDespawn() override;

public:
	static Slave_Vampire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;


	// MinionBase을(를) 통해 상속됨
	HRESULT Set_Animation() override;

};

NS_END
