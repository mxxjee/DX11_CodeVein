#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class VIBuffer_SkySphere;
class Shader;
class Texture;
NS_END

NS_BEGIN(Client)

class GodRaySun final : public GameObject
{
public:
	typedef struct tagGodRayDesc : public GAMEOBJECT_DESC
	{
		_float4 vSunPos;
	}GodRayDesc;

private:
	explicit GodRaySun();
	explicit GodRaySun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GodRaySun(const GodRaySun& original);
	virtual ~GodRaySun();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);
	virtual Shader* Get_Shader() { return m_pShaderCom; }
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	VIBuffer_SkySphere* m_pVIBufferCom = { nullptr };
	Shader* m_pShaderCom = { nullptr };
	Texture* m_pTextureCom = { nullptr };

public:
	static GodRaySun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
