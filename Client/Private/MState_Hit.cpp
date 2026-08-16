#include "Client_Define.h"
#include "MState_Hit.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Hit::MState_Hit()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Hit::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Hit::Enter_State()
{
	// 몬스터가 MELEE공격 상태에 들어왔다
	CHECK_JUST_NULL(m_pMonster);

	// 어떤 속도로 회전할지
	//m_pMonster->Set_TurnType(Monster::TURN_ATTACK);
	m_pMonster->Set_Attack(false);	 // 공격 가능상태 false로 만들어서 쿨타임 진행시키기(Monster의 Update가 함)
	//m_pMonster->Set_Walk_Dir(Monster::WALK_END);	// 

	//HitLevel 세팅
	m_eHitLevel = m_pMonster->Get_HitLevel();
	m_eHitDir = m_pMonster->Get_HitDir();

	//받은 hitlevel, hitdir로 조합 키 생성
	_int iKey = MakeHitKey(m_eHitLevel, m_eHitDir);
	auto iter = m_umapAnimationData.find(iKey);
	if (iter != m_umapAnimationData.end())
	{
		const auto& data = iter->second;
		m_pMonster->Set_Animation_CS(data.iAnimationIndex, data.fAnimationSpeed, data.bIsLoop, data.fLerpTime);
	
		if (m_eHitLevel == HIT_LEVEL::HIT_STRONG)
		{
			Monster::DOWN_STATE eDownState = (m_eHitDir == HIT_DIR::HIT_FL || m_eHitDir == HIT_DIR::HIT_FR ? Monster::DOWN_STATE::DOWN_FRONT : Monster::DOWN_STATE::DOWN_BACK);


			m_pMonster->Set_DownState(eDownState);
		}
	}
	else
	{
		//없으면 기본재생
		m_pMonster->Set_Animation_CS(m_iAnimIdx, m_fAnimSpeed);
	}


}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Hit::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);
}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Hit::Exit_State()
{

	CHECK_JUST_NULL(m_pMonster);
	m_pMonster->Set_Attack(true);

	m_pMonster->Set_TurnType(Monster::TURN_END);
}
/******************************************************* 상태 탈출 *******************************************************/

//////////////////////////////////////////////////////// 추가 애니메이션 등록 ////////////////////////////////////////////////////////
HRESULT Client::MState_Hit::Add_Additional_Anim(_uint _Index, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	Animation_Data data;
	data.bIsLoop = _isLoop;
	data.fAnimationSpeed = _speed;
	data.fLerpTime = _lerpTime;
	data.iAnimationIndex = _Index;

	m_umapAnimationData.emplace(_slotNum, data);

	return S_OK;
}
/******************************************************* 추가 애니메이션 등록 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Hit* Client::MState_Hit::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Hit* pInstance = new MState_Hit();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Hit 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Hit::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
