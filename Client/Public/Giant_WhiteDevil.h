#pragma once
#include "BossBase.h"

NS_BEGIN(Engine)
class TrailEffect;
NS_END

NS_BEGIN(Client)

class Giant_WhiteDevil :
	public BossBase
{
public:
	enum ANIMINDEX
	{
		AWAKE_IDLE,//AWAKE 대기상태
		AWAKE,	//AWAKE 수행애님
		IDLE,
		DEATH,

		///////////NormallAttack - MS_ATTACKMELEE///////////////
		NOMRAL_MELEE0, //AttackNomral01 ~ AttackNomral03으로 연계가능
		NOMRAL_MELEE1,//AttackNomral03
		///////////////////////////////////////

		///////////strongAttack -///////////////
		STRONG_MELEE0,	//AS_TGiantWhite_THalberd_AttackStrong01_N
		STRONG_MELEE1,	//콤보가능 MELEE1,MEELE2
		STRONG_MELEE2,
		///////////////////////////////////////

		///////////FAR_Attack -  MS_FARFATTACK///////////////
		SPIN_ATTACK,


		///////////SpecialAttack -  MS_ATTACKSPECIAL///////////////
		SKILL_BUFFSELF,
		SKILL_POINTSHOOT,
		///////////////////////////////////////

		/////////////Movement///////////////////
		RUN_F,								//AS_TGiantWhite_THalberd_Run_N_Loop
		WALK_F, WALK_L, WALK_R, WALK_B,   //AS_TGiantWhite_THalberd_Walk_R_Loop
		///////////////////////////////////////

		/////////////Turn///////////////////
		TURN180L, TURN180R, TURN90_L, TURN90_R,
		///////////////////////////////////////

		/////////////Evade///////////////////
		DODGE_L, DODGE_R,			//피함
		///////////////////////////////////////


		/////////////Damage///////////////////
		DAMAGE_FL, DAMAGE_FR, DAMAGE_BL, DAMAGE_BR,
		DAMAGE_STRONG_FL, DAMAGE_STRONG_FR, DAMAGE_STRONG_BL, DAMAGE_STRONG_BR,
		///////////////////////////////////////
		ANIMINDEX_END
	};

private:
	explicit Giant_WhiteDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Giant_WhiteDevil(const Giant_WhiteDevil& original);
	virtual ~Giant_WhiteDevil();

public:
	HRESULT Initialize_Prototype(LEVEL _level) override final;
	HRESULT Initialize(void* _arg) override final;
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update_Priority(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta) override final;
	HRESULT Render(const _float fTimeDelta)	override final;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

public:
	virtual void    OnAwarePlayer();   //몬스터가 플레이어를  감지한 첫 순간만 발동하는함수(보통 보스를 위해 사용)

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

	void Set_Offset(const _float fTimeDelta);
	void Change_Phase() override;
public:
	// 디버깅 하려고 override뺀것
	void OnSpawn(void* _arg) override;
	void OnDespawn() override;
	void Update_Trail();

private:
	// Sword Trail 관련 변수
	vector<TrailEffect*>		m_vecTrailEffects;
	_bool						m_bTrailActive = {};
	_bool						m_bPhase2 = {};

public:
	static Giant_WhiteDevil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	//virtual void    OnAwarePlayer();   //몬스터가 플레이어를  감지한 첫 순간만 발동하는함수(보통 보스를 위해 사용)
	virtual void   OnDead();
private:
	_float m_fPrevYOffset = {};
	_float m_fCurrentYOffset = {};
	_uint m_iPrevAnimIndex = {};
	_bool m_bOffsetChanged = {};
	Alarm m_YOffsetAlarm = {};

public:
	void Free() override final;

};
NS_END
