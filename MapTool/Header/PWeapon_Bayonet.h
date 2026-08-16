#pragma once

#include "MT_Defines.h"
#include "Player_Weapon.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

class PWeapon_Bayonet final : public Player_Weapon
{
public:
	typedef struct tagWeaponBayonetDesc : Player_Weapon::PLAYERWEAPON_DESC
	{

	}WEAPONBAYONET_DESC;

private:
	explicit PWeapon_Bayonet();
	explicit PWeapon_Bayonet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit PWeapon_Bayonet(const PWeapon_Bayonet& original);
	virtual ~PWeapon_Bayonet();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static PWeapon_Bayonet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};
