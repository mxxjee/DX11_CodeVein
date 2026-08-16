#pragma once

#include "UITool_Define.h"
#include "Camera.h"

NS_BEGIN(UITool)

class Camera_Free final : public Camera
{
public:
	typedef struct tagCameraFreeDesc : public Camera::CAMERA_DESC
	{
		_float fSensor = {};

	}CAMFREE_DESC;

private:
	explicit Camera_Free();
	explicit Camera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Camera_Free(const Camera_Free& original);
	virtual ~Camera_Free();


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

private:
	_float m_fSensor = {};
	_bool  m_bLock = { true };

public:
	static Camera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
