#include "Client_Define.h"
#include "Monster_Dead.h"
#include "GameObject.h"
#include "Monster.h"
#include "MonsterState.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Monster_Dead::Monster_Dead()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Dead::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
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
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::Monster_Dead::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);
	m_pMonster->Set_Kill();
	m_pMonster->OnDead();


	//COUT("MonsterState::DEad");
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::Monster_Dead::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);



}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::Monster_Dead::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Monster_Dead* Client::Monster_Dead::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Dead* pInstance = new Monster_Dead();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"Monster_Dead 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Monster_Dead::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
