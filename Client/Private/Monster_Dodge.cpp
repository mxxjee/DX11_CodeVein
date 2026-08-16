#include "Client_Define.h"
#include "Monster_Dodge.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Dodge::Monster_Dodge()
{
}

HRESULT Client::Monster_Dodge::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx,fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Dodge::Enter_State()
{
	if (m_pMonster == nullptr)
		return;

	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);

	//COUT("MonsterState::DODGE");
}

void Client::Monster_Dodge::Update_State(_float fTimeDelta)
{
}

void Client::Monster_Dodge::Exit_State()
{

}

Monster_Dodge* Client::Monster_Dodge::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Dodge* pInstance = new Monster_Dodge();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Dodge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Dodge::Free()
{
	__super::Free();
}
