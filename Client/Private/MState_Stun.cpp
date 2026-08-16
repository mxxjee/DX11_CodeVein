#include "Client_Define.h"
#include "MState_Stun.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Stun::MState_Stun()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Stun::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_vecStunAnimIndexData.resize(Monster::STUNSTATE_END);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Stun::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_CanBackAttack(false);
	m_pMonster->Set_AttackSweep(false);

	// 스턴 상태 가져오기
	Monster::STUN_STATE state = m_pMonster->Get_StunState();

	// 만약 세팅되어있지 않았으면 안전하게 BATTLE상태로 돌리기
	if (state == Monster::STUNSTATE_END || m_vecStunAnimIndexData[state].bIsSet == false)
		m_pMonster->Change_State(Monster::MS_BATTLE);

	// 정보가 있으면 애니메이션 세팅
	_uint animIndex = m_vecStunAnimIndexData[state].iAnimationIndex;
	_float animSpeed = m_vecStunAnimIndexData[state].fAnimationSpeed;
	_bool animLoop = m_vecStunAnimIndexData[state].bIsLoop;
	_float animLerp = m_vecStunAnimIndexData[state].fLerpTime;

	m_pMonster->Set_Animation_CS(animIndex, animSpeed, animLoop, animLerp);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Stun::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

	// 애니메이션 끝나기 대기
	if (!m_pMonster->Is_Animation_Finished())
		return;

	Monster::STUN_STATE state = m_pMonster->Get_StunState();

	switch (state)
	{
	case Monster::STUN_NONE:
		m_pMonster->Change_State(Monster::MS_BATTLE);
		break;
	case Monster::STUN_LOOP:	// 루프 상태였으면 END로 돌리기(시간 없어서 그냥 애니메이션 끝났을때 기준으로 함(야매))
	{
		Monster::STUN_STATE endState = Monster::STUN_END;
		_uint animIndex = m_vecStunAnimIndexData[endState].iAnimationIndex;
		_float animSpeed = m_vecStunAnimIndexData[endState].fAnimationSpeed;
		_bool animLoop = m_vecStunAnimIndexData[endState].bIsLoop;
		_float animLerp = m_vecStunAnimIndexData[endState].fLerpTime;
		m_pMonster->Set_Animation_CS(animIndex, animSpeed, animLoop, animLerp);
		break;
	}
	case Monster::STUN_END:
		m_pMonster->Change_State(Monster::MS_BATTLE);
		break;
	case Monster::STUNSTATE_END:
		m_pMonster->Change_State(Monster::MS_BATTLE);
		break;
	}

}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Stun::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

	// 다시 백어택 가능하게 만듬
	m_pMonster->Set_CanBackAttack(true);
	m_pMonster->Set_StunState(Monster::STUN_STATE::STUNSTATE_END);
}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 추가 정보 세팅 ////////////////////////////////////////////////////////
HRESULT Client::MState_Stun::Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	Animation_Data data;
	data.bIsLoop = _isLoop;
	data.fAnimationSpeed = _speed;
	data.fLerpTime = _lerpTime;
	data.iAnimationIndex = _animIndex;
	data.bIsSet = true;

	m_vecStunAnimIndexData[_slotNum] = data;

	return S_OK;
}
/******************************************************* 추가 정보 세팅 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Stun* Client::MState_Stun::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Stun* pInstance = new MState_Stun();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Stun 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Stun::Free()
{
	__super::Free();

	m_vecStunAnimIndexData.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/
