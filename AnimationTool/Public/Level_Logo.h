#pragma once

#include "AnimationTool_Define.h"
#include "Level.h"

NS_BEGIN(AnimationTool)

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

public:
	static Level_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);

	void Initialize_Level() override;

public:
	void Free() override final;

};

NS_END
