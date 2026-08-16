#include "Engine_Define.h"
#include "GameClock.h"



//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::GameClock::GameClock()
{
}

Engine::GameClock::~GameClock()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameClock::Initialize(_float _fixedDelta)
{
	m_fFixedDelta = 1.f / 90.f;
	m_fTargetFrameTime = _fixedDelta;
	m_lastTime = Clock::now();

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


//////////////////////////////////////////////////////// 틱 함수 ////////////////////////////////////////////////////////
Engine::GameClock::FrameInfo Engine::GameClock::Tick()
{
	TimePoint currentTime = Clock::now();
	_float rawDelta = Duration(currentTime - m_lastTime).count();
	m_lastTime = currentTime;

	if (m_fTargetFrameTime > 0.f)
	{
		m_fFrameAccumulator += rawDelta;

		if (m_fFrameAccumulator < m_fTargetFrameTime)
		{
			FrameInfo info = {};
			info.bCanUpdate = false;
			return info;
		}

		rawDelta = m_fFrameAccumulator;
		m_fFrameAccumulator = 0.f;
	}

	// 디버그 브레이크, 탭 전환 등으로 인한 비정상 스파이크 방지
	if (rawDelta > m_fMaxFrameTime)
		rawDelta = m_fMaxFrameTime;

	// TimeScale 적용 (슬로우모션, 일시정지 등)
	_float scaledDelta = rawDelta * m_fTimeScale;

	m_fAccumulator += scaledDelta;
	m_fTotalTime += scaledDelta;

	// 고정 업데이트 횟수 계산
	_uint fixedStepCount = 0;
	while (m_fAccumulator >= m_fFixedDelta)
	{
		m_fAccumulator -= m_fFixedDelta;
		++fixedStepCount;
	}

	// 보간 비율 : 마지막 고정 스텝 이후 남은 시간 / 고정 간격
	_float alpha = m_fAccumulator / m_fFixedDelta;

	FrameInfo info = {};
	info.bCanUpdate = true;
	info.fDeltaTime = scaledDelta;
	m_fDeltaTime = info.fDeltaTime;
	info.fFixedDelta = m_fFixedDelta; 
	info.fAlpha = alpha;
	info.iFixedStepCount = fixedStepCount;
	info.fTotalTime = m_fTotalTime;
	info.fTimeScale = m_fTimeScale;

	return info;
}
/******************************************************* 틱 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GameClock* Engine::GameClock::Create(_float _fixedDelta)
{
	GameClock* pInstance = new GameClock();

	MSG_FAIL(pInstance->Initialize(_fixedDelta), L"GameClock Create Failed", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::GameClock::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/