#pragma once
#include "AnimationTool_Define.h"
#include "Player_Weapon.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(AnimationTool)
class PWeapon_Sword final : public Player_Weapon
{
public:
	typedef struct tagWeaponSwordDesc : Player_Weapon::PLAYERWEAPON_DESC
	{

	}WEAPONSWORD_DESC;

private:
	explicit PWeapon_Sword();
	explicit PWeapon_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit PWeapon_Sword(const PWeapon_Sword& original);
	virtual ~PWeapon_Sword();

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
	static PWeapon_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};

NS_END