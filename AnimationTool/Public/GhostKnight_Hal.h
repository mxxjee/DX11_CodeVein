#pragma once
#include "Monster.h"

#include "AnimationTool_Define.h"
#include "Monster_Weapon.h"
#include "MWeapon_Hammer.h"

NS_BEGIN(AnimationTool)
class GhostKnight_Hal :
    public Monster
{
private:
	explicit GhostKnight_Hal();
	explicit GhostKnight_Hal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GhostKnight_Hal(const GhostKnight_Hal& original);
	virtual ~GhostKnight_Hal();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg) override final;
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update_Priority(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta) override final;
	HRESULT Render(const _float fTimeDelta)	override final;
	HRESULT Ready_PartObjects();

	virtual	class Monster_Weapon* Get_ActiveWeapon() override { return m_pActiveWeapon; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Update_WeaponPosition();

	//class Monster_Weapon* m_pActiveWeapon = { nullptr };
	//class Monster_Weapon* m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };

	//_bool       m_bAttackSweepActive = { false };	// 공격중인지
	//_float      m_fAttackRadius = { 0.5f };			// 공격 범위
	//_float      m_fAttackDamage = { 10.f };			// 공격 데미지
	//_float      m_fAttackHalfHeight = { 0.f };		// 공격 반높이
	//_float3     m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
	//_float3     m_vCurrentWeaponPos = {};			// 현재 프레임 무기 위치
	//set<_uint>  m_setHitTargets = {};				// 이미 공격한 몬스터 저장
	//_uint       m_iColliderEventHandle = {};		// 
	//_float		m_fKnockbackForce = {};
	//const _float4x4* m_pWeaponBoneMatrix = {};			// 무기 본 위치

	//_float4x4* m_pWeaponTrailTip = { nullptr };
	//_float4x4* m_pWeaponTrailRoot = { nullptr };

public:
	static GhostKnight_Hal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};

NS_END
