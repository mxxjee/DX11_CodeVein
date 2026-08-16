#include "Client_Define.h"
#include "MState_Cinematic.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Cinematic::MState_Cinematic()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Cinematic::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_vecCinematicAnimData.resize(Monster::CINEMATIC_END);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Cinematic::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	// 시네마틱 상태 가져오기
	Monster::CINEMATIC_STATE state = m_pMonster->Get_CinematicState();

	// 만약 세팅되어있지 않았으면 안전하게 BATTLE상태로 돌리기
	if (state == Monster::CINEMATIC_END || m_vecCinematicAnimData[state].bIsSet == false)
		m_pMonster->Change_State(Monster::MS_BATTLE);

	// 정보가 있으면 애니메이션 세팅
	_uint animIndex = m_vecCinematicAnimData[state].iAnimationIndex;
	_float animSpeed = m_vecCinematicAnimData[state].fAnimationSpeed;
	_bool animLoop = m_vecCinematicAnimData[state].bIsLoop;
	_float animLerp = m_vecCinematicAnimData[state].fLerpTime;

	m_pMonster->Set_Animation_CS(animIndex, animSpeed, animLoop, animLerp);
	m_pMonster->Set_Lifted(true);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Cinematic::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

	if (!m_pMonster->Is_Animation_Finished())
		return;

	m_pMonster->Change_State(Monster::MS_DOWN);
}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Cinematic::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_CinematicState(Monster::CINEMATIC_END);
	m_pMonster->Set_Lifted(false);
}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 추가 애니메이션 ////////////////////////////////////////////////////////
HRESULT Client::MState_Cinematic::Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	Animation_Data data;
	data.bIsLoop = _isLoop;
	data.fAnimationSpeed = _speed;
	data.fLerpTime = _lerpTime;
	data.iAnimationIndex = _animIndex;
	data.bIsSet = true;

	m_vecCinematicAnimData[_slotNum] = data;

	return S_OK;
}
/******************************************************* 추가 애니메이션 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Cinematic* Client::MState_Cinematic::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Cinematic* pInstance = new MState_Cinematic();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Cinematic 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Cinematic::Free()
{
	__super::Free();

	m_vecCinematicAnimData.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/
