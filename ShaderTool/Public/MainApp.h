#pragma once

#include "ShaderTool_Define.h"

NS_BEGIN(Engine)
class GameInstance;
class ImguiManager;
class Mouse;
class Shader;
NS_END

NS_BEGIN(ShaderTool)

class MainApp final : public Base
{
private:
	explicit MainApp();
	virtual ~MainApp();

public:
	HRESULT Initialize();
	HRESULT Ready_Static();
	HRESULT Start_Level();
	HRESULT	Create_Windows();
	HRESULT Create_RTVWindows();

	void Update(const _float fTimeDelta);
	void Render(const _float fTimeDelta);

private:
	void Show_FPS(const _float fTimeDelta);

private:
	_uint m_iFPS = {};
	_uint m_iCurrentFPS = {};
	_wstring m_wstrFPS = {L"FPS : 0"};

	class Loader* m_pLoader = { nullptr };
	Mouse* m_pMouse = { nullptr };

	//플레이어 정보 여기서 한번에 던지려고
	Shader* m_pShaderCom;

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
