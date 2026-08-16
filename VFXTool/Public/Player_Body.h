#pragma once

#include "VFXTool_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(VFXTool)
class Player_Body final : public PartObject
{
public:
	typedef struct tagPlayerBodyDesc : public PartObject::PARTOBJECT_DESC
	{


	}BODY_DESC;

private:
	explicit Player_Body();
	explicit Player_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Body(const Player_Body& original);
	virtual ~Player_Body();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private: 


private:
	Shader* m_pShaderCom = { nullptr };
	Model* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Player_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END