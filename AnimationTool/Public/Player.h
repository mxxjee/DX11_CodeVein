#pragma once
#include "AnimationTool_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class Model;
class TrailEffect;
NS_END

NS_BEGIN(AnimationTool)
class Player : public ContainerObject
{
private:
	explicit Player();
	explicit Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player(const Player& original);
	virtual ~Player();

public:
	class Player_Weapon* Get_ActivePlayerWeapon() { return m_pActiveWeapon; }
	class Player_BloodWeapon* Get_ActivePlayerBloodWeapon() { return m_pActiveBloodWeapon; }

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int    Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	class Player_MasterRig* m_pMasterRig = { nullptr };
	class Model* m_pMasterModel = { nullptr };
	
	class Player_Weapon* m_pActiveWeapon = { nullptr };
	class Player_Weapon* m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };
	class Player_BloodWeapon* m_pActiveBloodWeapon = { nullptr }; // 특수무기

	_uint m_iSoundEventHandle = {};

	_bool       m_bAttackSweepActive = { false };       // 현재 공격 판정 활성화 여부
	_float      m_fAttackRadius = { 0.f };              // Sweep 반지름
	_float      m_fAttackDamage = { 0.f };              // 데미지
	PX_ACTOR_TYPE m_eAttackerType = { PX_ACTOR_TYPE::END };
	_float3     m_vPrevWeaponPos = {};                  // 이전 프레임 무기 위치 (궤적 Sweep용)
	_float3     m_vCurrentWeaponPos = {};               // 현재 프레임 무기 위치
	set<_uint>  m_setHitTargets;                        // 이번 공격에 이미 맞은 대상 (중복 히트 방지)

	// Sword Trail 관련 변수
	vector<TrailEffect*>		m_vecTrailEffects;
	_bool						m_bTrailActive = {};
	_float4x4* m_pWeaponTrailTip = {};
	_float4x4* m_pWeaponTrailRoot = {};

	// Dissolve 관련 변수
	_bool						m_bDissolve = {};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Events();
	HRESULT Bind_ShaderResources();

public:
	static Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};
NS_END
