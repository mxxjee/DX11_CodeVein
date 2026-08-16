#include "Client_Define.h"
#include "UIObj_Selector.h"

Client::UIObj_Selector::UIObj_Selector()
{
}

Client::UIObj_Selector::UIObj_Selector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice, pContext)
{
}


Client::UIObj_Selector::UIObj_Selector(const UIObj_Selector& original)
	:UIObject(original)
{
}

Client::UIObj_Selector::~UIObj_Selector()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Selector::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_Selector::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	

	return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


_int Client::UIObj_Selector::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::UIObj_Selector::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int Client::UIObj_Selector::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	return 0;
}

HRESULT Client::UIObj_Selector::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_Selector* Client::UIObj_Selector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_Selector* pInstance = new UIObj_Selector(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Selector 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_Selector::Clone(void* pArg)
{
	UIObj_Selector* pInstance = new UIObj_Selector(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Selector 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::UIObj_Selector::MoveTo( _float fY)
{
	if(!m_bIsActive)
		Set_Active(true);


	Set_Position(m_Local.m_fX,fY);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Selector::After_ApplyData()
{
	__super::After_ApplyData();

	m_pLine = Get_Child(L"Line");

	m_InitScale = _float2(m_Local.m_fSizeX, m_Local.m_fSizeY);

}
/******************************************************* 객체 준비 함수 *******************************************************/


//////////////////////////////////////////////////////////////////////////////
void Client::UIObj_Selector::Free()
{
	__super::Free();
}

