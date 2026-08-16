#pragma once

#include "Base.h"
#include <chrono>

NS_BEGIN(Engine)

class ENGINE_DLL GameClock final : public Base
{
public:
	// 프레임 타이밍 정보를 한 번에 전달하는 구조체
	struct FrameInfo
	{
		_float	fDeltaTime = {};	// 실제 경과 시간 (렌더링용)
		_float	fFixedDelta = {};	// 고정 시간 간격 (물리/로직용)
		_float	fAlpha = {};	// 보간 비율 0~1 (렌더링 보간용)
		_uint	iFixedStepCount = {};	// 이번 프레임에 수행할 고정 업데이트 횟수
		_float	fTotalTime = {};	// 게임 시작 이후 총 경과 시간
		_float	fTimeScale = {};	// 현재 시간 배율
		_bool	bCanUpdate = {};
	};

private:
	GameClock();
	virtual ~GameClock() override;

private:
	HRESULT		Initialize(_float _fixedDelta);

public:
	FrameInfo	Tick();

	void		Set_FixedTimeStep(_float _fixedDelta) { m_fFixedDelta = _fixedDelta; }
	void		Set_TimeScale(_float _scale) { m_fTimeScale = _scale; }
	void		Set_MaxFrameTime(_float _max) { m_fMaxFrameTime = _max; }

	_float		Get_TotalTime() const { return m_fTotalTime; }
	_float		Get_TimeScale() const { return m_fTimeScale; }
	_float		Get_DeltaTime() const { return m_fDeltaTime; }
	_float		Get_FixedTime() const { return m_fFixedDelta; }
public:
	static GameClock* Create(_float _fixedDelta = 1.f / 60.f);

private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;
	using Duration = std::chrono::duration<_float>;

	TimePoint	m_lastTime = {};
	_float		m_fAccumulator = {};
	_float		m_fTotalTime = {};
	_float		m_fFixedDelta = {};
	_float		m_fMaxFrameTime = { 0.25f };	// 스파이크 클램프 상한
	_float		m_fTimeScale = { 1.f };		// 슬로우모션 등에 활용
	_float		m_fDeltaTime = {}; //이거 시호가 fTimeDelta 초기화시점에 가져오고싶어서 추가함 

	// 프레임 제한용 변수
	_float m_fTargetFrameTime = {};
	_float m_fFrameAccumulator = {};

public:
	virtual void Free() override final;

};

NS_END
