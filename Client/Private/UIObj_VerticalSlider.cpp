#include "Client_Define.h"
#include "UIObj_VerticalSlider.h"
#include "Mouse.h"
#include "Camera.h"

Client::UIObj_VerticalSlider::UIObj_VerticalSlider()
{
}

Client::UIObj_VerticalSlider::UIObj_VerticalSlider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_VerticalSlider::UIObj_VerticalSlider(const UIObj_VerticalSlider& original)
    :UIObject(original)
{
}

Client::UIObj_VerticalSlider::~UIObj_VerticalSlider()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_VerticalSlider::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_VerticalSlider::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< VerticalSliderUIEvent>([this](const VerticalSliderUIEvent& e)
		{
			switch (e.eType)
			{
			case VerticalSliderUIEventType::OPEN_PALETTE:
			{
				CHECK_JUST_NULL(e.ColorInfo);
				//값세팅
				Set_SliderByLocalPosY(e.ColorInfo->SliderCursorPosY);
			}
			break;
			default:
				break;
			}

		}));


	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////



void    Client::UIObj_VerticalSlider::After_ApplyData()
{
	__super::After_ApplyData();

                                                                                       
    m_pCursor = Get_Child(L"Slider_Cursor");


}

void    Client::UIObj_VerticalSlider::OnDragging()
{
	/*영역에서 못벗어나게하기*/
	m_fValue = Get_MouseRatio();
	COUT("CurrentValue:" << m_fValue);

	if (m_pCursor)
	{

		float fLimitRatioX = (m_Local.m_fSizeX - m_pCursor->Get_SizeX()) / m_Local.m_fSizeX;
		float fLimitRatioY = (m_Local.m_fSizeY - m_pCursor->Get_SizeY()) / m_Local.m_fSizeY;

		float fTargetY = (m_fValue - 0.5f) * fLimitRatioY * m_Local.m_fSizeY;

		// 커서 위치 설정 
   		m_pCursor->Set_Position(m_pCursor->Get_X(), fTargetY / 2.f);
	}

	Update_CursorValue();
}


_float Client::UIObj_VerticalSlider::Get_MouseRatio()
{
	POINT ptMouse = m_pMouse->Get_MousePos();
	float fHeight = m_LocalHitBoxArea.B - m_LocalHitBoxArea.T;

	if (fHeight <= 0.f) return 0.5f;

	float v = clamp((ptMouse.y - m_LocalHitBoxArea.T) / fHeight, m_fMinValue, m_fMaxValue);


	return v;
}
void Client::UIObj_VerticalSlider::Set_SliderByLocalPosY(_float fLocalPosY)
{
	if (!m_pCursor) return;

	// 커서의 실제 로컬 Y 위치를 강제로 세팅
	m_pCursor->Set_Position(m_pCursor->Get_X(), fLocalPosY);

	float fLimitRatioY = (m_Local.m_fSizeY - m_pCursor->Get_SizeY()) / m_Local.m_fSizeY;
	float fFullRangeY = fLimitRatioY * m_Local.m_fSizeY;

	if (fFullRangeY <= 0.f) {
		m_fValue = 0.5f;
	}
	else {
		
		float v = (fLocalPosY * 2.f / fFullRangeY) + 0.5f;
		m_fValue = clamp(v, m_fMinValue, m_fMaxValue);
	}

	// 값이 바뀌었으니 이전 값 갱신 (Update_CursorValue가 중복 호출되지 않게 하기 위함)
	Init_CursorValue();
}
void Client::UIObj_VerticalSlider::Update_CursorValue()
{
	
	if (m_UpdateFunc)
		m_UpdateFunc();

	m_fPreValue = m_fValue;



}
void Client::UIObj_VerticalSlider::Init_CursorValue()
{
	if (m_InitFunc)
		m_InitFunc();

	m_fPreValue = m_fValue;
}
_float Client::UIObj_VerticalSlider::Get_CursorPosYFromValue(_float fValue)
{
	if (!m_pCursor) return 0.f;

	float fLimitRatioY = (m_Local.m_fSizeY - m_pCursor->Get_SizeY()) / m_Local.m_fSizeY;

	float fFullRangeY = fLimitRatioY * m_Local.m_fSizeY;
	float fTargetY = (fValue - 0.5f) * fFullRangeY;

	return fTargetY / 2.f;
}
void Client::UIObj_VerticalSlider::Reset_Cursor_And_Value(_float CursorPosY)
{
	//위치세팅하고
	CHECK_JUST_NULL(m_pCursor);
	m_pCursor->Set_Position(m_pCursor->Get_X(), CursorPosY);


	//바로 값까지 갱신한다.
	Set_Value(CursorPosY);
}
void Client::UIObj_VerticalSlider::Set_Value(_float CursorPosY)
{

	float fCurY = CursorPosY;

	float fLimitRatioY = (m_Local.m_fSizeY - m_pCursor->Get_SizeY()) / m_Local.m_fSizeY;
	float fRangeY = fLimitRatioY * m_Local.m_fSizeY;

	if (fRangeY <= 0.f)
		m_fValue = 0.5f;

	/* 역산 공식:
	   fCurY = (value - 0.5f) * fRangeY / 2.0f
	   따라서 value = (fCurY * 2.0f / fRangeY) + 0.5f
	*/
	float v = (fCurY * 2.f / fRangeY) + 0.5f;

	
	m_fValue=clamp(v, m_fMinValue, m_fMaxValue);
	
	// ? 필요한감 ? Update_CursorValue();
}

void        Client::UIObj_VerticalSlider::Set_Active(_bool _isActive)
{

	//if (_isActive)
	//{
	//	Set_Value()
	//}

	__super::Set_Active(_isActive);
}
///////////////////////////////////////////////////
UIObj_VerticalSlider* Client::UIObj_VerticalSlider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_VerticalSlider* pInstance = new UIObj_VerticalSlider(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_VerticalSlider 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_VerticalSlider::Clone(void* pArg)
{
	UIObj_VerticalSlider* pInstance = new UIObj_VerticalSlider(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_VerticalSlider 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
////////////////////////////////////////////////////




void Client::UIObj_VerticalSlider::Free()
{
	__super::Free();
}


