#include "Client_Define.h"
#include "Mstate_Awake_Idle.h"
#include "GameObject.h"
#include "Monster.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Mstate_Awake_Idle::Mstate_Awake_Idle()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Mstate_Awake_Idle::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::Mstate_Awake_Idle::Enter_State()
{
	// 몬스터가 깨어나기전 awake_loop animation
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_Animation_CS(m_iAnimIdx, m_fAnimSpeed, true);
}

void Client::Mstate_Awake_Idle::Update_State(_float fTimeDelta)
{	
	// 깨어나는 몬스터는
	CHECK_JUST_NULL(m_pMonster);

	//다음전이조건은 jsonc에 작성
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Mstate_Awake_Idle* Client::Mstate_Awake_Idle::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Mstate_Awake_Idle* pInstance = new Mstate_Awake_Idle();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"Mstate_Awake_Idle 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/


void Client::Mstate_Awake_Idle::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);
}


//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Mstate_Awake_Idle::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

