#pragma once

#include "AnimationTool_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(AnimationTool)
class Player_Hand final : public PartObject
{
public:
	typedef struct tagPlayerHandDesc : public PartObject::PARTOBJECT_DESC
	{

	}HAND_DESC;

private:
	explicit Player_Hand();
	explicit Player_Hand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Hand(const Player_Hand& original);
	virtual ~Player_Hand();

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
	static Player_Hand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END