#include "Engine_Define.h"
#include "Base.h"

Base::Base()
{
    static _uint gNum = 0;
    //static bool s_bInitialized = false;
    //if (!s_bInitialized)
    //{
    //    staticNum = 0;
    //    s_bInitialized = true;
    //}


	m_iObjectNum = gNum;
	++gNum;
}

Base::~Base()
{
}

_uint Base::AddRef()
{
	//cout << "RefCnt Increased ";
	++m_iReferenceCount;
	if (g_bPrintRefCnt)
	{
		Print_CurrentRefCount();
	}
	return m_iReferenceCount;
}

_uint Engine::Base::AddRef_PerFrame()
{
	return ++m_iReferenceCount;
}

_uint Base::Release()
{
	if (m_iReferenceCount == 0)
	{
        _string typeidasdf = typeid(*this).name();
		if (g_bPrintRefCnt)
		{
			COUT("[" << typeid(*this).name() << m_iObjectNum << "] destroyed.");
		}

		Free();

#ifdef _DEBUG
        delete this;
#else
        if(m_iObjectNum != 0)
			    delete this;
#endif

        return 0;
	}

	//cout << "RefCnt Decreased ";
	--m_iReferenceCount;
	if (g_bPrintRefCnt)	
	{
		Print_CurrentRefCount();
	}
	return m_iReferenceCount + 1;
}

_uint Engine::Base::Release_PerFrame()
{
	if (m_iReferenceCount == 0)
	{
		COUT("[" << typeid(*this).name() << m_iObjectNum << "] destroyed.");

		Free();

		delete this;

		return 0;
	}

	return m_iReferenceCount--;
}

_uint Engine::Base::Print_CurrentRefCount()
{
#ifdef _DEBUG
	WCOUT("[" << typeid(*this).name() << m_iObjectNum << "] Ref_Cnt : " << m_iReferenceCount);
#endif
	return m_iReferenceCount;
}

_uint Engine::Base::Print_CurrentRefCount(const _wstring& _objectname)
{
#ifdef _DEBUG
	WCOUT("[" << _objectname << m_iObjectNum << "] Ref_Cnt : " << m_iReferenceCount);
#endif
	return m_iReferenceCount;
}

void Engine::Base::Free()
{
}
