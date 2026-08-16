#include "Client_Define.h"
#include "Monster_Idle.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Idle::Monster_Idle()
{
}

HRESULT Client::Monster_Idle::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner,iAnimIdx,fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster ==nullptr)
		return E_FAIL;

	//아이들업데이트 타이머설정(Idle에서 바로 상태감지가 아닌 특정 대기시간이후 감지)
	m_Alarm_To_Update.Limit = m_pGameInstance->RandomValue(m_minTime, m_maxTime);
	m_Alarm_To_Update.m_AlarmFunc = [this]()
		{
			m_bUpdate = true;
		};

	m_Alarm_To_Update.Off();

	return S_OK;
}

void Client::Monster_Idle::Enter_State()
{
	if (m_pMonster == nullptr)
		return;

	auto* ActiveWeapon = m_pMonster->Get_ActiveMonsterWeapon();
	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, true, 0.2f,m_fAnimSpeed);

	//타이머초기화.
	m_Alarm_To_Update.Limit = m_pGameInstance->RandomValue(m_minTime, m_maxTime);
	m_Alarm_To_Update.Elapsed = 0.f;
	m_bUpdate = false;
	m_Alarm_To_Update.On();

	//COUT("MonsterState::IDLE");
}

void Client::Monster_Idle::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;

	//N초 이후 
	m_Alarm_To_Update.Update(fTimeDelta);
	CHECK_FALSE(m_bUpdate);

	_float fDist = m_pMonster->Get_DistanceToTarget();

	//Patrol유무 따지기.
	if (m_pMonster->Get_IdleType() == Monster::ENEMY_IDLE_TYPE::PATROL)
	{
		if (fDist > m_pMonster->Get_WalkRange())
		{
			m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());
			return;
		}
	}

	if (fDist <= m_pMonster->Get_DetectRange())
	{
		// COUT("인지 범위");
		_bool isInFOV = m_pMonster->Detect_With_FOV(); // 몬스터의 시야 안에 있는지 판단
		if (isInFOV)
		{
			m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());

		}
		else if (!isInFOV && m_pMonster->Get_IdleType() != Monster::ENEMY_IDLE_TYPE::PATROL)
		{
			m_pStateMachine->Change_State(Monster::AWAKE);
		}
		else if(m_pMonster->Get_IdleType() == Monster::ENEMY_IDLE_TYPE::PATROL)
		{
			m_pStateMachine->Change_State(Monster::PATROL);
			return;
		}
	
	}

}

void Client::Monster_Idle::Exit_State()
{
}

// 여기서만 쓸거 하드코딩임 건들면 큰일남
HRESULT Client::Monster_Idle::Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	m_minTime = _speed;
	m_maxTime = _lerpTime;
	return S_OK;
}

Monster_Idle* Client::Monster_Idle::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Idle* pInstance = new Monster_Idle();

	if (FAILED(pInstance->Initialize(pOwner,iAnimIdx,fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Idle::Free()
{
	__super::Free();
}
