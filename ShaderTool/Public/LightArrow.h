#pragma once

#include "ShaderTool_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class VIBuffer_Rect;
class Shader;
class Texture;
NS_END

NS_BEGIN(ShaderTool)

class LightArrow final : public GameObject
{
private:
	explicit LightArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit LightArrow(const LightArrow& original);
	virtual ~LightArrow() = default;


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
	static LightArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

private:
	VIBuffer_Rect* m_pVIBuffer = {};
	Shader* m_pShaderCom = {};
	Texture* m_pTexture = {};

};

NS_END
