#pragma once

#include "AnimationTool_Define.h"
#include "Level.h"
#include "VFX_Parsing.h"

NS_BEGIN(Engine)
class Mouse;
NS_END

NS_BEGIN(AnimationTool)

class Level_Main final : public Level
{
private:
	explicit Level_Main();
	explicit Level_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Level_Main(const Level_Main& original);
	virtual ~Level_Main();

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Player();
	HRESULT Ready_Camera();
	HRESULT Ready_Light();
	HRESULT Ready_Effects();

	Mouse* m_pMouse = {};
	_bool m_btrue = {};

	// 이펙트 파싱용
	VFX_Parsing* m_pParsing_VFX = { nullptr };

public:
	static Level_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;
};
NS_END
