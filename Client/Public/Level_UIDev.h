#pragma once

#include "Client_Define.h"
#include "Level.h"
#include "PlayerStatus.h"


NS_BEGIN(Client)
class Parser_UITool;
class Level_UIDev :
    public Level
{
private:
	explicit Level_UIDev();
	explicit Level_UIDev(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_UIDev();

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	HRESULT Ready_Load();
	HRESULT Ready_UIObjects();
	HRESULT Ready_Values();


private:
	class Serializable* m_pParsing_Map = { nullptr };
	class Serializable* m_pParsing_Shader = { nullptr };
public:
	static Level_UIDev* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

private:
	class Parser_UITool* m_pParsing_UI = { nullptr };
	bool	m_bDrawDebug = false;

	//Sample
	PlayerStatus	m_PlayerStatus;

	_uint idx = 0;
	UIObject*		pGuideUI = nullptr;
	bool bActive = true;

};
NS_END
