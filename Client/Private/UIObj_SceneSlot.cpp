#include "Client_Define.h"
#include "UIObj_SceneSlot.h"
#include "UIObj_SceneSelectMenu.h"

Client::UIObj_SceneSlot::UIObj_SceneSlot()
{
}

Client::UIObj_SceneSlot::UIObj_SceneSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice, pContext)
{
}

Client::UIObj_SceneSlot::UIObj_SceneSlot(const UIObj_SceneSlot& original)
	:UIObject(original)
{
}

Client::UIObj_SceneSlot::~UIObj_SceneSlot()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SceneSlot::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_SceneSlot::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	
	m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<SceneSlotEvent>([this](const SceneSlotEvent& e)
		{
			m_iFocusIdx = e.m_iFocusIdx;
		
			if (e.m_iFocusIdx == m_iIdx)
				m_eSlotState = SceneSlot_State::EXTRA;

			else
				m_eSlotState = SceneSlot_State::DEFAULT;


			Change_State();
		}));


	m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<SceneSlotUpdateColorEvent>([this](const SceneSlotUpdateColorEvent& e)
		{
			if (e.m_iObjectID != m_iObjectID)
				return;


			Change_Texture(e.TexKey);
		}));
	return S_OK;
}

_int Client::UIObj_SceneSlot::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int Client::UIObj_SceneSlot::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int Client::UIObj_SceneSlot::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	_int iDiff = m_iIdx - m_iFocusIdx;
	
	//if (iDiff > 3) iDiff -= 3;
	//else if (iDiff <= -2) iDiff += 2;
	Move_By_FocusIdx(iDiff, fTimeDelta,true);


	return 0;
}

HRESULT Client::UIObj_SceneSlot::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);

	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SceneSlot::After_ApplyData()
{
	__super::After_ApplyData();

	//자식캐싱
	wstring strName[] = { L"Arrow_Left",L"Arrow" };
	for (auto& str : strName)
	{
		UIObject* pObj = Get_Child(str);
		if (pObj)
			Arrows.push_back(pObj);


	}


	m_pFocusMenu = Get_Child(L"FocusMenu");
	m_pBottom_Back = Get_Child(L"Slot_Bottom_Back");
	m_pSceneType = Get_Child(L"SceneType");


	UIObject* pLevelPic = Get_Child(L"Level_Pic");
	if (pLevelPic)
	{
		m_pImageComp = dynamic_cast<UI_Image*>(pLevelPic->Get_UIRenderComponent());
	}
	//m_fInitSize = _float2(m_Local.m_fSizeX, m_Local.m_fSizeY);


	m_fInitPosition = _float2(m_Local.m_fX, m_Local.m_fY);
	Bind_Arrow_Func();


}
/******************************************************* 객체 준비 함수 *******************************************************/

UIObj_SceneSlot* Client::UIObj_SceneSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_SceneSlot* pInstance = new UIObj_SceneSlot(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SceneSlot 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_SceneSlot::Clone(void* pArg)
{
	UIObj_SceneSlot* pInstance = new UIObj_SceneSlot(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SceneSlot 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

void Client::UIObj_SceneSlot::Change_State()
{
	if (m_eSlotState != m_ePreSlotState)
	{
		switch (m_eSlotState)
		{
		case SceneSlot_State::EXTRA:
		{
			for (auto& pArrow : Arrows)
			{
				pArrow->Set_Visible(true);
				pArrow->Set_Active(true);

			}
			m_pFocusMenu->Set_Visible(true);
			m_pFocusMenu->Set_Active(true);

			m_pBottom_Back->Set_Active(true);
			m_pSceneType->Set_Active(true);

			if(m_pImageComp)
				m_pImageComp->Set_Color(_float4(1.f,1.f,1.f,1.f));

			Set_Size(m_fInitSize.x , m_fInitSize.y );

		}
			break;

		case SceneSlot_State::DEFAULT:
		{
			for (auto& pArrow : Arrows)
			{
				pArrow->Set_Visible(false);
				pArrow->Set_Active(false);
			}
				

			m_pFocusMenu->Set_Visible(false);
			m_pFocusMenu->Set_Active(false);
			
			m_pBottom_Back->Set_Active(false);
			m_pSceneType->Set_Active(false);

			if (m_pImageComp)
				m_pImageComp->Set_Color(_float4(0.45f, 0.45f, 0.45f, 1.f));

			Set_Size(m_fInitSize.x * m_fRatio, m_fInitSize.y * m_fRatio);

		}
			break;

		default:
			break;
		}
	}
	
}

void Client::UIObj_SceneSlot::Change_Texture(wstring TexName)
{
	CHECK_JUST_NULL(m_pImageComp);

	m_pImageComp->Change_Texture(wstringToString(TexName));

}

void Client::UIObj_SceneSlot::Move_By_FocusIdx(int iDiff, const _float fTimeDelta, bool bLerp)
{
#pragma region 수정한거..
	////슬롯개수와 현재 focus에 
	//// 맞춰서 처리
	////Diff값에 맞춰서 루프돌기.. 원점에서부터 diff값 -1까지는 30. 
	//m_OffSetX = 0.f;
	//int NewDiff = fabs(iDiff);

	//bool bMinus = false;
	//if (iDiff < 0)
	//	bMinus = true;

	//if (NewDiff != 0)
	//{
	//	for (int i = 0; i < NewDiff; ++i)
	//	{
	//		m_OffSetX += 28.f;
	//		--NewDiff;
	//	}

	//	if(NewDiff ==1)
	//		m_OffSetX += 24.f;
	//}
	//

	////if (fabs(iDiff) != 1)
	////	m_OffSetX = 25.f;

	////else
	////	m_OffSetX = 30.f;

	////목적지 계산
	//if (bMinus)
	//	m_OffSetX *= -1.f;

	//float fTargetX = m_fInitOrigin.x +  m_OffSetX;

	//// 보간
	//if (bLerp)
	//{
	//	//float fNewX = lerp<float>(m_Local.m_fX, fTargetX, EaseInQuad(static_cast<_float>(fTimeDelta * 15.f)));
	//	//float fNewX = lerp<float>(m_Local.m_fX, fTargetX, static_cast<_float>(fTimeDelta * 15.f));
	//	float fNewX = MoveTowards(m_Local.m_fX, fTargetX, static_cast<_float>(fTimeDelta * 50.f));
	//	Set_Position(fNewX, m_Local.m_fY);

	//}

	//else
	//	Set_Position(fTargetX, m_Local.m_fY);

#pragma endregion
// 1. 설정값 정의 (필요하면 멤버 변수로 빼도 됨)
	float fBigWidth = m_fInitSize.x;
	float fSmallWidth = m_fInitSize.x * m_fRatio;
	float fGap = -7.f; // 슬롯 간 간격

	float fTargetX = 0.f;


	if (iDiff == 0) {
		fTargetX = m_fInitOrigin.x; 
	}
	else {
		// 포커스 슬롯의 반폭 + 갭 + (iDiff - 1) * (작은 슬롯 폭 + 갭) + 작은 슬롯의 반폭
		// iDiff가 1일 때: 중심에서 갭만큼 떨어짐
		// iDiff가 2일 때: 중심에서 갭 + 작은 슬롯 폭 + 갭만큼 떨어짐

		float fOffsetFromCenter = (fBigWidth / 2.f) + fGap + (fSmallWidth / 2.f);

		if (iDiff > 0) { // 오른쪽
			fTargetX = m_fInitOrigin.x + fOffsetFromCenter + (iDiff - 1) * (fSmallWidth + fGap);
		}
		else { // 왼쪽
			fTargetX = m_fInitOrigin.x - (fOffsetFromCenter + (abs(iDiff) - 1) * (fSmallWidth + fGap));
		}
	}

	float fNewX = bLerp ? lerp<float>(m_Local.m_fX, fTargetX, static_cast<_float>(fTimeDelta * 10.f)) : fTargetX;
	Set_Position(fNewX, m_Local.m_fY);
}

void Client::UIObj_SceneSlot::Set_Active(_bool _isActive)
{
	//활성화했을때바로 자리,크기 위치하기.
	if (_isActive)
	{
		Set_Position(m_fInitPosition.x, m_fInitPosition.y);
		_int iDiff = m_iIdx - m_iFocusIdx;
 		

	}

	__super::Set_Active(_isActive);






}

void Client::UIObj_SceneSlot::Bind_Arrow_Func()
{
	//	wstring strName[] = { L"Arrow_Left",L"Arrow" };
	for (auto& pObj : Arrows)
	{
		if (pObj)
		{
			UI_Button* pButtonComp = DCAST(UI_Button*)(pObj->Get_Component_FromName(Proto_UIButton));
			if (pButtonComp&&pObj->Get_Name() == L"Arrow_Left")
			{
				pButtonComp->Set_OnClickFunc([this]()
					{
						//이벤트전송
						UIObj_SceneSelectMenu::SceneSlotMenuEvent event;
						event.eType = UIObj_SceneSelectMenu::SelectMenuEvenType::MINUS_IDX;
						m_pGameInstance->Publish(event);
					
					});
			}

			else
			{
				pButtonComp->Set_OnClickFunc([this]()
					{
						//이벤트전송
						UIObj_SceneSelectMenu::SceneSlotMenuEvent event;
						event.eType = UIObj_SceneSelectMenu::SelectMenuEvenType::PLUS_IDX;
						m_pGameInstance->Publish(event);

					});
			}
		}
	}
}


void Client::UIObj_SceneSlot::Free()
{
	__super::Free();

}
