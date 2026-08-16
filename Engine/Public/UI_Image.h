#pragma once
#include "UI_Render.h"

NS_BEGIN(Engine)
class NewTexture;
class VIBuffer_Rect;


class ENGINE_DLL UI_Image :
    public UI_Render
{
public:
	enum RENDER_TYPE{RENDER_NORMAL, RENDER_RESOURCE_ONLY,RENDER_BLUR
		,RENDER_MINIMAP_FULL,
		RENDER_MINIMAP_CENTER,END};
	struct ColorEffect
	{
		/*랜덤으로 변화하는 컬러 효과*/
		bool    m_bUseColorEffect = false;
		_float		m_InitHue =1.f;
		_float		m_MaxHue = 1.f;
		_float      m_MinHue = 1.f;
		_float      m_fColorTime = 0.f;
		_float		m_fColorSpeed = 5.f;

		void        Reset()
		{
			m_fColorTime = 0.f;
		}
	};
public:
	enum class TEXTYPE{DIFFUSE,NOISE,MASK,END};
public:
	typedef struct tagUIImageDesc :public tagUICompDesc
	{
		wstring TextureComponentKey = L"Prototype_Component_UITexture_UIResource";
		string TextureKey = "UI4_Main/White";
		bool	bUseBindExtra = false;

		bool	bUseInfinitScroll = false;
		float	fSpeed = 0.f;

	}UIIMAGEDESC;
protected:
	explicit UI_Image();
	explicit UI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UI_Image(const UI_Image& original);
	virtual ~UI_Image();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	virtual _int	Update(const _float fTimeDelta);

public:
	static UI_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg);


	virtual			HRESULT		Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);

public:
	void		Set_CurrentTexKey(string str) { m_CurrentTexName = str; }
	void		Set_TargetTexIdx(_uint i) { m_iTargetTexNumber = i; }

	void		Change_Texture(string str);
	void		Change_Texture(_uint i);

	void		Set_Color(_float4 vColor) { m_vColor = vColor; }
	string		Get_CurrentTexKey() { return m_CurrentTexName; }
	_uint		Get_CurrentTexIdx() { return m_iTargetTexNumber; }
public:
	NewTexture* Get_ProtoTextureComp() { return m_pTextureCom; }
#ifdef _DEBUG
	virtual void		Render_Imgui();
	
private:
	void			Render_TextureList();

private:
	ImGuiTextFilter         m_HierarchyFilter;//검색창 imgui
#endif // _DEBUG


#pragma region parsing
public:
	virtual void	Save_Data(ordered_json& pJson) override;
	virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion
public:
	void Free() override;

protected:
	NewTexture*			m_pTextureCom = { nullptr };
	VIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	

	wstring			m_ProtoTextureKey = L"";

		/*Diffuse용*/
	_uint				m_iTargetTexNumber = 0;
	string				m_CurrentTexName="";


	/*Noise용*/
	_uint				m_iNoiseTargetTexNumber = 0;
	string				m_NoiseTexName = "";

	/*Mask 용*/
	_uint				m_iMaskTargetTexNumber = 0;
	string				m_MaskTexName = "";

				//DIFFUSE말고 다른것도바인딩할건지
	bool				m_bUseBindExtra = false;


	////바인딩을 위한 추가속성들
private:
	//무한 스크롤 사용할건지 여부.
	bool		m_bUseInfinitScroll = false;
	_float		m_fSpeed = 0.f;
	_float		m_fScrollTime = 0.f;


	//셰이더에 던지는 값
	_float4		m_vColor = _float4(1.f, 1.f, 1.f, 1.f);


	

	/*랜덤으로 변화는 색 효과*/
public:
	void    Set_UseRandomColor(bool b) { m_ColorEffect.m_bUseColorEffect = b; }
	void    Set_MinHue(_float vMinHue) { m_ColorEffect.m_MinHue = vMinHue; }
	void    Set_MaxHue(_float vMaxHue) { m_ColorEffect.m_MaxHue = vMaxHue; }

public:
	_float4		Get_CurrentColor() {return m_vColor;}
	_float4		Get_OriginColor() { return m_vOriginColor; }
private:
	bool    m_bUseRandomColor = false;
	ColorEffect     m_ColorEffect;
	_float3			m_vHsv;
			//읽고나서 초기 컬러값.
	_float4			m_vOriginColor = _float4(1.f, 1.f, 1.f, 1.f);


private:
	RENDER_TYPE			m_eRenderMode = RENDER_TYPE::RENDER_NORMAL;
	bool				m_bAddictiveBlending = false;

private:
	_float2			m_fClip=_float2(0.f,0.f);
	_float			m_fIntensity = 1.f;
	_uint			m_iProtoTexLevel = 0;
};
NS_END

