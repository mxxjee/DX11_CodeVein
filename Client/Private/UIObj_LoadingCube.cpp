#include "Client_Define.h"
#include "UIObj_LoadingCube.h"

Client::UIObj_LoadingCube::UIObj_LoadingCube()
{
}

Client::UIObj_LoadingCube::UIObj_LoadingCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice,pContext)
{
}

Client::UIObj_LoadingCube::UIObj_LoadingCube(const UIObj_LoadingCube& original)
	:UIObject(original)
{
}

Client::UIObj_LoadingCube::~UIObj_LoadingCube()
{
}


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_LoadingCube::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_LoadingCube::Initialize(void* arg)
{

    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



_int Client::UIObj_LoadingCube::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::UIObj_LoadingCube::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int Client::UIObj_LoadingCube::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	return 0;
}

HRESULT Client::UIObj_LoadingCube::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return 0;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
UIObj_LoadingCube* Client::UIObj_LoadingCube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_LoadingCube* pInstance = new UIObj_LoadingCube(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_LoadingCube 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/


void        Client::UIObj_LoadingCube::After_ApplyData()
{
	//자식캐싱
	__super::After_ApplyData();

	//for (int i = 1; i <= 9; ++i)
	//{
	//	UIObject* pParts = Get_Child(to_wstring(i));
	//	m_Parts.push_back(pParts);

	//}

}

GameObject* Client::UIObj_LoadingCube::Clone(void* pArg)
{
	UIObj_LoadingCube* pInstance = new UIObj_LoadingCube(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_LoadingCube 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::UIObj_LoadingCube::Free()
{
	__super::Free();
}
