#include "Client_Define.h"
#include "MState_Walk.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Walk::MState_Walk()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Walk::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	// NONE을 제외한만큼 WALK상태 초기화
	m_vecWalkData.resize(Monster::WALK_END);


	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Walk::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	_int dir = -1;
	// 걷기 방향이 없으면(공격하면 -1로 초기화됨)
	if ((dir = m_pMonster->Get_Walk_Dir()) <= Monster::WALK_FRONT)
	{
		// 걷기 방향 세팅
		dir = m_pGameInstance->RandomValue_int(_UINT(Monster::WALK_FRONT), _UINT(Monster::WALK_RIGHT));
		// 걷는 방향이 등록 안 돼있으면 찾을때까지 루프
		while (m_vecWalkData[dir].bIsSet == false)
		{
			dir = m_pGameInstance->RandomValue_int(_UINT(Monster::WALK_FRONT), _UINT(Monster::WALK_RIGHT));
		}
		// 몬스터에 등록
		m_pMonster->Set_Walk_Dir(dir);
	}

	// 해당 방향으로 걷는 애니메이션으로 변경 
	m_pMonster->Set_Animation_CS(m_vecWalkData[dir].iAnimationIndex, m_vecWalkData[dir].fAnimationSpeed, m_vecWalkData[dir].bIsLoop, m_vecWalkData[dir].fLerpTime);
	
	m_pMonster->Set_TurnType(Monster::TURN_WALK);
	m_pMonster->Set_RootPosAble(true);
	m_pMonster->Set_Acceleration(Monster::ACC_WALK);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Walk::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

	

}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Walk::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_TurnType(Monster::TURN_END);
	m_pMonster->Set_RootPosAble(true);
	m_pMonster->Set_Acceleration(Monster::ACC_DEC);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 추가 정보 세팅 ////////////////////////////////////////////////////////
HRESULT Client::MState_Walk::Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	Animation_Data data;
	data.bIsLoop = _isLoop;
	data.fAnimationSpeed = _speed;
	data.fLerpTime = _lerpTime;
	data.iAnimationIndex = _animIndex;
	data.bIsSet = true;

	m_vecWalkData[_slotNum] = data;

	return S_OK;
}
/******************************************************* 추가 정보 세팅 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Walk* Client::MState_Walk::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Walk* pInstance = new MState_Walk();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Walk 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Walk::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
