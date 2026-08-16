#pragma once

#include "Client_Define.h"
#include "Camera.h"

class CCamera_Object final : public Camera
{
public:
	enum class CAMERAMODE {FOLLOW,LOCK,END};
	struct CameraStateEvent
	{
		CAMERAMODE eMode;
	};
	typedef struct tagCameraObjDesc : public Camera::CAMERA_DESC
	{
		_float fSensor = {};
	}CAMOBJ_DESC;

private:
	explicit CCamera_Object();
	explicit CCamera_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Object(const CCamera_Object& original);
	virtual ~CCamera_Object();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
    void Update_Camera(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	// getter/setter
	void Set_CameraPosition(_fvector vPos)
	{
		if (nullptr != m_pTransformCom)
		{
			m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);
		}
	}
	void Set_Target(GameObject* pTarget) { m_pTarget = pTarget; }

public:
	//Get함수
	virtual _float      Get_Yaw() { return m_fRotationY; }

private:
	_float m_fSensor = {};
	GameObject* m_pTarget = nullptr;
	//_float3     m_vOffset = _float3(0.f, 5.f, -10.f);

	//_float      m_fTargetDistance = 10.0f;
	//_float      m_fAngleX = 0.0f;
	//_float      m_fAngleY = 0.0f;

	//_float      m_fMouseSensitivity = 0.1f;


	_float m_fDistance = {}; //타겟과의 거리
	//카메라 마우스 X,Y축 제어
	_float m_fRotationY = {};
	_float m_fRotationX = {};
	_float m_fMinRotationX = {};
	_float m_fMaxRotationX = {};
public:
	static CCamera_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

private:
	CAMERAMODE		m_eCameraMode = CAMERAMODE::FOLLOW;

};

