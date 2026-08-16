#pragma once

#include "ShaderTool_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(ShaderTool)
class Player_Weapon : public PartObject
{
public:
	typedef struct tagWeaponDesc : PartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr }; //부착할 뼈의 행렬
		WEAPON_TYPE		 eWeaponType = { WEAPON_TYPE::WP_END };

	}PLAYERWEAPON_DESC;

protected:
	explicit Player_Weapon();
	explicit Player_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Weapon(const Player_Weapon& original);
	virtual ~Player_Weapon();

public:
	WEAPON_TYPE Get_WeaponType() const { return m_eWeaponType; }
	_float3		Get_TrailRootOffset() const { return m_vTrailRootOffset; }
	_float3		Get_TrailTipOffset() const { return m_vTrailTipOffset; }

public:
	virtual _int RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest) = 0;

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

protected:
	const _float4x4* m_pSocketMatrix = { nullptr }; //구조체 받아서 저장할 변수
	WEAPON_TYPE			m_eWeaponType = { WEAPON_TYPE::WP_END };
	_float3				m_vTrailRootOffset = {};
	_float3				m_vTrailTipOffset = {};

public:
	void Free() override;

};
NS_END
