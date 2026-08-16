#pragma once

#include "VFXTool_Define.h"
#include "Level.h"

NS_BEGIN(VFXTool)

class Level_Load final : public Level
{
private:
	explicit Level_Load();
	explicit Level_Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level_Load();


public:
	HRESULT Initialize(LEVEL _level);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	_uint m_iCreateLevel = {};
	LEVEL m_eCreateLevel = { LEVEL::END };
	class Loader* m_pLoader = { nullptr };


public:
	static Level_Load* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	void Initialize_Level() override;


public:
	void Free() override final;
};

NS_END
