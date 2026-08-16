#pragma once

#include "Client_Define.h"
#include "Monster.h"

NS_BEGIN(Engine)
class TrailEffect;
NS_END

NS_BEGIN(Client)

class GhostKnight_Halberd final : public Monster
{
private:
	explicit GhostKnight_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GhostKnight_Halberd(const GhostKnight_Halberd& original);
	virtual ~GhostKnight_Halberd();


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
	HRESULT Ready_PartOjbect();
	HRESULT Ready_PhysXEvent();
	HRESULT Ready_Event();
	HRESULT Ready_States();
	HRESULT Ready_Value();
	HRESULT Ready_Socket();

	HRESULT Bind_ShaderResources();

	_uint Select_NextPattern() override;

	enum ANIMINDEX {
		IDLE_N, IDLE_LOOK, TURN90_L, TURN180_L, TURN90_R,
		TURN180_R, RUN_F, DRAPESUCK_F, DRAPESUCK_B, DEATH_N,
		DEATH_F, DEATH_B, ATTACK_0, ATTACK_2, ATTACK_3,
		ATTACK_2_CUT, ATTACK_3_CUT, ATTACK_JUMP_START, ATTACK_JUMP_END, BACKSTAB_N,
		REPEL, DOWN_P, DOWN_S, STUN_N, STUN_E,

		ANIMINDEX_END
	};

public:
	// 디버깅 하려고 override뺀것
	void OnSpawn(void* _arg) override;
	void OnDespawn() override;
	void Update_Trail();

private:
	// Sword Trail 관련 변수
	vector<TrailEffect*>		m_vecTrailEffects;
	_bool						m_bTrailActive = {};

public:
	static GhostKnight_Halberd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;


	// MinionBase을(를) 통해 상속됨
	HRESULT Set_Animation() override;

};

NS_END
