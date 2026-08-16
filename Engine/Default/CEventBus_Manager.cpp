#include "CEventBus_Manager.h"

CEventBus_Manager::CEventBus_Manager()
{
}

HRESULT CEventBus_Manager::Initialize()
{
	return S_OK;
}

void CEventBus_Manager::Emit(const GameEvent& Event)
{
	m_GameEventQue.push_back(Event);
}

_uint CEventBus_Manager::RegisterListners(const string& CBName, EventCallBack Callback)
{
	_uint iResultHandle = 0;
	auto iter = m_EventCB.find(CBName);
	if (iter == m_EventCB.end())
		iResultHandle = 0;

	else
		iResultHandle = iter->second.size();

	m_EventCB[CBName][iResultHandle] = Callback;


	return iResultHandle;
}

void CEventBus_Manager::UnRegisterListenrs(const string& CBName, _uint iHandle)
{
	auto iter = m_EventCB.find(CBName);
	if (iter == m_EventCB.end())
		return;


	auto EraseHandle = iter->second.find(iHandle);
	if(EraseHandle!=iter->second.end())
		iter->second.erase(EraseHandle);


}

void CEventBus_Manager::DisPatch(_float fTimeDelta)
{
	for (auto& event : m_GameEventQue)
	{
		auto Target = m_EventCB.find(event.Name);

		if (Target == m_EventCB.end())		//이 이벤트와 맞는 이름을 가진 콜백함수가없네유..
			continue;

		for (auto& pair : Target->second)
		{
			if (pair.second != nullptr)
				pair.second(event);
		}
			


	}

	m_GameEventQue.clear();
}

CEventBus_Manager* CEventBus_Manager::Create()
{
	CEventBus_Manager* pInstance = new CEventBus_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CEventBus_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventBus_Manager::Free()
{
	__super::Free();
	m_GameEventQue.clear();

	for (auto& pair : m_EventCB)
	{
		
		pair.second.clear();
	}


	m_EventCB.clear();


}
