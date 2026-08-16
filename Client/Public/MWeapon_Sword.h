#pragma once
#include "Client_Define.h"
#include "Monster_Weapon.h"
#include "Monster.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(Client)
class MWeapon_Sword final : public Monster_Weapon
{
public:
	typedef struct tagWeaponSwordDesc : Monster_Weapon::MONSTERWEAPON_DESC
	{
		

	}WEAPONSWORD_DESC;

private:
	explicit MWeapon_Sword();
	explicit MWeapon_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit MWeapon_Sword(const MWeapon_Sword& original);
	virtual ~MWeapon_Sword();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static MWeapon_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END