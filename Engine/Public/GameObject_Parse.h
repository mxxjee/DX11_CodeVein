#pragma once

#include "GameObject.h"


/* 템플릿 클래스라 헤더에 다 때려박음 */
/* Deprecated 클래스 파싱용으로 상속받게 하려고 했는데 다른 방식으로 변경됨 */
/* 템플릿 클래스 참고용으로 남겨둠 */
NS_BEGIN(Engine)

template <typename OBJParse>
class GameObject_Parse abstract : public GameObject
{
protected:
	explicit GameObject_Parse();
	explicit GameObject_Parse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GameObject_Parse(const GameObject_Parse& original);
	virtual ~GameObject_Parse();

public:
	// 부모 클래스에서 상속받은 함수
	// 자식 클래스에서 이 함수를 따로 선언하지 않으면 이 함수가 실행됨
	void Set_TypeName() override { s_strTypename = wstringToString(m_wstrPrototypeName); }
	void Set_TypeName(const _string& _typename) override { s_strTypename = _typename; }

protected:
	_wstring m_wstrModelName = {};
	static _string s_strTypename;  // OBJParse마다 별도로 생성됨

public:
	void Free() override;
};

// static 변수 정의
template <typename OBJParse>
_string GameObject_Parse<OBJParse>::s_strTypename = {};

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
template <typename OBJParse>
GameObject_Parse<OBJParse>::GameObject_Parse()
{
}

template <typename OBJParse>
GameObject_Parse<OBJParse>::GameObject_Parse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
}

template <typename OBJParse>
GameObject_Parse<OBJParse>::GameObject_Parse(const GameObject_Parse& original)
	: GameObject(original)
{
}

template <typename OBJParse>
GameObject_Parse<OBJParse>::~GameObject_Parse()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
template <typename OBJParse>
void GameObject_Parse<OBJParse>::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

NS_END