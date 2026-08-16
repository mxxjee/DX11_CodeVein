#include "Client_Define.h"
#include "Monster_Patrol.h"
#include "GameObject.h"
#include "Monster.h"



Client::Monster_Patrol::Monster_Patrol()
{
}



HRESULT Client::Monster_Patrol::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx,fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);

	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Patrol::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;
	//이동하기 전에 낭떠러지체크
	if (!m_pMonster->Check_Floor_By_Raycast(1.5f))
	{
	
		//낭떠러지 만났으면 뒤돌아!!
		m_pStateMachine->Change_State(Monster::PATROL_TURN);
		return;
	}


	//플레이어와의 비교(상태전이)
	_float fDist = m_pMonster->Get_DistanceToTarget();


	//정찰범위안->뛰어가기!! 쫓아가랏
	if (fDist <= m_pMonster->Get_DetectRange()) {

		m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());


		
		
		
	}



	///////////아래는 플레이어 발견못했을때의 경우
	//정찰 할 곳 업데이트
	//정찰 지점에 도착했는지 확인하기
	if (m_pMonster->Get_DistanceToPatrolPoint() <= 0.5f)
	{
		//도착했으면 idle(잠깐 휴식)
		m_pStateMachine->Change_State(Monster::ENEMY_STATE::IDLE);
		return;
	}
	

	m_pMonster->LookAt_PatrolPoint(XMConvertToRadians(90.f),fTimeDelta);
	m_pMonster->Set_MoveDirectionToPatrolPoint();
}

void Client::Monster_Patrol::Enter_State()
{
	if (m_pMonster == nullptr)
		return;

	m_pMonster->Set_Speed(m_pMonster->Get_WalkSpeed());

	auto* ActiveWeapon = m_pMonster->Get_ActiveMonsterWeapon();
	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, true,0.2f,m_fAnimSpeed);

	//타겟포인트갱신
	m_pMonster->Update_Patrol_Target();


	//COUT("MonsterState::PATROL");
}

void Client::Monster_Patrol::Exit_State()
{
	
}

Monster_Patrol* Client::Monster_Patrol::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Patrol* pInstance = new Monster_Patrol();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx,fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Patrol");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void Client::Monster_Patrol::Free()
{
	__super::Free();
}
