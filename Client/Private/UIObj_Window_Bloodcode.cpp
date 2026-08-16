#include "Client_Define.h"
#include "UIObj_Window_Bloodcode.h"
#include "UIObj_SlotGrid.h"
#include "UIObj_Text.h"

Client::UIObj_Window_Bloodcode::UIObj_Window_Bloodcode()
{
}

Client::UIObj_Window_Bloodcode::UIObj_Window_Bloodcode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObj_SubWindow(pDevice,pContext)
{
}

Client::UIObj_Window_Bloodcode::UIObj_Window_Bloodcode(const UIObj_Window_Bloodcode& original)
	:UIObj_SubWindow(original)
{
}

Client::UIObj_Window_Bloodcode::~UIObj_Window_Bloodcode()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_Bloodcode::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_Window_Bloodcode::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	
	return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/



_int Client::UIObj_Window_Bloodcode::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

    return 0;
}

_int Client::UIObj_Window_Bloodcode::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_Window_Bloodcode::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Window_Bloodcode::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_Window_Bloodcode::Free()
{
	__super::Free();
}
//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_Window_Bloodcode* Client::UIObj_Window_Bloodcode::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_Window_Bloodcode* pInstance = new UIObj_Window_Bloodcode(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_Bloodcode 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

UIObj_Window_Bloodcode* Client::UIObj_Window_Bloodcode::Clone(void* pArg)
{
	UIObj_Window_Bloodcode* pInstance = new UIObj_Window_Bloodcode(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_Bloodcode 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_Bloodcode::After_ApplyData()
{
	UIObject* pChild = Get_Child(L"SlotGrid");
	if (pChild)
	{
		m_SlotGrid = dynamic_cast<UIObj_SlotGrid*>(pChild);
		if (m_SlotGrid)
		{
			m_SlotGrid->Set_SlotGridType(UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL);
			m_SlotGrid->Set_Hover_AllSlots(false);
		
		}
	}

	pChild = Get_Child(L"Desc_Text_Name");
	if (pChild)
	{
		m_pDescTextName = dynamic_cast<UIObj_Text*>(pChild);

	}


	UIObject* pDescInfoText = Get_Child(L"Desc_Text_Info");
	if (pDescInfoText)
	{
		m_pDescInfoText = dynamic_cast<UIObj_Text*>(pDescInfoText);

	}

	UIObject* pDescIcon = Get_Child(L"SelectSlot_Icon");
	if (pDescIcon)
	{
		UIObject* pSelectIcon = pDescIcon->Get_Child(L"Slot_Icon");
		if (pSelectIcon)
			m_pDescIcon = pSelectIcon;

	}

}
/******************************************************* 객체 준비 함수 *******************************************************/
