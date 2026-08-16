#pragma once

#include "ShaderTool_Define.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class Shader;
class Model;
class Transform;
NS_END

NS_BEGIN(ShaderTool)
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
	_vector				Get_RootWorldDelta() const { return m_vWorldDelta; }
	void				Set_RootWorldDelta(_vector vWorldDelta) { m_vWorldDelta = vWorldDelta; }

	void				Set_Animation(_uint iAnimationIndex, _bool bIsLoop = false, _float fLerpDuration = 0.2f, _float fAnimationSpeed = 1.0f); //기존 전신 애니메이션 
	void				Set_AnimationUpper(_uint iAnimationIndex, _bool bUpperBlendEnable, _float fUpperLayerWeight = 1.f, _bool _isLoop = false, _float fLerpDuration = 0.2f, _float fAnimationSpeed = 1.f);//상체 블렌드용 애니메이션
	void				Clear_AnimationUpper();
	void				Request_ClearAnimationUpper(_float fFadeOutTime);

	const _bool			Is_AnimFinished() { return m_pModelCom->Is_AnimFinished(); }
	const _bool			Is_UpperAnimFinished() { return m_pModelCom->Is_UpperAnimFinished(); }

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

private:
	class Player* m_pPlayer = { nullptr };
	_vector			m_vWorldDelta = {};
	Transform* m_pPlayerTransform = { nullptr };

private:
	Shader* m_pShaderCom = { nullptr };
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
NS_END
