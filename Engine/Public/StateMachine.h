#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class State;

class ENGINE_DLL StateMachine final : public Component
{
private:
	explicit StateMachine();
	explicit StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit StateMachine(const StateMachine& Prototype);
	virtual ~StateMachine() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Update_State(_float fTimeDelta);
	virtual void	Update_State_Select(_uint _stateNum, _float fTimeDelta) final;

public:
	// 상태 추가
	HRESULT Add_State(_uint iStateType, State* pState);
	// 상태 전환
	HRESULT Change_State(_uint iStateType);
	// 초기 세팅용
	void Set_State(_uint iStateType);

	State* Find_State(_uint iStateType);

	State* Get_CurrentState() { return m_pCurState; }
	_uint  Get_CurStateType() const { return m_iCurStateType; }
	_uint  Get_PrevStateType() const { return m_iPrevStateType; }

	void   Set_CurStateType(_uint iCurStateType) { m_iCurStateType = iCurStateType; }
	DAMAGEPOWER Get_CurrentState_DamagePower();

	HRESULT Add_Additional_Animation(_uint _stateNum, _uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop = false, _float _lerpTime = 0.2f);

	void Reset_State(); //상태 비우고 초기화용(상체상태머신)

private:
	map<_uint, State*> m_mapStates;

	State* m_pCurState = { nullptr };
	State* m_pPrevState = { nullptr };

	_uint m_iCurStateType = {};
	_uint m_iPrevStateType = {};

public:
	static StateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END