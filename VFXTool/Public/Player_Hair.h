#pragma once

#include "VFXTool_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(VFXTool)
class Player_Hair final : public PartObject
{
public:
	typedef struct tagPlayerHairDesc : public PartObject::PARTOBJECT_DESC
	{

	}HAIR_DESC;

private:
	explicit Player_Hair();
	explicit Player_Hair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Hair(const Player_Hair& original);
	virtual ~Player_Hair();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	Shader* m_pShaderCom = { nullptr };
	Model* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Player_Hair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END