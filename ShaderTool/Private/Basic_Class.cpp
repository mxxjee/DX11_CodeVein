#include "ShaderTool_Define.h"
#include "Basic_Class.h"

//#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
ShaderTool::Basic_Class::Basic_Class()
{
}

ShaderTool::Basic_Class::Basic_Class(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: __super(pDevice, pContext)
{
}

ShaderTool::Basic_Class::Basic_Class(const Basic_Class& original)
	: __super(original)
{
}

ShaderTool::Basic_Class::~Basic_Class()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Basic_Class::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT ShaderTool::Basic_Class::Initialize(void* arg)
{
    static _uint namenum = 0;

    GAMEOBJECT_DESC desc;

    if (arg == nullptr)
    {
        arg = &desc;
    }
    else
    {

    }

    CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Basic_Class_" + to_wstring(namenum++);

    CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

    CHECK_FAILED(Ready_Components(), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Basic_Class::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Basic_Class::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Basic_Class::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Basic_Class::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Basic_Class::Render(const _float fTimeDelta)
{
    MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Basic_Class::Bind_ShaderResources()
{
    return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Basic_Class* ShaderTool::Basic_Class::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Basic_Class* pInstance = new Basic_Class(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Basic_Class 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* ShaderTool::Basic_Class::Clone(void* pArg)
{
	Basic_Class* pInstance = new Basic_Class(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Basic_Class 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::Basic_Class::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
