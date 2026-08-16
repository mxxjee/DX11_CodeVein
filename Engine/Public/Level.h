#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL Level abstract : public Base
{
protected:
	explicit Level();
	explicit Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Level();

public:
	virtual HRESULT Initialize();
	virtual _int	Update_Priority(const _float fTimeDelta);
	virtual _int	Update(const _float fTimeDelta);
	virtual _int	Update_Late(const _float fTimeDelta);
	virtual HRESULT Render(const _float fTimeDelta);

	_bool IsClearLevel() const { return m_bIsClearLevel; }

protected:
	void Set_Level(_uint _level) { m_iLevel = _level; }

	_bool m_bIsClearLevel = { true };
	class GameInstance* m_pGameInstance = { nullptr };
	_uint m_iLevel = {};
	_wstring m_wstrName = {};

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static Level* Create();
	static Level* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Initialize_Level() = 0;

public:
	void Free() override;

};

NS_END
