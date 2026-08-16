#pragma once

#include "AnimationTool_Define.h"
#include "PartObject.h"
#include "Monster.h"

class Monster_Weapon : public PartObject
{
public:
	typedef struct tagWeaponDesc : PartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr }; //부착할 뼈의 행렬
		WEAPON_TYPE		 eWeaponType = { WEAPON_TYPE::WP_END };

	}MONSTERWEAPON_DESC;

protected:
	explicit Monster_Weapon();
	explicit Monster_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Monster_Weapon(const Monster_Weapon& original);
	virtual ~Monster_Weapon();

public:
	WEAPON_TYPE Get_WeaponType() const { return m_eWeaponType; }

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
	WEAPON_TYPE		 m_eWeaponType = { WEAPON_TYPE::WP_END };

public:
	void Free() override;

};