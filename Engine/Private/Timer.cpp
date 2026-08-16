#include "Engine_Define.h"
#include "Timer.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Timer::Timer()
{
}

Engine::Timer::~Timer()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Timer::Initialize()
{
	QueryPerformanceCounter(&m_iFrameTime);
	QueryPerformanceCounter(&m_iFixTime);
	QueryPerformanceCounter(&m_iLastTime);

	QueryPerformanceFrequency(&m_iCpuTick);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_float Engine::Timer::Compute_Timer()
{
	QueryPerformanceCounter(&m_iFrameTime);

	if (m_iFrameTime.QuadPart - m_iFixTime.QuadPart >= m_iCpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_iCpuTick);

		m_iFixTime = m_iFrameTime;
	}

	m_fTimeDelta = _float(m_iFrameTime.QuadPart - m_iLastTime.QuadPart) / static_cast<_float>(m_iCpuTick.QuadPart);

	m_iLastTime = m_iFrameTime;

	return m_fTimeDelta;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Timer* Engine::Timer::Create()
{
	Timer* pInstance = new Timer();

	MSG_FAIL(pInstance->Initialize(), L"Timer Create Failed", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Timer::Free()
{
	__super::Free();

}
/******************************************************* 객체 반환 함수 *******************************************************/
