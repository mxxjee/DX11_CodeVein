#include "Client_Define.h"
#include "UIObj_NpcDialogue.h"
#include "UIObj_Text.h"

Client::UIObj_NpcDialogue::UIObj_NpcDialogue()
{
}

Client::UIObj_NpcDialogue::UIObj_NpcDialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice,pContext)
{
}

Client::UIObj_NpcDialogue::UIObj_NpcDialogue(const UIObj_NpcDialogue& original)
	:UIObject(original)
{
}

Client::UIObj_NpcDialogue::~UIObj_NpcDialogue()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_NpcDialogue::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_NpcDialogue::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	m_pGameInstance->Subscribe< DialogueUIEvent>([this](const DialogueUIEvent& Event)
		{
			

			switch (Event.eType)
			{
			
			case DIALOGUEUIEVENT_TYPE::UPDATE_DESC:
			{
				Set_Active(true);
				if (m_pNPCDesc_Text)
					m_pNPCDesc_Text->Set_Text(Event.m_Text);

			}
				break;

			case DIALOGUEUIEVENT_TYPE::UPDATE_NAME:
			{
				Set_Active(true);
				if (m_pNPCName_Text)
					m_pNPCName_Text->Set_Text(Event.m_Text);

			}
				break;
			case DIALOGUEUIEVENT_TYPE::EXIT:
			{
				Set_Active(false);
			}
			break;
			default:
				break;
			}

		});
    return S_OK;
}

_int Client::UIObj_NpcDialogue::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::UIObj_NpcDialogue::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;

}

_int Client::UIObj_NpcDialogue::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	return 0;
}

HRESULT Client::UIObj_NpcDialogue::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return S_OK;
}

UIObj_NpcDialogue* Client::UIObj_NpcDialogue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_NpcDialogue* pInstance = new UIObj_NpcDialogue(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_NpcDialogue 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_NpcDialogue::Clone(void* pArg)
{
	UIObj_NpcDialogue* pInstance = new UIObj_NpcDialogue(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_NpcDialogue 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_NpcDialogue::After_ApplyData()
{
	__super::After_ApplyData();

    //자식캐싱
	m_pNPCName_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"Text_Name"));
	m_pNPCDesc_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"Text_Desc"));


}


void Client::UIObj_NpcDialogue::Free()
{
	__super::Free();
}
