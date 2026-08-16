#include "Client_Define.h"
#include "MState_Evade.h"
#include "GameObject.h"
#include "Monster.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Evade::MState_Evade()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Evade::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	// NONE을 제외한만큼 Evade상태 초기화
	m_vecEvadeData.resize(Monster::EAVDE_DIR::EVADE_END);

	
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::MState_Evade::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);
	
	_int dir = m_pGameInstance->RandomValue_int(_UINT(Monster::EAVDE_DIR::EVADE_LEFT), _UINT(Monster::EVADE_RIGHT));

	m_pMonster->Set_Walk_Dir(dir);

	auto& data = m_vecEvadeData[dir];


	m_pMonster->Set_Animation_CS(data.iAnimationIndex, data.fAnimationSpeed, false, data.fLerpTime);

	m_pMonster->Set_TurnType(Monster::TURN_WALK);
	m_pMonster->Set_RootPosAble(true);
	m_pMonster->Set_Acceleration(Monster::ACC_EVADE);
}


void Client::MState_Evade::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

	//플레이어쳐다보면서 회피해야함
	m_pMonster->LookAt_Smooth_Target(fTimeDelta);
}

void Client::MState_Evade::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_TurnType(Monster::TURN_END);
	m_pMonster->Set_RootPosAble(true);
	m_pMonster->Set_Acceleration(Monster::ACC_DEC);

}

HRESULT Client::MState_Evade::Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
	Animation_Data data;
	data.bIsLoop = _isLoop;
	data.fAnimationSpeed = _speed;
	data.fLerpTime = _lerpTime;
	data.iAnimationIndex = _animIndex;
	data.bIsSet = true;

	m_vecEvadeData[_slotNum] = data;

	return S_OK;
}


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Evade* Client::MState_Evade::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Evade* pInstance = new MState_Evade();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Evade 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/

void Client::MState_Evade::Free()
{
	__super::Free();
}
