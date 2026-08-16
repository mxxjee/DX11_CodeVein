#include "Client_Define.h"
#include "UIObj_PaletteArea.h"
#include "UIObj_PaletteDisplay.h"

#include "Mouse.h"
#include "CustomizingManager.h"
#include "GameInstance.h"
#include "Camera.h"

Client::UIObj_PaletteArea::UIObj_PaletteArea()
{
}

Client::UIObj_PaletteArea::UIObj_PaletteArea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice,pContext)
{
}

Client::UIObj_PaletteArea::UIObj_PaletteArea(const UIObj_PaletteArea& original)
	:UIObject(original)
{
}

Client::UIObj_PaletteArea::~UIObj_PaletteArea()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_PaletteArea::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_PaletteArea::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	
	m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< PaletteAreaUIEvent>([this](const PaletteAreaUIEvent& e)
		{
			CHECK_JUST_NULL(m_pCursor);
			switch (e.eType)
			{
			case PaletteAreaUIEventType::OPEN_PALETTE:
			{
				CHECK_JUST_NULL(e.ColorInfo);
				//값세팅

				m_pCursor->Set_Position(e.ColorInfo->vCursorPos.x, e.ColorInfo->vCursorPos.y);
				
				Set_CursorUV(Get_CursorUV(e.ColorInfo->vCursorPos));
				

				UIObj_PaletteDisplay::PaletteDisplayUIEvent Event;
				Event.eType = UIObj_PaletteDisplay::EventType::UPDATE_DISPLAYCOLOR;
				Event.vRGBColor = e.ColorInfo->vColor;

				m_pGameInstance->Publish(Event);

				CustomizingManager::GetInstance()->Update_CurrentTargetField(&Event.vRGBColor);

			}
				break;
			default:
				break;
			}

		}));

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_PaletteArea::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int Client::UIObj_PaletteArea::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);

	return 0;
}

_int Client::UIObj_PaletteArea::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	return 0;
}

HRESULT Client::UIObj_PaletteArea::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);

	return S_OK;
}

///////////////////////////////////////////////////
UIObj_PaletteArea* Client::UIObj_PaletteArea::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_PaletteArea* pInstance = new UIObj_PaletteArea(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_PaletteArea 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_PaletteArea::Clone(void* pArg)
{
	UIObj_PaletteArea* pInstance = new UIObj_PaletteArea(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PaletteArea 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
////////////////////////////////////////////////////

void    Client::UIObj_PaletteArea::After_ApplyData()
{
	__super::After_ApplyData();


	m_pCursor =Get_Child(L"Palette_Cursor");
	
}


void    Client::UIObj_PaletteArea::OnDragging()
{
	/*영역에서 못벗어나게하기*/

	_float2 mouseRatio = Get_MouseRatio();

	if (m_pCursor)
	{

		float fLimitRatioX = (m_Local.m_fSizeX - m_pCursor->Get_SizeX()) / m_Local.m_fSizeX;
		float fLimitRatioY = (m_Local.m_fSizeY - m_pCursor->Get_SizeY()) / m_Local.m_fSizeY;

		float fTargetX = (mouseRatio.x - 0.5f) * fLimitRatioX * m_Local.m_fSizeX;
		float fTargetY = (mouseRatio.y - 0.5f) * fLimitRatioY * m_Local.m_fSizeY;

		// 커서 위치 설정 
		m_pCursor->Set_Position(fTargetX / 2.f, fTargetY / 2.f);

		_float2 CursorPos = _float2(m_pCursor->Get_X(), m_pCursor->Get_Y());

		m_fUV = Get_CursorUV(CursorPos);


	
		Set_DisplayColor();
	}
	
}

_float2 Client::UIObj_PaletteArea::Get_CursorUV(_float2 CursorPos)
{
	
	float fLimitRatioX = (m_Local.m_fSizeX - m_pCursor->Get_SizeX()) / m_Local.m_fSizeX;
	float fLimitRatioY = (m_Local.m_fSizeY - m_pCursor->Get_SizeY()) / m_Local.m_fSizeY;

	float fRangeX = fLimitRatioX * m_Local.m_fSizeX;
	float fRangeY = fLimitRatioY * m_Local.m_fSizeY;

	float u = (fRangeX > 0.f) ? (CursorPos.x * 2.f / fRangeX) + 0.5f : 0.5f;
	float v = (fRangeY > 0.f) ? (CursorPos.y * 2.f / fRangeY) + 0.5f : 0.5f;


	return _float2(u, v);

}

void Client::UIObj_PaletteArea::Set_DisplayColor()
{
	//색상보내기
	_float H = fmod(m_fUV.x * 360.0f, 360.0f);//원래 함수랑 맞춰야해서 360곱해버리기
	_float S = m_fSaturation;	//슬라이더값받아오기
	_float V = 1.0f - m_fUV.y; 

	_float4 RGB = HSVtoRGB(H, S, V);
	UIObj_PaletteDisplay::PaletteDisplayUIEvent Event;
	Event.eType = UIObj_PaletteDisplay::EventType::UPDATE_DISPLAYCOLOR;
	Event.vRGBColor = RGB;

	m_pGameInstance->Publish(Event);


	/////////////////////////////////////////////////////
	//지금 focus되어있는 selector에게도 보내기(저장도함께)
	Export_Data.fSaturate = m_fSaturation;
	Export_Data.vColor = RGB;

	CustomizingManager::GetInstance()->Update_CustomData(CUSTOMIZING_VALUE_TYPE::COLOR,&Export_Data);
	

}

_float2 Client::UIObj_PaletteArea::Get_CursorPosFromRGB(_float4 vRGB)
{
	if (!m_pCursor) return _float2(0.f, 0.f);


	_float3 hsv = RGBtoHSV(vRGB);

	float u = hsv.x / 360.0f;
	float v = 1.0f - hsv.z; 


	float fLimitRatioX = (m_Local.m_fSizeX - m_pCursor->Get_SizeX()) / m_Local.m_fSizeX;
	float fLimitRatioY = (m_Local.m_fSizeY - m_pCursor->Get_SizeY()) / m_Local.m_fSizeY;


	float fTargetX = (u - 0.5f) * fLimitRatioX * m_Local.m_fSizeX;
	float fTargetY = (v - 0.5f) * fLimitRatioY * m_Local.m_fSizeY;


	return _float2(fTargetX / 2.f, fTargetY / 2.f);
}
_float Client::UIObj_PaletteArea::Get_SaturationFromRGB(_float4 vRGB)
{
	_float r = vRGB.x;
	_float g = vRGB.y;
	_float b = vRGB.z;

	_float maxVal = max(max(r, g), b);
	_float minVal = min(min(r, g), b);
	_float delta = maxVal - minVal;

	if (maxVal <= 0.f)
		return 0.f;

	// 채도 S = (Max - Min) / Max
	return delta / maxVal;
}
_float2 Client::UIObj_PaletteArea::Get_MouseRatio()
{
	POINT ptMouse = m_pMouse->Get_MousePos();

	float fWidth = m_LocalHitBoxArea.R - m_LocalHitBoxArea.L;
	float fHeight = m_LocalHitBoxArea.B - m_LocalHitBoxArea.T;

	if (fWidth <= 0.f || fHeight <= 0.f) return _float2(0.5f, 0.5f);

	float u = clamp((ptMouse.x - m_LocalHitBoxArea.L) / fWidth, 0.f, 1.f);
	float v = clamp((ptMouse.y - m_LocalHitBoxArea.T) / fHeight, 0.f, 1.f);

	return _float2(u, v);
}
void Client::UIObj_PaletteArea::Free()
{
	__super::Free();
}


