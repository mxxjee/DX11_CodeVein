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
class Window_AnimList final : public ImguiWindow
{
private:
	Window_AnimList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Window_AnimList() = default;

public:
	virtual HRESULT		Initialize(void* pArg) override;
	virtual _uint       Update_Contents(_float fTimeDelta) override;

	void Draw_AnimationTab(_float fTimeDelta);

	void Draw_SelectObjectTab();


	AnimToolData* Get_AnimToolData() { return &m_AnimToolData; }

private:
	HRESULT Ready_Player(_float fTimeDelta);
	HRESULT Ready_Character(_float fTimeDelta);
	void Draw_LeftAnimListPannel(); //왼쪽 애니메이션 리스트 패널보여주기
	void Draw_Prototypes();			// 프로토타입 리스트 띄우기
	void Change_Character();		// 캐릭터 바꾸기

	void Select_Object_Tab();		// 선택 오브젝트 정보 보여주는 탭

	// Transform 컴포넌트별 ImGui 위젯
	_bool Render_TransformField(const char* _label, const char* _inputID, const char* _dragID,
		_float* _values, _float4& _cached, _float _dragSpeed, _float _dragMin, _float _dragMax);


private:
	AnimToolData m_AnimToolData = {};
	GameInstance* m_pGameInstance = {};

	UMAP<_wstring, class GameObject*> m_pPrototypes;	// 표시할 프로토타입들
	GameObject* m_pSelectedPrototype;					// 선택된 프로토타입
	_wstring m_wstrSelectedPrototype;					// 선택된 프로토타입 이름
	_bool m_bShowPrototypes = { true };					// 프로토타입 리스트 ON, OFF

	static constexpr _float TRANSFORM_EPSILON = 0.0001f;

	_float4 m_vCurrentScale = {};
	_float4 m_vCurrentRotation = {};
	_float4 m_vCurrentPos = {};
	

	

public:
	static Window_AnimList* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

NS_END