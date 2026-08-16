#pragma once

#include "ShaderTool_Define.h"
#include "Camera.h"

NS_BEGIN(ShaderTool)

class CameraFree final : public Camera
{
public:
	typedef struct tagCameraFreeDesc : public Camera::CAMERA_DESC
	{
		_float fSensor = {};
	}CAMFREE_DESC;

private:
	explicit CameraFree();
	explicit CameraFree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CameraFree(const CameraFree& original);
	virtual ~CameraFree();


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

private:
	_float m_fSensor = {};
	_bool  m_bLock = { true };

public:
	static CameraFree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
