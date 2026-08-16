#pragma once

#include "AnimationTool_Define.h"
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
class Animation;
class Model;
NS_END

NS_BEGIN(AnimationTool)
class Window_AnimTimeline final : public ImguiWindow
{
public:
	struct TimelineViewData
	{
		_float fZoom = { 1.f };  //전체 길이를 화면에 맞춤, 2.0일때는 2배확대하도록
		_float fPanTime = { 0.f }; //화면 왼쪽 끝에 표시되고 있는 시간
		_bool bPanning = { false }; // 화면이 확대된 상태에서 화면을 드래그하여 이동 (우클릭)
		_bool bScrubbing = { false }; // 마우스로 드래그하여 특정 프레임으로 빠르게 이동 (좌클릭)
	};

private:
	Window_AnimTimeline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Window_AnimTimeline() = default;

public:
	virtual HRESULT		Initialize(void* pArg) override;
	virtual _uint       Update_Contents(_float fTimeDelta) override;

private:
	void Draw_Timeline(_float fTimeDelta); // 선택한 애니메이션 재생 시간/스크럽 등 눈으로 확인하는 용도
	_float ChooseTiemStepSecond(_float fTimePerPixel);
	const _char* Get_NotifyEventName(ANIM_EVENT_TYPE eEventType);

private:
	TimelineViewData m_ViewData;
	
	AnimToolData* m_pAnimToolData = {};
	GameInstance* m_pGameInstance = {};

	//선택,상호작용위한
	//_uint		m_iSelectedNotifyId = g_INVALID;
	//_uint		m_iHoveredNotifyId = g_INVALID;


public:
	static Window_AnimTimeline* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};
NS_END
