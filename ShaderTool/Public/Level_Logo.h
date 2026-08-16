#pragma once

#include "ShaderTool_Define.h"
#include "Level.h"

NS_BEGIN(Engine)
class Serializable;
class ImguiManager;
class ImguiWindow;
NS_END

NS_BEGIN(ShaderTool)

class Level_Logo final : public Level
{
private:
	explicit Level_Logo();
	explicit Level_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Level_Logo(const Level_Logo& original);
	virtual ~Level_Logo();

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_UI();
	HRESULT Ready_Camera();
	HRESULT Ready_Player();
	HRESULT Ready_Light();
	HRESULT Ready_Terrain();
	HRESULT Ready_Option();
	HRESULT Ready_Map();
	HRESULT Ready_GodRay();
	HRESULT Spawn_SavePoint(LEVEL _level);
	HRESULT Ready_Sky();
	HRESULT Ready_Monster();
	void ChangeChurchBoss();


public:
	static Level_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

private:
	class Serializable* m_pParsing_Shader = { nullptr };
	class Serializable* m_pParsing_Pos = { nullptr };
	_bool m_bMapCreate;
};

NS_END
