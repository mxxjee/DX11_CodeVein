#include "Engine_Define.h"
#include "State.h"
#include "GameInstance.h"

Engine::State::State() :m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT Engine::State::Initialize(GameObject* pOwner)
{
	if (pOwner == nullptr)
		return E_FAIL;

	m_pOwner = pOwner;

	return S_OK;
}

void Engine::State::On_ProjectileEvent()
{
}

void Engine::State::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
