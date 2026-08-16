#pragma once

#include "GameObject.h"


/* 플레이어가 닿았을때 이벤트를 발생시킬 오브젝트(기본적으로 눈에 안 보임) */
/* 일단 당장 생각난건 몬스터 소환인데 더 있을듯 NPC 상점창 열기? */
NS_BEGIN(Engine)

class ENGINE_DLL EventShape abstract : public GameObject
{
public:
	typedef struct EventShapeDescription : public GameObject::GAMEOBJECT_DESC {
		
		_float3 vTargetPos = {};

	}EVENT_SHAPE_DESC;

protected:
	explicit EventShape();
	explicit EventShape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit EventShape(const EventShape& original);
	virtual ~EventShape();

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* arg);
	_int	Update_Priority(const _float fTimeDelta);
	_int	Update(const _float fTimeDelta);
	_int	Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

	virtual HRESULT Ready_Components() { return S_OK; }


protected:
	_float3 m_vTargetPos = {};	// 목표 지점


	class Collider* m_pColliderCom = { nullptr };


public:
	static EventShape* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual EventShape* Clone(void* arg);

public:
	void Free() override;

};

NS_END
