#pragma once

#include "EventManager.h"
#include "FileManager.h"

NS_BEGIN(Engine)

#pragma region EventManager
template <typename Event>
EventHandle Engine::GameInstance::Subscribe(std::function<void(const Event&)> _callback)
{
    return m_pEventManager->Subscribe<Event>(_callback);
}

template<typename Event>
void Engine::GameInstance::Publish(const Event& _eventData)
{
    m_pEventManager->Publish<Event>(_eventData);
}

template<typename Event>
void Engine::GameInstance::Publish_Stack(const Event& _eventData)
{
    m_pEventManager->Publish_Stack(_eventData);
}

template<typename T>
inline bool GameInstance::LoadFromJson(const string& _filePath, T& _outData, FileHeader* _outHeader)
{
    return m_pFileManagaer->LoadFromJson(_filePath, _outData, _outHeader);
}
#pragma endregion EventManager

NS_END