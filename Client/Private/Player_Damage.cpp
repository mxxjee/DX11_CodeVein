#include "Client_Define.h"
#include "Player_Damage.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player_Weapon.h"
#include "Player_Stat.h"
#include "Player_Death.h"

Client::Player_Damage::Player_Damage()
{
}

HRESULT Client::Player_Damage::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	m_pPlayerStatCom = m_pPlayer->Get_PlayerStatCom();

	Set_HasAnimEvents(true);
	if (m_pPlayerTransformCom == nullptr || m_pUpperStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	_int iDamage1 = (_int)Player::DAMAGE_LEVEL::DAMAGE1;

	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::LEFT] =  94;
	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::RIGHT] =  95 ;
	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::UP] =  91 ;
	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::DOWN] =  88 ;
	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::LEFT_UP] = 92 ;
	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::LEFT_DOWN] =  89;
	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::RIGHT_UP] = 93 ;
	m_iDamage1AnimIndex[iDamage1][(_int)INPUT_DIR::RIGHT_DOWN] =  90 ;

	_int iDamage2 = (_int)Player::DAMAGE_LEVEL::DAMAGE2;
	m_iDamage2AnimIndex[iDamage2][Dir4Index(INPUT_DIR::LEFT)] =  98;
	m_iDamage2AnimIndex[iDamage2][Dir4Index(INPUT_DIR::RIGHT)] =  99 ;
	m_iDamage2AnimIndex[iDamage2][Dir4Index(INPUT_DIR::UP)] =  97 ;
	m_iDamage2AnimIndex[iDamage2][Dir4Index(INPUT_DIR::DOWN)] =  96 ;
	_int iDamage3 = (_int)Player::DAMAGE_LEVEL::DAMAGE3;
	m_iDamage3AnimIndex[iDamage3][Dir4Index(INPUT_DIR::LEFT)] =  102 ;
	m_iDamage3AnimIndex[iDamage3][Dir4Index(INPUT_DIR::RIGHT)] =  103 ;
	m_iDamage3AnimIndex[iDamage3][Dir4Index(INPUT_DIR::UP)] = 101 ;
	m_iDamage3AnimIndex[iDamage3][Dir4Index(INPUT_DIR::DOWN)] =  100 ;
	_int iDamage4 = (_int)Player::DAMAGE_LEVEL::DAMAGE4;
	m_iDamage4AnimIndex[iDamage4][Dir4Index(INPUT_DIR::LEFT)] =  106 ;
	m_iDamage4AnimIndex[iDamage4][Dir4Index(INPUT_DIR::RIGHT)] =  107 ;
	m_iDamage4AnimIndex[iDamage4][Dir4Index(INPUT_DIR::UP)] =  105 ;
	m_iDamage4AnimIndex[iDamage4][Dir4Index(INPUT_DIR::DOWN)] =  104 ;

	return S_OK;
}

void Client::Player_Damage::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	//데미지는 여기서 이벤트 종류 찾아서 분기 해야할듯 Damage도 총4개고 존재하고 방향도 존재함

	_vector vEnemyHitDirection = m_pPlayer->Get_EnemyHitDirection();

	_vector vPlayerLook = m_pPlayerTransformCom->Get_State(DIRECTION::LOOK);

	//_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, XMVector3Normalize(vEnemyHitDirection))); //이 내적을 통해서 1이면 서로 같은 방향 -1이면 반대방향 ,cos

	////외적을 통해서 -면 왼쪽 +면 오른쪽 좌우 판정 ,sin
	//_vector vCross = XMVector3Cross(vPlayerLook, XMVector3Normalize(vEnemyHitDirection));
	//_float fCrossY = XMVectorGetY(vCross);


	INPUT_DIR InputDir = m_pPlayer->Calculate_Dir(vPlayerLook, vEnemyHitDirection);
	
	//애니메이션 같은방향이면 반대로 해줘야함 

	_float fLoseHpRatio = m_pPlayer->Get_LoseHpRatio();
	DAMAGEPOWER eDamagePower = m_pPlayer->Get_EnemyDamagePower();
	Player::DAMAGE_LEVEL eDamageLevel = m_pPlayer->Calculate_DamageLevel(fLoseHpRatio, eDamagePower);

	//COUT("eDamagePower :" << static_cast<_int>(eDamagePower));

	_int iAnimationIndex = Get_DamageAnimIndex(eDamageLevel, InputDir);
	
	//COUT("iAnimIndex : " << iAnimationIndex);

	m_pPlayerMasterRig->Set_Animation(iAnimationIndex, false, 0.2f);

}

void Client::Player_Damage::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	if (m_pPlayerStatCom->Get_Hp() <=0.99f) //피격상태일때 플레이어의 Hp를 체크
	{
		m_pPlayer->Set_ApplyTranslation(false); //움직이지 못하게

		if (m_pPlayerMasterRig->Is_AnimFinished() == true) //데미지 피격 재생 끝났다면
		{
			State* pFindState = m_pStateMachine->Find_State(Player::DEATH);
			Player_Death* pFindDeathState = dynamic_cast<Player_Death*>(pFindState);
			if (pFindDeathState)
				pFindDeathState->Set_DeathType(DEATHIDLE);
			m_pStateMachine->Change_State(Player::DEATH);
		}

		return; //여기서는 계속 리턴해주고
	}

	if (!EventDesc.bCanMove())
	{
		m_pPlayer->Set_ApplyTranslation(false);
	}

	if (EventDesc.bCanCombo() && Desc.bShiftPressed && (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB || Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::SPECIALATTACK);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.bCDown && !Desc.bZeroStamina)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKPARRY);
		return;
	}
	else if (EventDesc.bCanCombo() && m_pPlayer->CanUseSkill())
	{
		m_pPlayer->Set_ApplyTranslation(true);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKNORMAL);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKSTRONGSTART);
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::BACKSTEP); //백스텝  
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if (EventDesc.bCanMove() && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::RUN);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bAltDown)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::GUARDWALK);
		m_pUpperStateMachine->Change_State(Player::GUARDSTART);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bDownKeyDown)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pUpperStateMachine->Change_State(Player::USEITEM);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bUpKeyDown)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pUpperStateMachine->Change_State(Player::WEAPONCHANGE);
		return;
	}


	//애니메이션이 끝났으면 
	if (m_pPlayerMasterRig->Is_AnimFinished() == true)
	{
		m_pStateMachine->Change_State(Player::IDLE);
	}

}

void Client::Player_Damage::Exit_State()
{
}

_int Client::Player_Damage::Get_DamageAnimIndex(Player::DAMAGE_LEVEL eDamageLevel,INPUT_DIR eInputDir)
{
	const _int iDamageLevel = (_int)eDamageLevel;

	if (eDamageLevel == Player::DAMAGE_LEVEL::DAMAGE1)
	{
		return m_iDamage1AnimIndex[iDamageLevel][(_int)eInputDir]; //8방향
	}

	INPUT_DIR Input4Dir = m_pPlayer->Convert8To4Dir(eInputDir);
	const _int iInput4Dir = Dir4Index(Input4Dir);

	switch (eDamageLevel)
	{
	case Player::DAMAGE_LEVEL::DAMAGE2:
		return m_iDamage2AnimIndex[iDamageLevel][iInput4Dir]; //4방향
	case Player::DAMAGE_LEVEL::DAMAGE3:
		return m_iDamage3AnimIndex[iDamageLevel][iInput4Dir]; //4방향
	case Player::DAMAGE_LEVEL::DAMAGE4:
		return m_iDamage4AnimIndex[iDamageLevel][iInput4Dir]; //4방향
	default: return m_iDamage1AnimIndex[(int)Player::DAMAGE_LEVEL::DAMAGE1][(int)INPUT_DIR::UP];
	}

}

_int Client::Player_Damage::Dir4Index(INPUT_DIR eInputDir)
{
	switch (eInputDir)
	{
	case INPUT_DIR::LEFT:  return 0;
	case INPUT_DIR::RIGHT: return 1;
	case INPUT_DIR::UP:    return 2;
	case INPUT_DIR::DOWN:  return 3;
	default:               return 2;
	}
}

Player_Damage* Client::Player_Damage::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_Damage* pInstance = new Player_Damage();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_Damage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_Damage::Free()
{
	__super::Free();
}
