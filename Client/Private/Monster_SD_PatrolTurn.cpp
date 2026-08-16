#include "Client_Define.h"
#include "Monster_SD_PatrolTurn.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"
#include "Animation.h"
#include "Slave_Devil.h"



Client::Monster_SD_PatrolTurn::Monster_SD_PatrolTurn()
{
}

HRESULT Client::Monster_SD_PatrolTurn::Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner,iAnimIdx,fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_SD_PatrolTurn::Enter_State()
{
	if (m_pMonster == nullptr)
		return;

	m_fTimeAcc = 0.f;

	_vector vLook = XMVector3Normalize(m_pMonsterTransformCom->Get_State(DIRECTION::LOOK));
	m_vStartLookDir = XMVector3Normalize(vLook);

	_vector vRight = XMVector3Normalize(m_pMonsterTransformCom->Get_State(DIRECTION::RIGHT));

	//낭떠러지를만났다,자동적으로 뒤돌아보게하기
	m_vTargetDir = XMVector3Normalize((vLook * -1.f) + (vRight * 0.5f));
	m_vTargetDir = XMVectorSetY(m_vTargetDir, 0.f);


	if (XMVectorGetX(XMVector3LengthSq(m_vTargetDir)) > 0.0001f)
	{
		_float fAngleDegree = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenVectors(vLook, m_vTargetDir)));

		_float fDotRight = XMVectorGetX(XMVector3Dot(vRight, m_vTargetDir));
		_bool bIsRight = (fDotRight > 0.f);

		_uint iAnimIndex = 11;

		if (fAngleDegree <= 91.f)
		{
			iAnimIndex = bIsRight ? 11 : 12;
		}
		else
		{
			iAnimIndex = bIsRight ? 13: 14;
		}

		m_pMonster->Get_Model()->Set_Animation_CS(iAnimIndex, false);

	}
	else
	{
		m_pMonster->Get_Model()->Set_Animation_CS(11, false);
	}
}

void Client::Monster_SD_PatrolTurn::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;


	if (m_pMonster->Get_Model()->Is_AnimFinished())
	{
		//
		//m_pStateMachine->Change_State(Slave_Devil::SLAVE_DEVIL_PATTERN::PATROL_EXIT);
		//return;
	}


}

Monster_SD_PatrolTurn* Client::Monster_SD_PatrolTurn::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_SD_PatrolTurn* pInstance = new Monster_SD_PatrolTurn();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_SD_PatrolTurn");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void Client::Monster_SD_PatrolTurn::Exit_State()
{
}

void Client::Monster_SD_PatrolTurn::Free()
{
	__super::Free();
}
