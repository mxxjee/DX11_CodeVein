#pragma once

#include "ShaderTool_Define.h"
#include "MapObject.h"

NS_BEGIN(ShaderTool)

class GodRayObject final : public MapObject
{
public:
	typedef struct GodRayObjectDesc : public GameObject::GAMEOBJECT_DESC {
		_uint iInstanceNum = UINT_MAX;
	}STATICOBJ_DESC;

private:
	explicit GodRayObject();
	explicit GodRayObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GodRayObject(const GodRayObject& original);
	virtual ~GodRayObject();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static GodRayObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
