#include "Client_Define.h"
#include "UIObj_ShortCutQuickSlot.h"
#include "InventoryManager.h"
#include "UI_Progress.h"
#include "UIObj_SlotGrid_Shortcut.h"
#include "UIObj_ShortCutMenu.h"
#include "UIObj_BitmapFont.h"

UIObj_ShortCutQuickSlot::UIObj_ShortCutQuickSlot()
{
}

UIObj_ShortCutQuickSlot::UIObj_ShortCutQuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObj_Slot(pDevice, pContext)
{
}

UIObj_ShortCutQuickSlot::UIObj_ShortCutQuickSlot(const UIObj_ShortCutQuickSlot& original)
	:UIObj_Slot(original)
{
}

UIObj_ShortCutQuickSlot::~UIObj_ShortCutQuickSlot()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ShortCutQuickSlot::Initialize_Prototype(_uint iLevel)
{
    CHECK_FAILED(__super::Initialize_Prototype(iLevel), E_FAIL);
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ShortCutQuickSlot::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	
	m_pGameInstance->Subscribe<tagShortCutQuickSlotEvent>([this](const tagShortCutQuickSlotEvent& Event)
		{

			if (Event.eType == EventType::UPDATE_MENUMODE)
			{
				if (Event.ShortCutMenuMode == _UINT(UIObj_ShortCutMenu::ShortCutMenu_State::DEFAULT))
				{
					m_bShortCutDefault = true;


				}

				else if (Event.ShortCutMenuMode == _UINT(UIObj_ShortCutMenu::ShortCutMenu_State::EXTRA))
				{
					m_bShortCutDefault = false;


				}
			}
			

			else if (Event.eType == EventType::UPDATE_EQUIPDATA)
			{
				const vector < ItemInfo*> Datats = m_pInventoryManager->Get_DisplayQuickSlot();
				Equip(Datats[m_iSlotIdx]);
			}
		});
    
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



_int UIObj_ShortCutQuickSlot::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int UIObj_ShortCutQuickSlot::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int UIObj_ShortCutQuickSlot::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	//아이템이 하나라도 장착되어을떄.
	if (m_pInventoryManager->Can_Active_ShortCutMenu())
	{
		_int iFocusIdx = m_pInventoryManager->Get_ShortcutFocusIdx();

		_int iDiff = m_iSlotIdx - iFocusIdx;

		if (iDiff > 3) iDiff -= 6;
		else if (iDiff <= -3) iDiff += 6;

		Move_By_FocusIdx(iDiff,fTimeDelta,true);
		Check_State(iDiff,true);
		Update_State(fTimeDelta);
	}

	return 0;
}

HRESULT UIObj_ShortCutQuickSlot::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return S_OK;
}

void Client::UIObj_ShortCutQuickSlot::Move_By_FocusIdx(int iDiff, const _float fTimeDelta, bool bLerp)
{
	//슬롯개수와 현재 focus에 맞춰서 처리
	_int iMaxCount = 6;
	


	//목적지 계산
	float fTargetX = iDiff * 18.f;

	// 보간
	if (bLerp)
	{
		float fNewX = lerp<float>(m_Local.m_fX, fTargetX, static_cast<_float>( fTimeDelta * 10.f));
		Set_Position(fNewX, m_Local.m_fY);

	}

	else
		Set_Position(fTargetX, m_Local.m_fY);
}

void Client::UIObj_ShortCutQuickSlot::Size_By_FocusIdx(int iDiff)
{
	iDiff = abs(iDiff);

	if (iDiff == 1)//거리가 1이라면 바로 양옆슬롯들
		Set_Size(m_vSideSize.x, m_vSideSize.y);

	else if (iDiff == 1)
		Set_Size(m_vInitSize.x, m_vInitSize.y);


}



void Client::UIObj_ShortCutQuickSlot::Change_State(ShortcutQuickSlotState SlotState, bool bLerp)
{
	if (m_eSlotState != SlotState)
	{
		switch (SlotState)
		{
		case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::FOCUS:
		{
			
			Set_Visible(true);
			if (!m_bIsActive)
				Set_Active(true);

			Set_Size(m_vInitSize.x, m_vInitSize.y);
			Set_Alpha(1.f);
			Set_CombinedAlpha(1.f);

			Set_Color(false);
		}
			break;

		case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::SIDE:
		{
			Set_Visible(true);
			Set_Alpha(1.f);
			if (!m_bIsActive)
				Set_Active(true);
			

			if(!bLerp)
				Set_Size(m_vSideSize.x, m_vSideSize.y);
				
			Set_Color(true);
		}
			break;


		//
		case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::HIDDEN:
		{
		
			if (m_eSlotState == ShortcutQuickSlotState::SIDE)
			{
				//이전상태가 side였다가 hidden로간거면,,
				Play_Animation(MoveAnimKey);
			}

			else
				Set_Visible(false);

		}
			break;
		case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::END:
			break;
		default:
			break;
		}

		m_eSlotState = SlotState;
	}
}

void Client::UIObj_ShortCutQuickSlot::Check_State(int iDiff, bool bLerp)
{
	ShortcutQuickSlotState eState = Get_SlotState(iDiff);
	
	//if (iAbsDiff == 0)	//거리가 0 = 내가 포커스인덱스
	//	Change_State(ShortcutQuickSlotState::FOCUS, bLerp);

	//else if (iAbsDiff == 1)//거리가 1이라면 바로 양옆슬롯들
	//	Change_State(ShortcutQuickSlotState::SIDE, bLerp);

	//else
	//	Change_State(ShortcutQuickSlotState::HIDDEN, bLerp);

	Change_State(eState, bLerp);

}

void Client::UIObj_ShortCutQuickSlot::Update_State(const _float fTimeDelta)
{

	switch (m_eSlotState)
	{
	case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::FOCUS:
	{


	}
	break;

	case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::SIDE:
	{
		if (m_bShortCutDefault)
		{
			Set_Alpha(0.f);
			Set_Visible(false);
		}
		else
		{
			Set_Visible(true);
			Set_Alpha(1.f);
			Set_CombinedAlpha(1.f);

			

			if (!m_bIsActive)
				Set_Active(true);
			 

			//		}
			_float fTargetSizeX = lerp<_float>(m_Local.m_fSizeX, m_vSideSize.x, (fTimeDelta * 15.f));
			_float fTargetSizeY = lerp<_float>(m_Local.m_fSizeY, m_vSideSize.y, (fTimeDelta * 15.f));

			Set_Size(fTargetSizeX, fTargetSizeY);

		}
		break;

	case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::HIDDEN:
	{
		if (m_Local.m_fAlpha < 0.5f)
			Set_Visible(false);


	}
	break;

	case Client::UIObj_ShortCutQuickSlot::ShortcutQuickSlotState::END:
		break;
	default:
		break;
	}


	}
}

void Client::UIObj_ShortCutQuickSlot::Set_Color(bool bDark)
{
	CHECK_JUST_NULL(m_pIcon);
	CHECK_JUST_NULL(m_pCountText);

	UI_Image* pImage = dynamic_cast<UI_Image*>(m_pIcon->Get_UIRenderComponent());
	UI_BitmapText* pTextComp = dynamic_cast<UI_BitmapText*>(m_pCountText->Get_Component_FromName(Proto_BitmapText));

	
	CHECK_JUST_NULL(pImage);
	CHECK_JUST_NULL(pTextComp);


	if (bDark)
	{
		pImage->Set_Color(_float4(0.8f, 0.8f,0.8f, 0.5f));
		pTextComp->Set_Color(_float4(0.6549f,
			0.6470f,
			0.501f,
			0.5f));

	}

	else
	{
		pImage->Set_Color(_float4(1.f, 1.f, 1.f, 1.f));
		pTextComp->Set_Color(_float4(1.0f,
			0.9901f,
			0.7982f,
			1.0f));
	}

}

////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_ShortCutQuickSlot::After_ApplyData()
{

    __super::After_ApplyData();

	m_vInitSize = _float2(m_Local.m_fSizeX, m_Local.m_fSizeY);
	m_vSideSize = m_vInitSize * 0.8f;

}
////////////////////////////////////////////////////////////////////////////////////////

HRESULT Client::UIObj_ShortCutQuickSlot::Equip(ItemInfo* pInfo)
{

	if (FAILED(__super::Equip(pInfo)))
		return E_FAIL;
	return S_OK;

}
/******************************************************* 객체 준비 함수 *******************************************************/

////////////////////////////////////////////////////////////////////////////////////////
void Client::UIObj_ShortCutQuickSlot::Set_Active(_bool _isActive)
{
	//활성화했을때바로 자리,크기 위치하기.
	if (_isActive)
	{
		_int iFocusIdx = m_pInventoryManager->Get_ShortcutFocusIdx();
		_int iDiff = m_iSlotIdx - iFocusIdx;

		if (iDiff > 3) iDiff -= 6;
		else if (iDiff <= -3) iDiff += 6;

		float fTargetX = iDiff * 18.f;


		if (iDiff == 1) // 오른쪽에서 들어오는 놈
			m_Local.m_fX = fTargetX + 18.f;
		else if (iDiff == -1) // 왼쪽에서 들어오는 놈
			m_Local.m_fX = fTargetX - 18.f;
		else
			m_Local.m_fX = fTargetX;

		// 이 '약간 바깥쪽' 좌표를 현재 위치로 세팅
		Set_Position(m_Local.m_fX, m_Local.m_fY);

		// 대신 알파를 0으로 둬서 튀는 걸 방지!
		Set_Alpha(0.f);
		
		ShortcutQuickSlotState eState = Get_SlotState(iDiff);
		if (eState == ShortcutQuickSlotState::SIDE||
			eState == ShortcutQuickSlotState::FOCUS)
		{
			
			Set_Visible(true);
			Set_Alpha(1.f);
			Set_CombinedAlpha(1.f);

		}
	}

	__super::Set_Active(_isActive);
	
	




}

void Client::UIObj_ShortCutQuickSlot::Set_Visible(_bool _isVisible)
{
	UIObject::Set_Visible(_isVisible);
}
////////////////////////////////////////////////////////////////////////////////////////

UIObj_ShortCutQuickSlot::ShortcutQuickSlotState Client::UIObj_ShortCutQuickSlot::Get_SlotState(int iDiff)
{
	_int iAbsDiff = abs(iDiff);

	if (iAbsDiff == 0)	//거리가 0 = 내가 포커스인덱스
		return ShortcutQuickSlotState::FOCUS;

	else if (iAbsDiff == 1)//거리가 1이라면 바로 양옆슬롯들
		return ShortcutQuickSlotState::SIDE;

	else
		return ShortcutQuickSlotState::HIDDEN;
}

//////////////////////////////////////////////////////////////////////
void UIObj_ShortCutQuickSlot::Free()
{
	__super::Free();
}

UIObj_ShortCutQuickSlot* Client::UIObj_ShortCutQuickSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_ShortCutQuickSlot* pInstance = new UIObj_ShortCutQuickSlot(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ShortCutQuickSlot 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}


GameObject* UIObj_ShortCutQuickSlot::Clone(void* pArg)
{
	UIObj_ShortCutQuickSlot* pInstance = new UIObj_ShortCutQuickSlot(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ShortCutQuickSlot 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
