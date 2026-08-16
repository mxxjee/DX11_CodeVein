#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class SampleParent final : public GameObject
{
private:
	explicit SampleParent();
	explicit SampleParent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit SampleParent(const SampleParent& original);
	virtual ~SampleParent();

public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static SampleParent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
