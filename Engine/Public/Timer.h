#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Timer final : public Base
{
private:
	explicit Timer();
	virtual ~Timer();

public:
	HRESULT Initialize();
	_float Compute_Timer();
	_float Get_Time() { return m_fTimeDelta; }

private:
	LARGE_INTEGER m_iFrameTime = {};
	LARGE_INTEGER m_iFixTime = {};
	LARGE_INTEGER m_iLastTime = {};
	LARGE_INTEGER m_iCpuTick = {};

	_float m_fTimeDelta = {};

public:
	static Timer* Create();

public:
	void Free() override final;

};

NS_END