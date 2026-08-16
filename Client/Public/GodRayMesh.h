#pragma once

#include "Client_Define.h"
#include "MapObject.h"

NS_BEGIN(Client)

class GodRayMesh final : public MapObject
{
public:
	typedef struct GodRayMeshDesc : public GameObject::GAMEOBJECT_DESC {
		_uint iInstanceNum = UINT_MAX;
	}STATICOBJ_DESC;

private:
	explicit GodRayMesh();
	explicit GodRayMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit GodRayMesh(const GodRayMesh& original);
	virtual ~GodRayMesh();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void BillBoard();

public:
	static GodRayMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

private:
	_float m_fAccTime = {};

};

NS_END
