#include "Client_Define.h"
#include "Player_GuardWalk.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_GuardWalk::Player_GuardWalk()
{
}

HRESULT Client::Player_GuardWalk::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false); //우선은 false 
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_GuardWalk::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
	if (ActiveWeapon == nullptr)
		return;
	else
	{
		Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
		ANIMREQUEST_DESC AnimDesc = {};

		_uint iAnimationIndex = {};

		_vector vPlayerLook =m_pPlayerTransformCom->Get_State(DIRECTION::LOOK);
		
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, XMVector3Normalize(Desc.vMoveDir))); //이 내적을 통해서 1이면 서로 같은 방향 -1이면 반대방향 ,cos

		//외적을 통해서 -면 왼쪽 +면 오른쪽 좌우 판정 ,sin
		_vector vCross = XMVector3Cross(vPlayerLook, XMVector3Normalize(Desc.vMoveDir));
		_float fCrossY = XMVectorGetY(vCross); 

		if (fDot > 0.75f) //거의 같은 방향이라고 판정
		{
			iAnimationIndex = 74;
		}
		else if (fDot < -0.75f) //반대 방향이라고 판정
		{
			iAnimationIndex = 78;
		}
		else if (fCrossY > 0.f) //오른쪽
		{
			if (fDot > 0.25f) iAnimationIndex = 71;
			else if (fDot < -0.25f) iAnimationIndex = 75;
			else iAnimationIndex = 69;
		}
		else if (fCrossY <0.f)//왼쪽
		{
			if (fDot > 0.25f) iAnimationIndex = 72;
			else if (fDot < -0.25f) iAnimationIndex = 77;
			else iAnimationIndex = 70;
		}
		else
		{
			iAnimationIndex = 74;
		}

		//switch (Desc.InputDir)
		//{
		//case INPUT_DIR::LEFT: iAnimationIndex = 70; break;
		//case INPUT_DIR::RIGHT: iAnimationIndex = 69; break;
		//case INPUT_DIR::UP: iAnimationIndex = 74; break;
		//case INPUT_DIR::DOWN: iAnimationIndex = 78; break;
		//case INPUT_DIR::LEFT_UP: iAnimationIndex = 72; break;//왼발 오른발 있음
		//case INPUT_DIR::LEFT_DOWN: iAnimationIndex = 77; break;
		//case INPUT_DIR::RIGHT_UP: iAnimationIndex = 71; break;
		//case INPUT_DIR::RIGHT_DOWN:iAnimationIndex = 75; break;//왼발 오른발있음 
		//default:
		//	break;
		//}'

		m_pPlayerMasterRig->Set_Animation(iAnimationIndex, true);
	}
}

void Client::Player_GuardWalk::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	if (Desc.bAltPressed && XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayerMasterRig->Request_ClearAnimationUpper(0.1f);

		auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
		if (ActiveWeapon == nullptr)
			return;

		ANIMREQUEST_DESC AnimDesc = {};
		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::GUARD_LOOP;

		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_Animation(iAnimIndex, true, 0.2f);
		return;
	}
	else if (Desc.bAltPressed && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		Enter_State(); 
		return;
	}
	else if (Desc.bAltUp) //알트 키를 뗐으면 End로 진입
	{
		m_pUpperStateMachine->Change_State(Player::GUARDEND);
		m_pStateMachine->Change_State(Player::IDLE);
		return;
	}

}

void Client::Player_GuardWalk::Exit_State()
{
}

Player_GuardWalk* Client::Player_GuardWalk::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_GuardWalk* pInstance = new Player_GuardWalk();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_GuardWalk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_GuardWalk::Free()
{
	__super::Free();
}
