#pragma once

#include "Client_Define.h"
#include "Character.h"

NS_BEGIN(Client)

class Monster_Test final : public Character
{
private:
	explicit Monster_Test();
	explicit Monster_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Monster_Test(const Monster_Test& original);
	virtual ~Monster_Test();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update_Priority(const _float fTimeDelta);
	_int Update_Parallel(const _float fTimeDelta) override final;
	_int Update(const _float fTimeDelta);
	_int Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

private:
	void OnDamaged(const DAMAGE_EVENT& _damageEvent);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_PhysXEvent();

private:
	_float m_fHP = { 100.f };			// 체력
	_float m_fMaxHP = { 100.f };		// 최대체력
	_bool m_bIsHit = { false };			// 히트 판정
	_uint m_iDamageEventHandle = {};	// 이벤트 핸들값

public:
	static Monster_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
