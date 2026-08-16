#include "Client_Define.h"
#include "Player_SpecialSuck.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "Player_DamageBlow.h"
#include "Player_BloodWeapon.h"
#include "Monster.h"
#include "UIObject.h"


Client::Player_SpecialSuck::Player_SpecialSuck()
{
}

HRESULT Client::Player_SpecialSuck::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false);
	Set_DamagePower(DAMAGEPOWER::STUNPOWER);
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	m_vecDrapeWolfKeyFrames = CinematicPreset::Load("../../DataFiles/CinematicPreset/WolfSuck.json");

	m_BackStabScreenAlarm.Limit = 5.1f;
	m_BackStabScreenAlarm.m_AlarmFunc = [this]()
		{
			if(!m_pBackStabScreen)
				m_pBackStabScreen= m_pGameInstance->Find_UI_ByName(L"BackStabScreen");

			CHECK_JUST_NULL(m_pBackStabScreen);

			m_pBackStabScreen->Set_Active_Delay(3.f);

			// 타겟 몬스터가 있으면
			// 데미지 이벤트도 발행
			Monster* monster = m_pPlayer->Get_TargetMonster();
			if (monster != nullptr)
			{
				DAMAGE_EVENT damageEvent = {};
				damageEvent.iAttackerID = m_pPlayer->Get_ObjectID();
				damageEvent.iTargetID = monster->Get_ObjectID();
				damageEvent.fDamage = m_pGameInstance->RandomValue(1023.f, 1511.f); // 랜덤값 
				damageEvent.vHitPosition = monster->Get_Position_Float3(); // sweep/overlap 결과의 실제 접촉 위치 사용
				damageEvent.vHitDirection = _float3(0.f, 0.f, 0.f);
				damageEvent.fKnockbackForce = 0.f;
				damageEvent.eDamagePower = DAMAGEPOWER::NORMALPOWER;
				damageEvent.pDamageOwner = m_pPlayer;

				m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
			}
			m_BackStabScreenAlarm.Off();
		};

	m_BackStabScreenAlarm.Off();
	return S_OK;
}

void Client::Player_SpecialSuck::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	auto* ActiveBloodWeapon = m_pPlayer->Get_ActivePlayerBloodWeapon();
	if (ActiveBloodWeapon == nullptr)
		return;
	else
	{
		_int iAnimIndex = 134;

		//만약에 특수무기 많아지면 기본 무기처럼 부모 만들어서 가져오게 만들어줘야겠지

		//// DrapeWolf용 카메라 이벤트
		CameraEvent event;
		event.eCameraState = CAMERA_STATE::CINEMATIC;
		event.bCinematicAutoReturn = true;
		event.vecKeyframes = m_vecDrapeWolfKeyFrames;  // 복사만 발생, IO 없음
		m_pGameInstance->Publish(event);

		m_pPlayerMasterRig->Set_Animation(iAnimIndex, false);
		Monster* monster = m_pPlayer->Get_TargetMonster();
		if (!monster)
			return;
		
		// 백스탭 한거라면
		if(m_pPlayer->Get_SuccessBackStab())
		{
			monster->Set_CinematicState(Monster::CINEMATIC_DRAPE_FULL_BACKSTAB);
			monster->Set_DownState(Monster::DOWN_FRONT);
			monster->Change_State(Monster::MS_CINEMATIC);
			monster->Set_Visible(false);
		}
		else
		{
			monster->Set_CinematicState(Monster::CINEMATIC_DRAPE_FULL);
			monster->Set_DownState(Monster::DOWN_BACK);
			monster->Change_State(Monster::MS_CINEMATIC);
			monster->Set_Visible(false);
		}

		m_bTeleported = false;
		m_fTeleportTimer = 0.f;
		m_fTeleportLimit = 2.f;	// << 이거는 흡혈아장마다 다르게

		m_BackStabScreenAlarm.Off();//초기화한번해주고
		m_BackStabScreenAlarm.On();
	}



}

void Client::Player_SpecialSuck::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	m_BackStabScreenAlarm.Update(fTimeDelta);

	if (m_pPlayerMasterRig->Is_AnimFinished() == true) //시네마틱 재생 끝나면 false로
	{
		m_pPlayer->Get_ActivePlayerWeapon()->Set_PartActive(true);
		m_pPlayer->Get_ActivePlayerBloodWeapon()->Set_PartActive(false);

		m_pPlayer->Set_ApplyTranslation(true);
		m_pPlayer->Set_PlayCinematic(false);
		m_pStateMachine->Change_State(Player::IDLE);
	}

	// 원래 텔레포트용으로 쓰려고 했는데 Visible 바꾸는게 나은듯
	if(m_bTeleported == false)
	{
		m_fTeleportTimer += fTimeDelta;

		// DrapeSuck일때의 일정 시간이 지나면
		if (m_fTeleportTimer >= m_fTeleportLimit)
		{
			// 몬스터 Visible 세팅
			Monster* monster = m_pPlayer->Get_TargetMonster();
			if (monster == nullptr)
				return;

			monster->Set_Visible(true);

			// 얘 이제 보여요
			m_bTeleported = true;
		}
	}

	m_pPlayer->Set_ApplyTranslation(false);

	//후반에는 키입력가능하게
}

void Client::Player_SpecialSuck::Exit_State()
{
	m_pPlayer->Set_SuccessBackStab(false);
}

Player_SpecialSuck* Client::Player_SpecialSuck::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_SpecialSuck* pInstance = new Player_SpecialSuck();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_SpecialSuck");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_SpecialSuck::Free()
{
	__super::Free();
}
