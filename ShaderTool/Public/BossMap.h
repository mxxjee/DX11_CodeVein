#pragma once

#include "ShaderTool_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

NS_BEGIN(ShaderTool)

class BossMap final : public GameObject
{
private:
	explicit BossMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit BossMap(const BossMap& original);
	virtual ~BossMap() = default;


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	virtual HRESULT Render(const _float fTimeDelta);
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
public:
	virtual Shader* Get_Shader() { return m_pShaderCom; }
	virtual Model* Get_Model() { return m_pModelCom; }

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static BossMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

private:
	Model* m_pModelCom = {};
	Shader* m_pShaderCom = {};

};

NS_END
