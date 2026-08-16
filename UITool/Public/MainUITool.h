#pragma once


NS_BEGIN(Engine)
class GameInstance;
class ImguiManager;
class Mouse;

NS_END


NS_BEGIN(UITool)
class CopyManager;
class MainUITool final:
    public Base
{
private:
	explicit MainUITool();
	virtual ~MainUITool() = default;

public:
	HRESULT		Initialize();
	HRESULT		Create_Windows();			//Imgui window들(Begin/End로묶인 애들)을 생성한다.

	void Update(const _float fTimeDelta);
	void Render(const _float fTimeDelta);

private:
	void Show_FPS(const _float fTimeDelta);

public:
	HRESULT Ready_Static();
	HRESULT Start_Level();

private:
	_uint m_iFPS = {};
	_uint m_iCurrentFPS = {};
	_wstring m_wstrFPS = { L"FPS : 0" };


	class UIToolLoader* m_pLoader = { nullptr };
	Mouse* m_pMouse = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	GameInstance* m_pGameInstance = { nullptr };
	ImguiManager* m_pImGuiManager = { nullptr };
	CopyManager* m_pCopyManager = { nullptr };

public:
	static MainUITool* Create();

public:
	void Free() override final;
};

NS_END