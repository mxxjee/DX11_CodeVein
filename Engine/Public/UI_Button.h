#pragma once
#include "UIComponent.h"

/*Button 상태에 따른 Textureinfo 처리*/
//ClickEnter/ClickExit/HoverEnter/HoverExit/Default에 따른 텍스처,이벤트지정
//UI_Image와 짝을 이뤄 UI_IMage를참조한다.

NS_BEGIN(Engine)
class UI_Image;

class ENGINE_DLL UI_Button :
    public UIComponent
{
public:
	typedef struct tagButtonDesc : UIComponent::UICOMPDESC
	{
		string		DefaultTexKey = DEFAULT_TEXTUREKEY;
		string		HoverTexKey = DEFAULT_TEXTUREKEY;
		string		ClickTexKey = DEFAULT_TEXTUREKEY;
		string		DisableKey = DEFAULT_TEXTUREKEY;

	}BUTTON_DESC;

public:
					//default - 기본텍스처
					//hover - 호버 시 텍스처
					//pressed-클릭시 
					//disabled-사용할수없는 상태일때의 텍스처
					//hover exit시에는 무조건 default 로 돌아가자.
	enum		BUTTONSTATE {DEFAULT,HOVER,PRESSED,DISABLED,END};
protected:
	explicit UI_Button();
	explicit UI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UI_Button(const UI_Button& original);
	virtual ~UI_Button();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	virtual _int	Update(const _float fTimeDelta);
	virtual _int	Update_Late(const _float fTimeDelta);


public:
	static UI_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg);

	//uimanager로 받은 픽킹정보에 의한 버튼 상태값 변경
	void	Change_ButtonState(BUTTONSTATE eState);


	void	Add_Event(BUTTONSTATE eState,UI_MasterEvent& Event);

public:
	void		OnHoverExit();

private:
			//각 정보가 갱신되었을때의 행동 처리
	void		Enter_State(BUTTONSTATE eState);
public:
	void Free() override;

#ifdef _DEBUG
public:
	virtual void		Render_Imgui();
	void				Show_EventList();

	void				Show_ClickEvents();
	void				Show_HoverEvents();
	void				Show_HoverExitEvents();
	void				Add_Event_On_Imgui(BUTTONSTATE eState);

private:
	ImGuiTextFilter         m_HierarchyFilter;//검색창 imgui
#endif // _DEBUG

#pragma region parsing
public:
	virtual void	Save_Data(ordered_json& pJson) override;
	virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion

public:
	string				Get_ClickEventAnimClipName() { return m_ClickAnimClipName; }
	string				Get_HoverEventAnimClipName() { return m_HoverAnimClipName; }
	string				Get_HoverExitAnimClipName() { return m_HoverExitAnimClipName; }

	void		Set_OnClickFunc(function<void()> Func) { OnClickFunc = Func; }
	void		Set_OnHoverFunc(function<void()> Func) { OnHoverFunc = Func; }
	void		Set_OnHoverExitFunc(function<void()> Func) { OnHoverExitFunc = Func; }


private:
	UI_Image*	m_pUIImage = nullptr;
	class NewTexture* m_pProtoTexture = nullptr;		//UI_Image가 쓰고있는 Texcom가져오기

private:
	BUTTONSTATE			 m_eButtonState=BUTTONSTATE::DEFAULT;
	BUTTONSTATE			 m_ePreButtonState = BUTTONSTATE::DEFAULT;

	string				m_TexKeys[_UINT(BUTTONSTATE::END)] = { "" };
	_uint				m_TexIdx[_UINT(BUTTONSTATE::END)] = { 0, };


private:
	vector<UI_MasterEvent>	OnClickEvents;
	vector<UI_MasterEvent>	OnHoverEvents;
	vector<UI_MasterEvent>	HoverExitEvents;

	function<void()>		OnClickFunc = nullptr;
	function<void()>		OnHoverFunc = nullptr;
	function<void()>		OnHoverExitFunc = nullptr;


	string		m_ClickAnimClipName = "";
	string		m_HoverAnimClipName = "";
	string		m_HoverExitAnimClipName = "";

};
NS_END

