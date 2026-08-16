#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Prototype_Class final : public Base
{
private:
	explicit Prototype_Class();
	explicit Prototype_Class(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Prototype_Class(const Prototype_Class& original);
	virtual ~Prototype_Class();

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* arg);
	_int	Update_Priority(const _float fTimeDelta);
	_int	Update(const _float fTimeDelta);
	_int	Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static Prototype_Class* Create();
	static Prototype_Class* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Prototype_Class* Clone(void* arg);

public:
	void Free() override final;

};

NS_END
