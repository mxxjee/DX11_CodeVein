#pragma once

/* 보이지는 않고 순수 맵 충돌체 역할만 할 오브젝트 */
#include "Client_Define.h"
#include "MapObject.h"
#include "IInteractable.h"

NS_BEGIN(Client)

class Item_Box final : public MapObject, IInteractable
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
	Model* m_pCoverModelCom = nullptr;

	PxRigidStatic* m_pPhysXActor = { nullptr };
	_uint m_iItemID = 5000;

	// 뚜껑용
	_matrix m_matCoverLocal = XMMatrixIdentity();
	_float  m_fOpenAngle = 0.f;      // 현재 열린 각도
	_bool   m_bIsOpened = false;     // 열림 상태 플래그
	ITEM_TRIGGER m_eItemTrigger = ITEM_TRIGGER::NONE;

#pragma region IInteractable
public:
	virtual bool		IsInteractable();	//상호작용가능한지 물어보는 함수
	virtual _float		Get_InteractionRange() { return 3.f; }         //
	virtual void		Enter_InteractionRange(GameObject* pPlayer);		//상호작용 가능 범위에들어왔을때 호출되는함수
	virtual void		Stay_InteractionRange(GameObject* pPlayer, const _float& fTimeDelta);
	virtual void		Exit_InteractionRange(GameObject* pPlayer);		//범위에 들어왔다가 나갔을때 호출되는 함수

	virtual void		Enter_Interaction(GameObject* pPlayer);		//키를 눌러서 실제로 인터렉션실행(대부분 여기서 진짜 상호작용 행동을 정의)
	virtual void		Stay_Interaction(GameObject* pPlayer, const _float& fTimeDelta);
	virtual void		Exit_Interaction(GameObject* pPlayer);			//객체가 판단하여 알아서 exit_Interaction()을 수행해야한다.


	virtual _vector		Get_Position() { return GameObject::Get_Position(); }
#pragma endregion

public:
	static Item_Box* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
NS_END