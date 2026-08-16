#include "Engine_Define.h"
#include "TimerManager.h"
#include "Timer.h"
#include "GameClock.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::TimerManager::TimerManager()
{
}

Engine::TimerManager::~TimerManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::TimerManager::Add_Timer(_wstring _timername)
{
	if (m_umapTimer.contains(_timername))
	{
		_wstring message = L"Timer \"" + _timername + L"\" Already exists";
		MSG_ON(message.c_str(), L"Add Failed");
		BREAK;
		return E_FAIL;
	}
	
	Timer* timer = Timer::Create();

	m_umapTimer.emplace(_timername, timer);

	return S_OK;
}

GameClock* Engine::TimerManager::Add_Clock(const _wstring& _clockName, _float _targetFPS)
{
	if (m_umapClockTimer.contains(_clockName))
	{
		MSG_ON(L"돌아가", L"이걸 누가 보겠누");
		BREAK;
	}

	GameClock* clock = GameClock::Create(_targetFPS);
	m_umapClockTimer.emplace(_clockName, clock);
	return clock;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_float Engine::TimerManager::Compute_Timer(_wstring _timername)
{
	if (!m_umapTimer.contains(_timername))
	{
		_wstring message = L"Timer \"" + _timername + L"\" does not exist.";
		MSG_ON(message.c_str(), L"Find Failed");
		BREAK;
		return 0.f;
	}

	return m_umapTimer.find(_timername)->second->Compute_Timer();
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 델타타임 획득 함수 ////////////////////////////////////////////////////////
_float Engine::TimerManager::Get_Time(_wstring _timername)
{
	return m_umapTimer.find(_timername)->second->Get_Time();
}
/******************************************************* 델타타임 획득 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
TimerManager* Engine::TimerManager::Create()
{
	return new TimerManager();
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::TimerManager::Free()
{
	__super::Free();

	for (auto& timer : m_umapTimer)
	{
		Safe_Release(timer.second);
	}
	m_umapTimer.clear();

	for (auto& timer : m_umapClockTimer)
	{
		Safe_Release(timer.second);
	}
	m_umapClockTimer.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/
