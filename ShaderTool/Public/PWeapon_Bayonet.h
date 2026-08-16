#pragma once

#include "ShaderTool_Define.h"
#include "Player_Weapon.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(ShaderTool)
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
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
	//프레임이 떨어지냐고 묻는다면 대답해주는게 인지상정!
public:
	virtual _int RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static PWeapon_Bayonet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};
NS_END
