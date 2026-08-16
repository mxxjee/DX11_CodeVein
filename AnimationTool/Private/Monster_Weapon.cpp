#include "AnimationTool_Define.h"
#include "Monster_Weapon.h"


Monster_Weapon::Monster_Weapon()
{
}

Monster_Weapon::Monster_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:PartObject(pDevice,pContext)
{
}

Monster_Weapon::Monster_Weapon(const Monster_Weapon& original)
	:PartObject(original)
{
}

Monster_Weapon::~Monster_Weapon()
{
}

HRESULT Monster_Weapon::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Monster_Weapon::Initialize(void* arg)
{
	MONSTERWEAPON_DESC Desc = {};
	MONSTERWEAPON_DESC* pDesc = static_cast<MONSTERWEAPON_DESC*>(arg);

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

_int Monster_Weapon::Update_Priority(const _float fTimeDelta)
{
	return 0;
}

_int Monster_Weapon::Update_Parallel(const _float fTimeDelta)
{
	return 0;
}

_int Monster_Weapon::Update(const _float fTimeDelta)
{
	return 0;
}

_int Monster_Weapon::Update_Late(const _float fTimeDelta)
{
	return 0;
}

HRESULT Monster_Weapon::Render(const _float fTimeDelta)
{
	return S_OK;
}

void Monster_Weapon::Free()
{
	__super::Free();
}
