#pragma once

#include "MT_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END


class Player_MasterRig final : public PartObject
{
public:
	typedef struct tagPlayerMasterRigDesc : public PartObject::PARTOBJECT_DESC
	{
		class Player* pPlayer = { nullptr };

	}MASTERRIG_DESC;

private:
	explicit Player_MasterRig();
	explicit Player_MasterRig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_MasterRig(const Player_MasterRig& original);
	virtual ~Player_MasterRig();

public:
	const _float4x4* Get_SocketMatrix(const _char* pBoneName);

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	class Player*	m_pPlayer = { nullptr };
	_vector			m_vWorldDelta = {};

private:
	Shader*			m_pShaderCom = { nullptr };
	Model*			m_pModelCom = { nullptr };
	_int			iAni_test = {};
	

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Player_MasterRig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
