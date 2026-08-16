#pragma once
#include "UIComponent.h"

NS_BEGIN(Engine)
class UI_Image;



class ENGINE_DLL UI_Progress :
    public UIComponent
{
public:
	enum class ProgressType{ HORIZONTAL,HORIZONTAL_RE,VERTICAL,VERTICAL_RE,RADIAL,CENTER,END};
	
	/*처리할 이벤트타입*/
	enum class PROGRESS_ACTION{
		SET_MAX,//최대값 설정하기
		SET_HP,//바로 curren설정하기 
		END};
public:
	typedef struct UIProgressDesc:public UIComponent::UICOMPDESC
	{
		_bool			bLerpReach = false;		//서서히 도달할건지, 바로도달할건지.
		_float			fSpeed = 5.f;
		ProgressType	m_eProgressType = ProgressType::HORIZONTAL;
		string			 TextureKey = "UI4_Main/White";

		_float*		fCurrent = nullptr;//포인터 참조로 변경
		_float*		fMax = nullptr;//포인터 참조로 변경
	}UI_PROGRESSDESC;
protected:
	explicit UI_Progress();
	explicit UI_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UI_Progress(const UI_Progress& original);
	virtual ~UI_Progress();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	virtual _int	Update(const _float fTimeDelta);
	virtual _int	Update_Late(const _float fTimeDelta);

public:
	virtual HRESULT		Bind_Resource(class Shader* pShader, UI_BUFFER_PACKET* pPacket);
public:
	static UI_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg);
public:
	void Free() override;


public:
	/*Event실행 시 호출하는함수 - 재정의 필요*/
	virtual void		Execute(const string& strAction, void* pData);




#ifdef _DEBUG
public:
	virtual void		Render_Imgui();

private:
	ImGuiTextFilter         m_HierarchyFilter;//검색창 imgui
	_float					m_PreviewCurrent=1.f;
	_float					m_PreviewMax=1.f;
	bool					m_bIsBound = false;
#endif // _DEBUG

public:
	/*컴포넌트 생성이후 세팅함수*/
	//프로그레스 나눌값( ex) 몬스터전체HP)
	void		Set_Max(_float* fMax) { m_fMax = fMax; }
	
	//프로그래스 현재값( ex) 몬스터 현재 HP)
	void		Set_Current(_float* fCurrent) { m_fCurrent = fCurrent; }

	//서서히 도달할건지 바로도달할건지 여부
	void		Set_LerpReach(bool b) { m_bLerpReach = b; }

	void		Compute_Ratio();
	void		Set_PreRatio(_float f) { m_fPreRatio = f; }
	void		Set_Ratio(_float f) { m_fRatio = f; }
	void		Set_TargetRatio(_float f) { m_fTargetRatio = f; }
	void		Set_RenderRato(_float f) { m_fRenderRatio = f; }

	void		Reset();

public:
	float		Get_TargetRatio() {return m_fTargetRatio;}
	float		Get_RenderRatio() { return m_fRenderRatio; }
	float		Get_GetClipX() { return g_fClipX; }
#pragma region parsing
public:
	virtual void	Save_Data(ordered_json& pJson) override;
	virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion

#pragma endregion

private:
	UI_Image* m_pUIImage = nullptr;
	class NewTexture* m_pProtoTexture = nullptr;		//UI_Image가 쓰고있는 Texcom가져오기


private:
	_float			m_fRatio = 0.f;
	_float			m_fPreRatio = 0.f;

	_float*			m_fMax = nullptr; // 1.f;
	_float*			m_fCurrent = nullptr;// 0.f;
	_float			m_fTargetRatio = 1.f;		//최종목표 비율
	_float			m_fRenderRatio = 0.f;		//실제 셰이더에 던질값(디버그에서는 테스트용도)
	_float			m_fSpeed = 5.f;

	_bool			m_bLerpReach = false;		//서서히 도달할건지, 바로도달할건지.
	
	_bool			m_bDelayCompute = false;	//바로계산할건지, 일부시간 이후에 계싼할건지
	_float			m_fDelayTime = 0.f;



	_float			m_fCurrentDelayTime = 0.f;
	bool			m_bStartDelay = false;

	_float			g_fClipX = 0.f;
private:
	string		m_FillTextureKey = "";
	_uint		m_TargetIdx = 0;
	ProgressType m_eType = ProgressType::HORIZONTAL;

#ifdef _DEBUG
	const char* ProgressTypestr[_UINT(ProgressType::END)] = {"Horizontal","Horizontal_Reverse","Vertical","Verticla_Reverse","Radial","Center"};
#endif // _DEBUG

};
NS_END
