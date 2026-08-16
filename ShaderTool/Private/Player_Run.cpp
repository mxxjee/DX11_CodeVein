#include "ShaderTool_Define.h"
#include "Player_Run.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

ShaderTool::Player_Run::Player_Run()
{
}

HRESULT ShaderTool::Player_Run::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false);
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void ShaderTool::Player_Run::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	m_pPlayerMasterRig->Set_Animation(14, true);
}

void ShaderTool::Player_Run::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	if (Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB)
	{
		m_pStateMachine->Change_State(Player::ATTACKNORMAL);
		return;
	}
	else if (Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB)
	{
		m_pStateMachine->Change_State(Player::ATTACKSTRONG);
		return;
	}
	else if (Desc.bShiftPressed)
	{
		m_pStateMachine->Change_State(Player::DASH);
		return;
	}
	else if (Desc.bAltDown)
	{
		m_pStateMachine->Change_State(Player::GUARDSTART);
		return;
	}


	if (XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //이거 이동량 없는경우
		m_pStateMachine->Change_State(Player::IDLE);
}

void ShaderTool::Player_Run::Exit_State()
{
}

Player_Run* ShaderTool::Player_Run::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_Run* pInstance = new Player_Run();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_Run");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ShaderTool::Player_Run::Free()
{
	__super::Free();
}
