#pragma once

#include "Client_Define.h"
#include "Projectile.h"

NS_BEGIN(Client)

class Projectile_Direct final : public Projectile
{
private:
	explicit Projectile_Direct();
	explicit Projectile_Direct(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Projectile_Direct(const Projectile_Direct& original);
	virtual ~Projectile_Direct();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	void OnSpawn(void* _arg) override;
	void OnDespawn() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:

public:
	static Projectile_Direct* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
