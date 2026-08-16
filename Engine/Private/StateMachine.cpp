#include "Engine_Define.h"
#include "StateMachine.h"
#include "State.h"

StateMachine::StateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component( pDevice, pContext )
{
}

StateMachine::StateMachine(const StateMachine& Prototype)
	: Component( Prototype )
{
}

HRESULT StateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT StateMachine::Initialize(void* pArg)
{
	return S_OK;
}

void StateMachine::Update_State(_float fTimeDelta)
{
	if (m_pCurState == nullptr)
		return;

	m_pCurState->Update_State(fTimeDelta);
}

void Engine::StateMachine::Update_State_Select(_uint _stateNum, _float fTimeDelta)
{
	m_mapStates[_stateNum]->Update_State(fTimeDelta);
}

HRESULT StateMachine::Add_State(_uint iStateType, State* pState)
{
	if (pState == nullptr)
		return E_FAIL;

	m_mapStates.emplace(iStateType, pState);

	return S_OK;
}

HRESULT StateMachine::Change_State(_uint iStateType)
{
	// 같은 상태라면 무시
	if (iStateType == m_iCurStateType)
		return S_OK;

	// 새 상태 검사
	auto iter = m_mapStates.find(iStateType);
	if (iter == m_mapStates.end())
		return E_FAIL;

	// 검사 후 현재 상태를 이전 상태로 저장
	m_iPrevStateType = m_iCurStateType;
	m_pPrevState = m_pCurState;

	if (m_pCurState != nullptr)
		m_pCurState->Exit_State();

	// 새 상태 전환
	m_iCurStateType = iStateType;
	m_pCurState = iter->second;
	m_pCurState->Enter_State();

	return S_OK;
}

void StateMachine::Set_State(_uint iStateType)
{
	// 처음 설정하는 상태 검사
	auto iter = m_mapStates.find(iStateType);
	if (iter == m_mapStates.end())
		return;

	// 상태 입장
	m_iCurStateType = iStateType;

	m_pCurState = iter->second;
	m_pCurState->Enter_State();
}

State* Engine::StateMachine::Find_State(_uint iStateType)
{
	auto iter = m_mapStates.find(iStateType);
	if (iter == m_mapStates.end())
		return nullptr;

	return iter->second;
}

DAMAGEPOWER Engine::StateMachine::Get_CurrentState_DamagePower()
{
	if (m_pCurState == nullptr)
		return DAMAGEPOWER::POWER_END;

	return m_pCurState->Get_DamagePower();
}

HRESULT Engine::StateMachine::Add_Additional_Animation(_uint _stateNum, _uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	return m_mapStates[_stateNum]->Add_Additional_Anim(_animIndex, _speed, _slotNum, _isLoop, _lerpTime);
}

void Engine::StateMachine::Reset_State()
{
	if (m_pCurState != nullptr)
		m_pCurState->Exit_State();

	m_iPrevStateType = m_iCurStateType;
	m_pPrevState = m_pCurState;
	m_pCurState = nullptr;
	m_iCurStateType = (_uint)-1; 
}

StateMachine* StateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	StateMachine* pInstance = new StateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component* StateMachine::Clone(void* pArg)
{
	StateMachine* pInstance = new StateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : StateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void StateMachine::Free()
{
	__super::Free();

	for (auto& Pair : m_mapStates)
		Safe_Release(Pair.second);
	m_mapStates.clear();

}
