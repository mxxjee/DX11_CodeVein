#pragma once

#include "VFXTool_Define.h"
#include "Level.h"

NS_BEGIN(Engine)
class ImguiManager;
NS_END

NS_BEGIN(VFXTool)

class Level_Main final : public Level
{
private:
	explicit Level_Main();
	explicit Level_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Main() = default;

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	HRESULT Ready_Player();
	HRESULT Ready_Camera();
	HRESULT Ready_Light();
	HRESULT Ready_ImGui();

private:
	ImguiManager*		m_pImguiManager = { nullptr };
	class VFX_Parsing*	m_pParser = { nullptr };

private:
	_float4 m_vTest{};

public:
	static Level_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	void Initialize_Level() override;

public:
	void Free() override final;


	
};

NS_END
