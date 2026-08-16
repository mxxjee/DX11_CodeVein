#pragma once
#include "Component.h"



NS_BEGIN(Engine)
class UIObject;
class Shader;
//UI가 소유하는 컴포넌트들의 부모/각 컴포넌트들의 Shader_Bind를 위해 만들어진 클래스
//이걸 단독으로 생성할일은없다. //
class ENGINE_DLL UIComponent  abstract:
    public Component
{
public:
	typedef struct tagUICompDesc
	{
		UIObject* pOwner = nullptr;

	}UICOMPDESC;

protected:
	explicit UIComponent();
	explicit UIComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UIComponent(const UIComponent& original);
	virtual ~UIComponent();

public:
	HRESULT				Initialize(void* arg) override;
	bool				Is_BindAble() { return m_bAbleToBind; }
	virtual HRESULT		Bind_Resource(class Shader* pShader, UI_BUFFER_PACKET* pPacket) { return S_OK; }


public:
	/*Event실행 시 호출하는함수 - 재정의 필요*/
	virtual void		Execute(const string& strAction, void* pData) {}

#ifdef _DEBUG
	static void			Set_DrawDebug(bool b) { m_bDrawDebug=b; }
	virtual vector<string>		Get_ActionNames();
#endif // _DEBUG

#pragma region parsing
public:
	virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType);

#pragma endregion

public:
	void Free() override;

protected:
	virtual			HRESULT		Render_UI(_uint ShaderPassNumber,UI_BUFFER_PACKET* pPacket) { return S_OK; }


protected:
	/*이 컴포넌트의 소유자*/
	UIObject* m_pOwner = { nullptr };
	Shader* m_pShaderCom = { nullptr };
	bool		m_bAbleToBind = true;

	static bool			m_bDrawDebug;


protected:
	/*이벤트 관련*/
	//키 : 이벤트이름, value : 이벤트에 맞는 enum값 등록
	unordered_map<string, _uint>		m_Actions;



};

NS_END

