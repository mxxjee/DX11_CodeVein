#include "Client_Define.h"
#include "Monster_Boss_Intro.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"
#include "BossBase.h"

#include "UIObj_BossName.h"
#include "UIObj_ProgressBar.h"
#include "UIObj_HpBar.h"
#include "InteractionManager.h"



Client::Monster_Boss_Intro::Monster_Boss_Intro()
{
}

HRESULT Client::Monster_Boss_Intro::Initialize(GameObject* pOwner, _float fSecond, _float fSpeed)
{
	__super::Initialize(pOwner);
	
	m_pGameInstance->Subscribe< BossIntroEvent>([this](const BossIntroEvent& Event)
		{

			if (Event.bFinish)
			{
				if (m_pStateMachine)
					m_pStateMachine->Set_State(Monster::IDLE);



							}
		});

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));

	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	m_AppearUIAlarm.Limit = fSecond;
	m_AppearUIAlarm.m_AlarmFunc = [this]()
		{
			BossBase* pBoss = dynamic_cast< BossBase*>(m_pOwner);
			if (pBoss)
			{
				//이름켜주기!!내부에서 알아서 보스hp바랑연동.
				UIObj_BossName::BossNameUIEVent BossNameEvent;
				BossNameEvent.bActive = true;
				BossNameEvent.BossName_Korean = pBoss->Get_ApperUIName_KR();
				BossNameEvent.BossName_English = pBoss->Get_ApperUIName_Eng();

				m_pGameInstance->Publish(BossNameEvent);
				
			
				//야쿠모햄등장
				UIObject* pBuddyHpar = m_pGameInstance->Find_UI_ByName(L"Buddy_Statusbar");
				if (pBuddyHpar)
				{
					pBuddyHpar->Set_Visible(true);
					pBuddyHpar->Set_Active(true);

				}




				m_AppearUIAlarm.Off();
			}
		};

	m_AppearUIAlarm.Off();
	m_fAnimSpeed = fSpeed;

	return S_OK;
}

void Client::Monster_Boss_Intro::Enter_State()
{
	if (m_pMonster == nullptr) return;

	m_bIntroPlayed = false;
	m_pMonster->Get_Model()->Set_Animation_CS(20, true, 2.f);

	UIObject* pBossHpBar = m_pGameInstance->Find_UI_ByName(L"Boss_HPBar");
	if (pBossHpBar)
	{
		UIObj_HpBar* pProgressBar = dynamic_cast<UIObj_HpBar*>(pBossHpBar);
		if (pProgressBar)
		{
			pProgressBar->Set_CurrentFloat(m_pMonster->Get_CurrentHp());
			pProgressBar->Set_MaxFloat(m_pMonster->Get_MaxHp());

			pProgressBar->Init_Ratio(true, 1.f);
			pProgressBar->Init_Ratio(false, 1.f);

			
			pBossHpBar->Set_Active(false,false);

			pProgressBar->Set_Owner_ObjectID(m_pMonster->Get_ObjectID());
			pProgressBar->Set_NameTex("Textures/Oliver_Colins_Name");
		}
	}


}

void Client::Monster_Boss_Intro::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;

	if (m_bIntroPlayed == false)
	{
		if (m_pMonster->Get_DistanceSq() <= m_pMonster->Get_DetectRangeSq())
		{
			m_bIntroPlayed = true;

			// 인트로 애니메이션 / 상태 세팅
			m_pMonster->Get_Model()->Set_Animation_CS(0, false);
			m_pMonster->Set_SuperArmor(true);
		}

		return;
	}

	//인트로 연출 진행
	m_AppearUIAlarm.Update(fTimeDelta);

	//if (m_pMonster->Get_Model()->Is_AnimFinished())
	//{
		_uint iCurAnimIndex = m_pMonster->Get_Model()->Get_CurrentAnimationIndex();

		//if (iCurAnimIndex == 20)
		//{
		//	m_pMonster->Get_Model()->Set_Animation_CS(21, false);
		//}
		if (iCurAnimIndex == 0)
		{
			m_pGameInstance->Play_Sound("Battle_Boss_BGM", 0.f, true);
			m_pMonster->Get_Model()->Set_Animation_CS(22, true, m_fAnimSpeed);
			// 알람 on
			m_AppearUIAlarm.On();

		}
	//}
}

Monster_Boss_Intro* Client::Monster_Boss_Intro::Create(GameObject* pOwner, _float fSecond, _float fSpeed)
{
	Monster_Boss_Intro* pInstance = new Monster_Boss_Intro();

	if (FAILED(pInstance->Initialize(pOwner, fSecond, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Boss_Intro");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Boss_Intro::Exit_State()
{
}

void Client::Monster_Boss_Intro::Free()
{
	__super::Free();

}
