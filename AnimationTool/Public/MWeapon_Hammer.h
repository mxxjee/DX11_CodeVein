#pragma once
#include "AnimationTool_Define.h"
#include "Monster_Weapon.h"
#include "Monster.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

class MWeapon_Hammer final : public Monster_Weapon
{
public:
	typedef struct tagWeaponHammerDesc : Monster_Weapon::MONSTERWEAPON_DESC
	{
		wstring wstrModelTag;
	}WEAPONHAMMER_DESC;

private:
	explicit MWeapon_Hammer();
	explicit MWeapon_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit MWeapon_Hammer(const MWeapon_Hammer& original);
	virtual ~MWeapon_Hammer();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	HRESULT Ready_Components(const wstring& wstrModelTag);
	HRESULT Bind_ShaderResources();

public:
	static MWeapon_Hammer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};