#pragma once
#include "UIComponent.h"

/*UI는 그룹화를 하기 위해 부모가 필요하므로,
부모는 transform만 갖는 깡통일수도있고,
image를갖는 오브젝트 일 수 도 있으므로 상속으로 추가 구현이 필요*/

//UI의 렌더를 담당하는 컴포넌트

NS_BEGIN(Engine)
class Transform;
class Shader;

class ENGINE_DLL UI_Render :
    public UIComponent
{
protected:
	explicit UI_Render();
	explicit UI_Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UI_Render(const UI_Render& original);
	virtual ~UI_Render();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;


public:
	virtual			HRESULT		Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);

public:
	static UI_Render* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* arg);

public:
	_bool	Is_Interactable() { return m_bInteractable; }
	void	Set_Interactable(bool bInteractable) { m_bInteractable = bInteractable; }
public:
	void Free() override;


protected:
	/*부모 transform 참조*/
	Transform*			m_pTransformCom = { nullptr };

	/*상호작용 여부
	TRUE : 마우스 인식 O
	FALSE : 마우스 인식X (픽킹에서 제외)*/
	bool				m_bInteractable = true;
	_float m_fWindowX = 0.f;
	_float m_fWindowY = 0.f;


#pragma region parsing
public:
	virtual void	Save_Data(ordered_json& pJson) override;
	virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion

#ifdef _DEBUG
public:
	virtual void		Render_Imgui();
protected:
	ID3D11Buffer* m_pDebugVB = nullptr;
	class Shader* m_pLineShader = nullptr;

	_float4x4 m_matViewMatrix = {};
	_float4x4 m_matProjectionMatrix = {};


	VTXPOSCOR vertices[5];
public:
	virtual void		Render_Debug();

#endif // _DEBUG

};
NS_END
