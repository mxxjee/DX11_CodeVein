#pragma once
#include "Stat.h"

NS_BEGIN(Engine)
class ENGINE_DLL Weapon_Stat : public Stat
{
public:
	typedef struct tagWeaponStatDesc : public Stat::STAT_DESC
	{
		//몬스터는 스탯이 필요해보이는게 잘 모르겠음

	}WEAPONSTAT_DESC;

private:
	explicit Weapon_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Weapon_Stat(const Weapon_Stat& original);
	virtual ~Weapon_Stat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* arg) override;

private:
	WEAPONSTAT_DESC m_WeaponStat = {};

public:
	static Weapon_Stat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;



};
NS_END
