#pragma once
#include "VFXTool_Define.h"

NS_BEGIN(Engine)
class GameInstance;
class ImguiManager;
NS_END


NS_BEGIN(VFXTool)

class MainApp final : public Base
{
private:
	explicit MainApp();
	virtual ~MainApp() = default;

public:
	HRESULT		Initialize();
	HRESULT		Ready_Static();
	HRESULT		Start_Level();
	HRESULT		Create_Windows();			//Imgui window들(Begin/End로묶인 애들)을 생성한다.

	void Update(const _float fTimeDelta);
	void Render(const _float fTimeDelta);

private:
	void Show_FPS(const _float fTimeDelta);

private:
	_uint m_iFPS = {};
	_uint m_iCurrentFPS = {};
	_wstring m_wstrFPS = { L"FPS : 0" };

private:
	class Loader* m_pLoader = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	GameInstance* m_pGameInstance = { nullptr };
	ImguiManager* m_pImGuiManager = { nullptr };

public:
	static MainApp* Create();

public:
	void Free() override final;
};

NS_END