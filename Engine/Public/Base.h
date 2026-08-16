#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL Base abstract
{
protected:
	explicit Base();
	virtual ~Base();

public:
	/* 레퍼런스 카운트 증가 */
	_uint AddRef();
	_uint AddRef_PerFrame();

	/* 레퍼런스 카운트가 0이면 삭제 */
	/* 레퍼런스 카운트 감소 */
	_uint Release();
	_uint Release_PerFrame();

	/* 클래스 이름으로 콘솔창에 레퍼런스 카운트 출력 */
	_uint Print_CurrentRefCount();
	/* 받아온 이름으로 콘솔창에 레퍼런스 카운트 출력 */
	_uint Print_CurrentRefCount(const _wstring& _objectname);
	
protected:
	_uint m_iReferenceCount = {};
	_uint m_iObjectNum;

public:
	virtual void Free();

};

NS_END
