#pragma once

#include "Client_Define.h"

NS_BEGIN(Engine)
class GameInstance;
class Mouse;
class ImguiManager;
class Shader;
class GameObject;
class PartObject;
NS_END

NS_BEGIN(Client)
class InventoryManager;
class MinimapManager;
class InteractionManager;
class ShopManager;



class UIObj_Cursor;
class Parser_UITool;

class MainApp final : public Base
{
private:
	explicit MainApp();
	virtual ~MainApp();

public:
	HRESULT Initialize();
	HRESULT Ready_Loading();
	HRESULT Ready_Static();
	HRESULT Start_Level();
	HRESULT Ready_IMGUI();
	HRESULT Ready_Light();

	HRESULT		Ready_Persistent();

	void Update(const _float fTimeDelta);
	void Render(const _float fTimeDelta);

private:
	void Show_FPS(const _float fTimeDelta);
	HRESULT Register_HotKey();
	void UI_Inventory_OnOff();
	void Bind_Player_Shader();
	void Find_Player(const _float fTimeDelta);
	

private:
	_uint m_iFPS = {};
	_uint m_iCurrentFPS = {};
	_wstring m_wstrFPS = {L"FPS : 0"};

	class Loader* m_pLoader = { nullptr };
	class Loader* m_pLoader_Effect = { nullptr };
	Mouse* m_pMouse = { nullptr };
	ImguiManager* m_pImguiManager = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	GameInstance* m_pGameInstance = { nullptr };

private:
	UIObj_Cursor* m_pCursor = nullptr;
	bool		m_bLockEsc = false;
	_uint		LockHandle = 0;


private:
	/*Client Manager*/
	InventoryManager*		m_pInventoryManager = nullptr;
	MinimapManager*			m_pMinimapManager = nullptr;
	InteractionManager*		m_pInteractionManager = nullptr;
	ShopManager*			m_pShopManager = nullptr;

	//플레이어 정보 여기서 한번에 던지려고
	Shader* m_pShaderCom = nullptr;
	class Player* m_pPlayer = nullptr;
	PartObject* m_pPlayerPart = nullptr;
	vector<CINEMATIC_KEYFRAME> m_vecWolfSceneKeyFrames = {};

private:
	Parser_UITool*			m_pParsing_UI = nullptr;
public:
	static MainApp* Create();

public:
	void Free() override final;

};

NS_END

