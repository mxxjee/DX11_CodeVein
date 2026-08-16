#pragma once
#include "Monster.h"

NS_BEGIN(Engine)
class TrailEffect;
NS_END

NS_BEGIN(Client)

class Monkey_Devil :
    public Monster
{

public:
	enum ANIMINDEX
	{
		IDLE,
		DEATH,

		MELEE0,//그냥점어택
		MELEE1,//나대는점어택
		MELEE2,//수직으로내려꽂는어택

		RUN_F,
		WALK_F,WALK_L,WALK_R,WALK_B,

		TURN180L,TURN180R,TURN90_L,TURN90_R,

		DOWN_S,DOWN_P,

		DAMAGE_FL,DAMAGE_FR,DAMAGE_BL,DAMAGE_BR,
		ANIMINDEX_END
	};
private:
	explicit Monkey_Devil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Monkey_Devil(const Monkey_Devil& original);
	virtual ~Monkey_Devil();

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
	HRESULT Ready_Controller(); //피직스 세팅함수
	virtual HRESULT Ready_PartObjects();
	HRESULT Ready_Components();
	HRESULT Ready_PhysXEvent();
	HRESULT Ready_Event();
	HRESULT Ready_States();
	HRESULT Ready_Value();
	HRESULT Ready_Socket();

	HRESULT Bind_ShaderResources();

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
	static Monkey_Devil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;


	// MinionBase을(를) 통해 상속됨
	HRESULT Set_Animation() override;

};
NS_END
