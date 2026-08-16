#include "MT_Defines.h"
#include "Player_Weapon.h"


Player_Weapon::Player_Weapon()
{
}

Player_Weapon::Player_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice,pContext)
{
}

Player_Weapon::Player_Weapon(const Player_Weapon& original)
	:PartObject(original)
{
}

Player_Weapon::~Player_Weapon()
{
}

HRESULT Player_Weapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Player_Weapon::Initialize(void* arg)
{
	PLAYERWEAPON_DESC Desc = {};
	PLAYERWEAPON_DESC* pDesc = static_cast<PLAYERWEAPON_DESC*>(arg);

	if (arg == nullptr)
	{
		arg = &Desc;
	}
	else
	{

		m_pSocketMatrix = pDesc->pSocketMatrix;
		m_eWeaponType = pDesc->eWeaponType;
	}

	CHECK_FAILED(PartObject::Initialize(pDesc), E_FAIL);

	return S_OK;
}

_int Player_Weapon::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int Player_Weapon::Update_Parallel(const _float fTimeDelta)
{
	return 0;
}

_int Player_Weapon::Update(const _float fTimeDelta)
{
	return 0;
}

_int Player_Weapon::Update_Late(const _float fTimeDelta)
{
	return 0;
}

HRESULT Player_Weapon::Render(const _float fTimeDelta)
{
	return S_OK;
}

void Player_Weapon::Free()
{
	__super::Free();
}
