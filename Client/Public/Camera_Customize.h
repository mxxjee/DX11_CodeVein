#pragma once

#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class Camera_Customize final : public Camera
{
public:
	typedef struct tagCameraFreeDesc : public Camera::CAMERA_DESC
	{
		_float fSensor = {};
	}CAMFREE_DESC;

private:
	explicit Camera_Customize();
	explicit Camera_Customize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Camera_Customize(const Camera_Customize& original);
	virtual ~Camera_Customize();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	void Update_Camera(const _float fTimeDelta) override final;
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	void Move_CustomizeCam_Y(const _float fTimeDelta);
	void Move_CustomizeCam_X(const _float fTimeDelta);

	void Process_OrbitInput(const _float fTimeDelta);
	void Compute_Orbit();

private:
	_float m_fSensor = {};

	/* 카메라가 바라볼 원점 */
	_float4 m_vPivot = {};
	_float4 m_vOrigin = {};	// 플레이어 원점

	/* 상하좌우 이동 가속 */
	_float m_fMaxSpeed = {};
	_float m_fCurrentXSpeed = {};
	_float m_fCurrentYSpeed = {};
	_float m_fAccel = {};
	_float m_fDecel = {};
	_float m_fOffsetX = {};
	_float m_fOffsetY = {};
	_float m_fMaxOffsetX = {};
	_float m_fMaxOffsetY = {};

	/* 확대, 축소, 회전 */
	_float m_fZoomSpeed = {};
	
	_float m_fOrbitDistance = {};
	_float	m_fTargetDistance = {};


	_float m_fMinDistance = {};
	_float m_fMaxDistance = {};


public:
	static Camera_Customize* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
