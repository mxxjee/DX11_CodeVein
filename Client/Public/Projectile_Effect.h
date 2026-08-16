#pragma once

#include "Client_Define.h"
#include "Projectile.h"

NS_BEGIN(Client)

class Projectile_Effect final : public Projectile
{
private:
	explicit Projectile_Effect();
	explicit Projectile_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Projectile_Effect(const Projectile_Effect& original);
	virtual ~Projectile_Effect();


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
	void Move_Up(const _float fTimeDelta);

private:
	_float m_fCurrentAngle = {};	// 기준 각도(m_vDir)기준 XZ평면이 얼마만큼 회전했는지

public:
	static Projectile_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
