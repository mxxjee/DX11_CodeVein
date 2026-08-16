#include "Engine_Define.h"
#include "Weapon_Stat.h"

Engine::Weapon_Stat::Weapon_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Stat(pDevice,pContext)
{
}

Engine::Weapon_Stat::Weapon_Stat(const Weapon_Stat& original)
	:Stat(original)
{
}

HRESULT Engine::Weapon_Stat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::Weapon_Stat::Initialize(void* arg)
{
	__super::Initialize(arg);

	WEAPONSTAT_DESC* Desc = static_cast<WEAPONSTAT_DESC*>(arg);



	return S_OK;
}

Weapon_Stat* Weapon_Stat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Weapon_Stat* pInstance = new Weapon_Stat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Weapon_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component* Engine::Weapon_Stat::Clone(void* pArg)
{
	Weapon_Stat* pInstance = new Weapon_Stat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Weapon_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::Weapon_Stat::Free()
{
	__super::Free();
}
