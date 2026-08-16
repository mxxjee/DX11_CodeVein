#include "Client_Define.h"
#include "Monster_Olv_JumpAttack.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Olv_JumpAttack::Monster_Olv_JumpAttack()
{
}

HRESULT Client::Monster_Olv_JumpAttack::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	Set_DamagePower(DAMAGEPOWER::SPECIALPOWER);
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Olv_JumpAttack::Enter_State()
{
	if (m_pMonster == nullptr) return;

	m_pMonster->LookAt_Target();
	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);
}

void Client::Monster_Olv_JumpAttack::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;

	if (m_pMonster->Get_Model()->Is_AnimFinished())
	{
		m_pStateMachine->Change_State(Monster::IDLE);
	}
}

void Client::Monster_Olv_JumpAttack::Exit_State()
{
	if (m_pMonster == nullptr) return;
	m_pMonster->Set_TargetCached(false);
	m_pMonster->Set_SuperArmor(false);
}

Monster_Olv_JumpAttack* Client::Monster_Olv_JumpAttack::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Olv_JumpAttack* pInstance = new Monster_Olv_JumpAttack();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Olv_JumpAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Olv_JumpAttack::Free()
{
	__super::Free();
}
