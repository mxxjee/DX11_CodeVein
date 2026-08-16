#include "Engine_Define.h"
#include "Monster_Stat.h"

Engine::Monster_Stat::Monster_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Stat(pDevice,pContext)
{
}

Engine::Monster_Stat::Monster_Stat(const Monster_Stat& original)
	:Stat(original)
{
}

HRESULT Engine::Monster_Stat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::Monster_Stat::Initialize(void* arg)
{
	__super::Initialize(arg);

	MONSTERSTAT_DESC* Desc = static_cast<MONSTERSTAT_DESC*>(arg);

	//공통된 스탯들은 부모함수에 전달되므로 자식 변수들만 저장해서 사용


	return S_OK;
}

Monster_Stat* Monster_Stat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Monster_Stat* pInstance = new Monster_Stat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Monster_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component* Engine::Monster_Stat::Clone(void* pArg)
{
	Monster_Stat* pInstance = new Monster_Stat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Monster_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::Monster_Stat::Free()
{
	__super::Free();
}
