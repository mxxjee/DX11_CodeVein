 #include "Client_Define.h"
#include "Player_DamageBlow.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "Player_Stat.h"
#include "Player_Death.h"

Client::Player_DamageBlow::Player_DamageBlow()
{
}

HRESULT Client::Player_DamageBlow::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	m_pPlayerStatCom = m_pPlayer->Get_PlayerStatCom();
	Set_HasAnimEvents(false);
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_DamageBlow::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	_vector vEnemyHitDirection = m_pPlayer->Get_EnemyHitDirection();

	if (!m_bFirstDamage)
	{
		DAMAGEPOWER eDamagePower = m_pPlayer->Get_EnemyDamagePower();
		if (eDamagePower == DAMAGEPOWER::HEAVYPOWER)
			m_eDamageBlowType = BLOW;
		else if (eDamagePower == DAMAGEPOWER::SPECIALPOWER)
			m_eDamageBlowType = STRIKE;
	}
	
	m_bFirstDamage = true;

	_vector vPlayerLook = m_pPlayerTransformCom->Get_State(DIRECTION::LOOK);
	INPUT_DIR InputDir = m_pPlayer->Calculate_Dir(vPlayerLook, vEnemyHitDirection);
	INPUT_DIR Input4Dir = m_pPlayer->Convert8To4Dir(InputDir);

	_int iAnimIndex = {};

	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
	if (ActiveWeapon == nullptr)
		return;
	else
	{
		switch (m_eDamageBlowType)
		{
		case BLOW: //날아가기 
		{
			switch (Input4Dir)
			{
			case Engine::INPUT_DIR::LEFT: iAnimIndex = 110;
				m_eDownPosture = SUPINE;
				break;
			case Engine::INPUT_DIR::RIGHT: iAnimIndex = 111;
				m_eDownPosture = SUPINE;
				break;
			case Engine::INPUT_DIR::UP:iAnimIndex = 109;
				m_eDownPosture = PRONE;
				break;
			case Engine::INPUT_DIR::DOWN: iAnimIndex = 108;
				m_eDownPosture = SUPINE;
				break;
			default: iAnimIndex = 108;
				break;
			}
			break;
		}
		case STRIKE: //제자리 눕기 
			switch (Input4Dir)
			{
			case Engine::INPUT_DIR::LEFT: iAnimIndex = 116;
				m_eDownPosture = SUPINE;
				break;
			case Engine::INPUT_DIR::RIGHT: iAnimIndex = 116;
				m_eDownPosture = SUPINE;
				break;
			case Engine::INPUT_DIR::UP:iAnimIndex = 115;
				m_eDownPosture = PRONE;
				break;
			case Engine::INPUT_DIR::DOWN: iAnimIndex = 116;
				m_eDownPosture = SUPINE;
				break;
			default: iAnimIndex = 116;
				break;
			}
			break;
		case LOOP:
			switch (m_eDownPosture)
			{
			case PRONE: iAnimIndex = 124; 
				break;
			case SUPINE: iAnimIndex = 122;
				break;
			default: iAnimIndex = 124;
				break;
			}
			break;
		default:
			break;
		}


		m_pPlayerMasterRig->Set_Animation(iAnimIndex, false);
	}

}

void Client::Player_DamageBlow::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	m_pPlayer->Set_ApplyTranslation(false); //실제 움직임 반영x 여기서는 재생만 하고 실제 End로 보내기
	m_pPlayer->Reset_PlayerConsumeDesc(); //소모 입력도 받지 못함

	//추후에 오브젝트 충돌 판단해서 Strike도 넣어보기

	if (m_pPlayerStatCom->Get_Hp() <=0.99f) //피격상태일때 플레이어의 Hp를 체크
	{
		m_pPlayer->Set_ApplyTranslation(false); //움직이지 못하게

		if (m_pPlayerMasterRig->Is_AnimFinished() == true) //데미지 피격 재생 끝났다면
		{
			State* pFindState = m_pStateMachine->Find_State(Player::DEATH);
			Player_Death* pFindDeathState = dynamic_cast<Player_Death*>(pFindState);
			if (pFindDeathState)
			{
				if (m_eDownPosture == DOWN_POSTURE::PRONE)
				{
					pFindDeathState->Set_DeathType(DOWNDEATH_P);
				}
				else if (m_eDownPosture == DOWN_POSTURE::SUPINE)
				{
					pFindDeathState->Set_DeathType(DOWNDEATH_S);
				}

			}

			m_pStateMachine->Change_State(Player::DEATH);
		}

		return; //여기서는 계속 리턴해주고
	}


	if (m_eDamageBlowType == DAMAGEBLOWTYPE::LOOP)
	{
		m_fLoopEndTime -= fTimeDelta;
	}

	if (m_eDamageBlowType == DAMAGEBLOWTYPE::LOOP && m_fLoopEndTime <=0.f)
	{
		//여기서는 조건이 충족되면
		//DamageEnd로 보내기
		m_bFirstDamage = false;
		m_pStateMachine->Change_State(Player::DAMAGEEND);
	
		return;
	}
	else if (m_eDamageBlowType == DAMAGEBLOWTYPE::STRIKE && m_pPlayerMasterRig->Is_AnimFinished() == true)
	{
		m_bFirstDamage = false;
		m_pStateMachine->Change_State(Player::DAMAGEEND);

		return;
	}
	else if (m_eDamageBlowType == DAMAGEBLOWTYPE::BLOW && m_pPlayerMasterRig->Is_AnimFinished() == true)
	{
		if (m_pPlayer->Get_LoseHpRatio() <= 30.f)
		{
			m_fLoopEndTime = 0.6f;
		}
		else if(m_pPlayer->Get_LoseHpRatio()>=30.f)
		{
			m_fLoopEndTime = 1.0f;
		}
		else
		{
			m_fLoopEndTime = 1.5f;
		}
		//애니메이션 재생 끝나면 루프로 들어가게 한후에 
		m_eDamageBlowType = LOOP;
		Enter_State();
		return;
	}
	

	//후반에는 키입력가능하게
}

void Client::Player_DamageBlow::Exit_State()
{
}

Player_DamageBlow* Client::Player_DamageBlow::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_DamageBlow* pInstance = new Player_DamageBlow();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_DamageBlow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_DamageBlow::Free()
{
	__super::Free();
}
