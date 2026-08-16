#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Timer;
class GameClock;

class ENGINE_DLL TimerManager final : public Base
{
private:
	explicit TimerManager();
	virtual ~TimerManager();

public:
	HRESULT Add_Timer(_wstring _timername);
	GameClock* Add_Clock(const _wstring& _clockName, _float _targetFPS);
	_float Compute_Timer(_wstring _timername);
	_float Get_Time(_wstring _timername);

	GameClock* Get_Clock(const _wstring& _clockName) { return m_umapClockTimer.contains(_clockName) ? m_umapClockTimer[_clockName] : nullptr;};

private:
	unordered_map<_wstring, Timer*> m_umapTimer;
	UMAP<_wstring, class GameClock*> m_umapClockTimer;

public:
	static TimerManager* Create();

public:
	void Free() override final;

};

NS_END