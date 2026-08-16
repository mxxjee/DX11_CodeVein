#pragma once
#include "Client_Define.h"
#include "Player_Weapon.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(Client)
class PWeapon_GreatSword final : public Player_Weapon
{
public:
	typedef struct tagWeaponGreatSwordDesc : Player_Weapon::PLAYERWEAPON_DESC
	{

	}WEAPONGREATSWORD_DESC;

private:
	explicit PWeapon_GreatSword();
	explicit PWeapon_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit PWeapon_GreatSword(const PWeapon_GreatSword& original);
	virtual ~PWeapon_GreatSword();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

public:
	virtual _int RequestAnimIndex(const ANIMREQUEST_DESC& tAnimRequest) override;
	virtual void Playing_WeaponClash_Sound() override;

private:
	virtual HRESULT Ready_Components() override;
	HRESULT Bind_ShaderResources();

public:
	static PWeapon_GreatSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};

NS_END