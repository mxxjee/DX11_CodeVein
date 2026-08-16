#pragma once

#include "Client_Define.h"
#include "Character.h"

NS_BEGIN(Engine)
class Model;
class Shader;
class Collider;
NS_END

NS_BEGIN(Client)

class SampleModel final : public Character
{
private:
	explicit SampleModel();
	explicit SampleModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit SampleModel(const SampleModel& original);
	virtual ~SampleModel();

public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);
	HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override final;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	Collider* m_pColliderCom = { nullptr };

public:
	static SampleModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};

NS_END
