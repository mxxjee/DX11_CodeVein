#pragma once

#include "Client_Define.h"
#include "Level_Client.h"

NS_BEGIN(Client)
class CustomizingManager;
class InteractionManager;
class Parser_UITool;
class Level_Customize final : public Level_Client
{
private:
	explicit Level_Customize();
	explicit Level_Customize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Customize();

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	HRESULT Ready_Load();
	HRESULT Ready_UIObjects();

	HRESULT Ready_Camera();
	HRESULT Ready_Player();
	virtual HRESULT Ready_UIObject() override;
	HRESULT Ready_StaticObject();
	HRESULT Ready_Light();
	HRESULT Ready_Values();

	HRESULT Ready_Sky_Customize();

private:
	void ChangeColor();

private:
	_bool m_bCamera = false;
	_uint				m_iLevelChangeHandle = 0;
	InteractionManager* m_pInteractionManager = nullptr;

public:
	static Level_Customize* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

private:
	class Parser_UITool* m_pParsing_UI = { nullptr };
	CustomizingManager* m_pCustomizingManager = nullptr;
};

NS_END
