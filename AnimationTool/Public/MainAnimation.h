#pragma once
#include "Client_Macro.h"


NS_BEGIN(Engine)
class GameInstance;
class ImguiManager;
class Mouse;
NS_END

NS_BEGIN(AnimationTool)
class MainAnimation : public Base
{
private:
	explicit MainAnimation();
	virtual ~MainAnimation() = default;

public:
	HRESULT		Initialize();
	HRESULT		Create_Windows();			//Imgui window들(Begin/End로묶인 애들)을 생성한다.

	HRESULT Ready_Static();
	HRESULT Start_Level();

	void Update(const _float fTimeDelta);
	void Render(const _float fTimeDelta);

private:
	void Show_FPS(const _float fTimeDelta);


private:
	_uint m_iFPS = {};
	_uint m_iCurrentFPS = {};
	_wstring m_wstrFPS = { L"FPS : 0" };

	class Loader* m_pLoader = { nullptr };
	Mouse* m_pMouse = { nullptr };


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	GameInstance*			m_pGameInstance = { nullptr };
	ImguiManager*			m_pImGuiManager = { nullptr };
public:
	static MainAnimation* Create();

public:
	void Free() override final;
};
NS_END
