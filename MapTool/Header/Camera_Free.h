#pragma once

#include "MT_Defines.h"
#include "Camera.h"


class CCamera_Free final : public Camera
{
public:
	typedef struct tagCameraFreeDesc : public Camera::CAMERA_DESC
	{
		_float fSensor = {};
	}CAMFREE_DESC;

private:
	explicit CCamera_Free();
	explicit CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Free(const CCamera_Free& original);
	virtual ~CCamera_Free();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	void Update_Camera(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	void Show_Pos();
	void Move(const _float fTimeDelta);
	void Lock_Mouse();

	void LookAt(const _fvector _targetposition) override final;
	void SyncYawPitchFromLook();
	void Set_CameraPosition(_fvector vPos)
	{
		if (nullptr != m_pTransformCom)
		{
			m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);
		}
	}

private:
	_float m_fSensor = {};
	_bool  m_bLock = { true };
	Collider* m_pColliderCom = { nullptr };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};





