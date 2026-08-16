#pragma once
#include "ContainerObject.h"

#include "AnimationTool_Define.h"
#include "Monster_Weapon.h"

#include "MWeapon_Hammer.h"

NS_BEGIN(AnimationTool)
class GiantVampire_Weapon :
    public ContainerObject
{
private:
	explicit GiantVampire_Weapon();
	explicit GiantVampire_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GiantVampire_Weapon(const GiantVampire_Weapon& original);
	virtual ~GiantVampire_Weapon();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg) override final;
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update_Priority(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta) override final;
	HRESULT Render(const _float fTimeDelta)	override final;
	HRESULT Ready_PartObjects();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Update_WeaponPosition();

	class Monster_Weapon* m_pActiveWeapon = { nullptr };
	class Monster_Weapon* m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };

	_bool       m_bAttackSweepActive = { false };	// 공격중인지
	_float      m_fAttackRadius = { 0.5f };			// 공격 범위
	_float      m_fAttackDamage = { 10.f };			// 공격 데미지
	_float      m_fAttackHalfHeight = { 0.f };		// 공격 반높이
	_float3     m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
	_float3     m_vCurrentWeaponPos = {};			// 현재 프레임 무기 위치
	set<_uint>  m_setHitTargets = {};				// 이미 공격한 몬스터 저장
	_uint       m_iColliderEventHandle = {};		// 
	_float		m_fKnockbackForce = {};
	const _float4x4* m_pWeaponBoneMatrix = {};			// 무기 본 위치
	const _float4x4* m_pWeaponBoneMatrixEnd = {};			// 무기 본 위치
	_float4x4* m_pWeaponTrailTip = { nullptr };
	_float4x4* m_pWeaponTrailRoot = { nullptr };

public:
	static GiantVampire_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END