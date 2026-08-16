#include "Client_Define.h"
#include "MState_Down.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Down::MState_Down()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Down::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_vecDownAnimData.resize(Monster::DOWN_END);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Down::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	// 다운 상태 가져오기
	Monster::DOWN_STATE state = m_pMonster->Get_DownState();

	// 만약 세팅되어있지 않았으면 안전하게 BATTLE상태로 돌리기
	if (state == Monster::DOWN_END || m_vecDownAnimData[state].bIsSet == false)
	{
		m_pMonster->Change_State(Monster::MS_BATTLE);
		return;
	}

	// 정보가 있으면 애니메이션 세팅
	_uint animIndex = m_vecDownAnimData[state].iAnimationIndex;
	_float animSpeed = m_vecDownAnimData[state].fAnimationSpeed;
	_bool animLoop = m_vecDownAnimData[state].bIsLoop;
	_float animLerp = m_vecDownAnimData[state].fLerpTime;

	m_pMonster->Set_Animation_CS(animIndex, animSpeed, animLoop, animLerp);
	m_pMonster->Set_Lifted(false);

	m_bFirstUpdate = true;
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Down::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);
	if (m_bFirstUpdate)
	{
		m_bFirstUpdate = false;//바로 animfinished될떄가있어서..1프레임무시체크
		return;
	}


	if (!m_pMonster->Is_Animation_Finished())
		return;

	m_pMonster->Change_State(Monster::MS_BATTLE);
}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Down::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_DownState(Monster::DOWN_END);
	m_pMonster->Set_CanBackAttack(true);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 추가 애니메이션 ////////////////////////////////////////////////////////
HRESULT Client::MState_Down::Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	Animation_Data data;
	data.bIsLoop = _isLoop;
	data.fAnimationSpeed = _speed;
	data.fLerpTime = _lerpTime;
	data.iAnimationIndex = _animIndex;
	data.bIsSet = true;

	m_vecDownAnimData[_slotNum] = data;

	return S_OK;
}
/******************************************************* 추가 애니메이션 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Down* Client::MState_Down::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Down* pInstance = new MState_Down();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Down 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Down::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
