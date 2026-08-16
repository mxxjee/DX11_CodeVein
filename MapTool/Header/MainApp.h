#pragma once
#include "MT_Defines.h"

NS_BEGIN(Engine)
class GameInstance;
class ImguiManager;
class Mouse;
NS_END

class CMainApp final : public Base
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Update(const _float fDT);
	void Render(const _float fDT);

private:
	void Show_FPS(const _float fTimeDelta);
	HRESULT Create_Window();
	HRESULT Loading_ToolSource();

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	GameInstance* m_pGameInstance = { nullptr };
	ImguiManager* m_pImGuiMgr = { nullptr };
	Mouse* m_pMouse = { nullptr };
	class CNavigation_UI* m_pNavUI = nullptr;

public:
	_uint m_iFPS = {};
	_uint m_iCurrentFPS = {};
	_wstring m_wstrFPS = { L"FPS : 0" };

	static CMainApp* Create();

public:
	void Free() override final;
};
