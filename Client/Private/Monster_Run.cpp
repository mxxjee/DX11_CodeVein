#include "Client_Define.h"
#include "Monster_Run.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Run::Monster_Run()
{
}

HRESULT Client::Monster_Run::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(false);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Run::Enter_State()
{
	if (m_pMonster == nullptr)
		return;
	m_pMonster->Set_Speed(m_pMonster->Get_RunSpeed());

	//보스:4번 , 0.2f:default값
	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx,true,0.2f,m_fAnimSpeed);
	m_pMonster->Set_TurnType(Monster::TURN_RUN);
	



	//COUT("MonsterState::RUN");
}

void Client::Monster_Run::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;

	_float fDist = m_pMonster->Get_DistanceToTarget();

	//각도체크를 통해서 돌지안돌지 체크함

	if (fDist <= m_pMonster->Get_AttackRange()) {
		m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());
		//_vector vLook = m_pMonster->Get_Transform()->Get_State(DIRECTION::LOOK);

		//_float3 vNormDir = m_pMonster->Get_DirectionNorm();

		//_vector vToTarget=XMLoadFloat3(&vNormDir); // 정규화된 방향
		//_float fDot = XMVectorGetX(XMVector3Dot(vLook, vToTarget));
		//_float fAngle = XMConvertToDegrees(acosf(fmaxf(-1.f, fminf(1.f, fDot))));

		//if (fAngle > 35.f) // 각도가 15도 이상 벌어져 있다면
		//{
		//	m_pStateMachine->Change_State(Monster::TURN);
		//}
		//else
		//{
		//	// 정면에 있다면 바로 공격!
		//	m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());
		//}
		return;
	}

	if (fDist > m_pMonster->Get_ChaseRange()) {
		m_pStateMachine->Change_State(Monster::IDLE);
		return;
	}

	//m_pMonster->LookAt_Target();
	//m_pMonster->LookAt_Smooth_Target(fTimeDelta);
	_vector vLook = m_pMonster->Get_Transform()->Get_State(DIRECTION::LOOK);
	_float3 vNormDir = m_pMonster->Get_DirectionNorm();
	_vector vToTarget = XMLoadFloat3(&vNormDir); // 정규화된 방향
	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vToTarget));
	_float fAngle = XMConvertToDegrees(acosf(fmaxf(-1.f, fminf(1.f, fDot))));
	if (fAngle > 35.f) // 각도가 15도 이상 벌어져 있다면
	{
		m_pStateMachine->Change_State(Monster::TURN);
	}
	m_pMonster->LookAt_Smooth_Target(fTimeDelta);

	m_pMonster->Set_MoveDirectionToTarget();
}

void Client::Monster_Run::Exit_State()
{

}

Monster_Run* Client::Monster_Run::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Run* pInstance = new Monster_Run();

	if (FAILED(pInstance->Initialize(pOwner,iAnimIdx,fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Run");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Run::Free()
{
	__super::Free();
}
