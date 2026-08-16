#pragma once
#include "MT_Defines.h"
#include "ContainerObject.h"

class Player : public ContainerObject
{
public:
	typedef struct tagPlayerDesc : public GameObject::GAMEOBJECT_DESC
	{
		PHYSX_ACTOR_DESC tActorDesc;
		PHYSX_CONTROLLER_DESC tControllerDesc;
	}PLAYER_DESC;

private:
	explicit Player();
	explicit Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player(const Player& original);
	virtual ~Player();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int    Update_Parallel(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

	void		Set_CanTurn(bool bTurn) { m_bTurn = bTurn; }
private:
	class Collider* m_pColliderCom = { nullptr };
	class Player_MasterRig* m_pMasterRig = { nullptr };
	_float m_fSpeed = 0.f;

	physx::PxController* m_pController = nullptr;
	_float               m_fGravity      = 0.f;      // 누적될 중력 값
	_bool                m_bIsGrounded   = false;
	_bool				 m_bEnablePhysics = false;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	static Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

private:
	bool		m_bTurn = true;
};
