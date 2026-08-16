#pragma once
#include "AnimationTool_Define.h"
#include "ContainerObject.h"
#include "Monster_Weapon.h"
#include "MWeapon_Hammer.h"

NS_BEGIN(Engine)
class TrailEffect;
NS_END

NS_BEGIN(AnimationTool)
class GiantVampire : public Monster
{
private:
	explicit GiantVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GiantVampire(const GiantVampire& original);
	virtual ~GiantVampire();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg) override final;
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update_Priority(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta) override final;
	HRESULT Render(const _float fTimeDelta)	override final;
	HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum);

	HRESULT Ready_PartObjects();

public:
	virtual	class Monster_Weapon* Get_ActiveWeapon() override { return m_pActiveWeapon; }

private:
	HRESULT Ready_Components();
	HRESULT Ready_Event();
	HRESULT Bind_ShaderResources();
	void Update_WeaponPosition();
	void Update_Trail();

	class Monster_Weapon* m_pActiveWeapon = { nullptr };
	class Monster_Weapon* m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };
	
	_float3     m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
	_float3     m_vCurrentWeaponPos = {};

	const _float4x4* m_pWeaponBoneMatrix = {};			// 무기 본 위치
	const _float4x4* m_pWeaponBoneMatrixEnd = {};
	
	_float4x4* m_pWeaponTrailTip = { nullptr };
	_float4x4* m_pWeaponTrailRoot = { nullptr };


	// Sword Trail 관련 변수
	vector<TrailEffect*>		m_vecTrailEffects;
	_bool						m_bTrailActive = {};

public:
	static GiantVampire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};

NS_END
