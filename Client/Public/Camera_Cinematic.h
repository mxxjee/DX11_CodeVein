#pragma once

#include "Client_Define.h"
#include "Camera.h"

NS_BEGIN(Client)

class Camera_Cinematic final : public Camera
{
private:
	explicit Camera_Cinematic();
	explicit Camera_Cinematic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Camera_Cinematic(const Camera_Cinematic& original);
	virtual ~Camera_Cinematic();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update(const _float fTimeDelta);
	void Update_Camera(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Camera_Cinematic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};

NS_END
