#pragma once

#include "GameInstance.h"

NS_BEGIN(Engine)

#pragma region EventManager
template <typename Event>
EventHandle Engine::GameObject::Subscribe_Event(function<void(const Event&)> _callback)
{
	EventHandle handlenum = m_pGameInstance->Subscribe<Event>(_callback);

	m_vecSubscribeNumbers.push_back(handlenum);

	return handlenum;
}

template <typename Event>
void Engine::GameObject::Publish(const Event& _eventData)
{
	m_pGameInstance->Publish(_eventData);
}
#pragma endregion EventManager

NS_END