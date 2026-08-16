#pragma once

#include "MT_Defines.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END


class Player_Head final : public PartObject
{
public:
	typedef struct tagPlayerHeadDesc : public PartObject::PARTOBJECT_DESC
	{

	}HEAD_DESC;

private:
	explicit Player_Head();
	explicit Player_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Head(const Player_Head& original);
	virtual ~Player_Head();

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
	static Player_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
