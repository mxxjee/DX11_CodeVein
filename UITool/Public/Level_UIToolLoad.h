#pragma once

#include "UITool_Define.h"
#include "Level.h"

NS_BEGIN(UITool)
class Level_UIToolLoad :
    public Level
{
private:
	explicit Level_UIToolLoad();
	explicit Level_UIToolLoad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_UIToolLoad();

public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);


private:
	_uint m_iCreateLevel = {};
	LEVEL m_eCreateLevel = { LEVEL::END };
	class UIToolLoader* m_pLoader = { nullptr };

public:
	static Level_UIToolLoad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	void Initialize_Level();


public:
	void Free() override final;


};

NS_END

