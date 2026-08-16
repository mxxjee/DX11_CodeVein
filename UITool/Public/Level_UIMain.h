#pragma once

#include "UITool_Define.h"
#include "Level.h"

NS_BEGIN(Engine)
class UIObject;
class Serializable;
NS_END
NS_BEGIN(UITool)
class Level_UIMain final : public Level
{
private:
	explicit Level_UIMain();
	explicit Level_UIMain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_UIMain();


public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Light();

public:
	static Level_UIMain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

private:
	UIObject* pGuideUI = nullptr;
	Serializable* m_pParsing_UI = { nullptr };

};

NS_END

