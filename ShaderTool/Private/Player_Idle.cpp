#include "ShaderTool_Define.h"
#include "Player_Idle.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

ShaderTool::Player_Idle::Player_Idle()
{
}

HRESULT ShaderTool::Player_Idle::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

void ShaderTool::Player_Idle::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon(); //장착 무기 없는 경우는 기본 애니메이션 재생
	if (ActiveWeapon == nullptr)
		m_pPlayerMasterRig->Set_Animation(0, true);
	else
	{
		ANIMREQUEST_DESC AnimDesc = {};
		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::IDLE;
		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_Animation(iAnimIndex, true);
	}

}

void ShaderTool::Player_Idle::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	if (Desc.bShiftPressed && (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB || Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB))
	{
		m_pStateMachine->Change_State(Player::SPECIALATTACK);
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
	else if (Desc.bAltDown)
	{
		m_pStateMachine->Change_State(Player::GUARDSTART);
		return;
	}
	else if (Desc.bSpaceDown && XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pStateMachine->Change_State(Player::BACKSTEP);
		return;
	}
	else if (Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pStateMachine->Change_State(Player::RUN);
		return;
	}
}

void ShaderTool::Player_Idle::Exit_State()
{
}

Player_Idle* ShaderTool::Player_Idle::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_Idle* pInstance = new Player_Idle();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ShaderTool::Player_Idle::Free()
{
	__super::Free();
}
