#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class VIBuffer_SkySphere;
class Shader;
class Texture;
NS_END

NS_BEGIN(Client)

class Sky_Sphere final : public GameObject
{
private:
	explicit Sky_Sphere();
	explicit Sky_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sky_Sphere(const Sky_Sphere& original);
	virtual ~Sky_Sphere();


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

private:
	VIBuffer_SkySphere* m_pVIBufferCom = { nullptr };
	Shader* m_pShaderCom = { nullptr };
	Texture* m_pTextureCom = { nullptr };
	_uint m_iSkyTextureNum = {};

public:
	static Sky_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
