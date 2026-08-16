#pragma once
#include "Monster.h"


NS_BEGIN(Engine)
class TrailEffect;
NS_END

NS_BEGIN(Client)

class Giant_Vampire :
    public Monster
{
public:
	enum ANIMINDEX
	{
		IDLE,
		DEATH,


		MELEE0,	//쉴드이펙트
		MELEE1,//휘감아베기
		MELEE2,//휘감아베기 2번
		MELEE3,//쩜공
		
		RUN_F,
		 WALK_F,WALK_L,WALK_R,

		 TURN180L,TURN180R,TURN90_L,TURN90_R,

		 BACKSTAB_N, DRAPESUCK_B, DRAPESUCK_B_S, DRAPESUCK_F, DRAPESUCK_F_S,
		 DOWN_S,DOWN_P,
		//앞오른쪽     //앞오른쪽 //뒤왼쪽  //뒤 오른쪽
		DAMAGE_FL,DAMAGE_FR , DAMAGE_BL, DAMAGE_BR,//데미지 바얗ㅇ에 따른 애니ㅔㅁ이션
		DAMAGE_STRONG_F,DAMAGE_STRONG_B,

		REPEL,
		STUN_N,STUN_E,

		ANIMINDEX_END
	};
private:
	explicit Giant_Vampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Giant_Vampire(const Giant_Vampire& original);
	virtual ~Giant_Vampire();

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

private:
	void Update_Trail();

public:
	// 디버깅 하려고 override뺀것
	void OnSpawn(void* _arg) override;
	void OnDespawn() override;
	virtual void OnDamaged(const DAMAGE_EVENT& _damageEvent) override;

private:
	// Sword Trail 관련 변수
	vector<TrailEffect*>		m_vecTrailEffects;
	_bool						m_bTrailActive = {};
	class ParticleSystem* m_pShieldEffect = nullptr;

public:
	static Giant_Vampire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;


	// MinionBase을(를) 통해 상속됨
	HRESULT Set_Animation() override;
};

NS_END