#include "Client_Define.h"
#include "Player_UpperDummy.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_UpperDummy::Player_UpperDummy()
{
}

HRESULT Client::Player_UpperDummy::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false);

	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_UpperDummy::Enter_State()
{
	m_pPlayer->Reset_PlayerConsumeDesc(); //단발(소비)형들 초기화
	m_pPlayer->Reset_PlayerAnimEventDesc();//애니메이션 바뀔예정이므로 이벤트 초기화

}

void Client::Player_UpperDummy::Update_State(_float fTimeDelta)
{
	m_pPlayerMasterRig->Set_UpperAnimFinished(false);
}

void Client::Player_UpperDummy::Exit_State()
{
}

Player_UpperDummy* Client::Player_UpperDummy::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_UpperDummy* pInstance = new Player_UpperDummy();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_UpperDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_UpperDummy::Free()
{
	__super::Free();
}
