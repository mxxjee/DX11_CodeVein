#pragma once

/* 보이지는 않고 순수 맵 충돌체 역할만 할 오브젝트 */
#include "MT_Defines.h"
#include "MapObject.h"

class Item_Box final : public MapObject
{
private:
	explicit Item_Box();
	explicit Item_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Item_Box(const Item_Box& original);
	virtual ~Item_Box();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
	virtual ordered_json Get_ExtraData() override;

private:
	HRESULT Ready_Components();

	PxRigidStatic* m_pPhysXActor = { nullptr };
	_uint m_iItemID = 5000;

public:
	static Item_Box* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
