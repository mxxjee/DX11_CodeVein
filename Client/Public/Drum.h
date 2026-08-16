#pragma once

/* 보이지는 않고 순수 맵 충돌체 역할만 할 오브젝트 */
#include "Client_Define.h"
#include "MapObject.h"

NS_BEGIN(Client)

class Drum final : public MapObject
{
private:
	explicit Drum();
	explicit Drum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Drum(const Drum& original);
	virtual ~Drum();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Drum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

	PxRigidStatic* m_pPhysXActor = { nullptr };
	class ParticleSystem* m_pEffect = nullptr;

public:
	void Free() override final;

};

NS_END
