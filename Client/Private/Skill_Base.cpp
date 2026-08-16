#include "Client_Define.h"
#include "Skill_Base.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Player_Stat.h"
#include "Player_Weapon.h"
#include "GameInstance.h"
#include "ItemManager.h"
#include "UIObj_QuickSlot.h"


Client::Skill_Base::Skill_Base()
{
}

HRESULT Client::Skill_Base::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	if (pPlayer == nullptr || pMasterRig == nullptr)
		return E_FAIL;


	m_pPlayer = pPlayer;
	m_pPlayerMasterRig = pMasterRig;
	m_pPlayerStatCom = m_pPlayer->Get_PlayerStatCom();
	m_pPlayerTransformCom = m_pPlayer->Get_Transform();
	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	SKILL_DESC* pDesc = static_cast<SKILL_DESC*>(pArg);

	m_tSkillDesc = *pDesc;

	//ItemManager::GetInstance()->Create_Skill_Prototype(
	//	m_tSkillDesc.wstrSkillName, m_tSkillDesc.strTexKey, m_tSkillDesc.iItemCount, m_tSkillDesc.iMaxItemSize,
	//	m_tSkillDesc.wstrDescription,m_tSkillDesc.wstrLineageType, m_tSkillDesc.wstrSkillType, m_tSkillDesc.fMyeonghyeolCost, 
	//	m_tSkillDesc.fCooldown, m_tSkillDesc.wstrAttackType);

	return S_OK;
}

void Client::Skill_Base::Update_Skill(_float fTimeDelta)
{
}

void Client::Skill_Base::On_StartKetsugi()
{
}

void Client::Skill_Base::On_SkillProjectileEvent()
{
}

void Client::Skill_Base::On_DurationEnd()
{
}

_bool Client::Skill_Base::Can_SkillEnter() const
{
	if (m_pPlayerStatCom->Get_Myeonghyeol() < m_tSkillDesc.fMyeonghyeolCost) //플레이어의 현재 명혈이 요구하는 스킬의 명혈보다 작다면 false
	{
		return false;
	}

	if (m_fCurrentCooldown > 0.f) //쿨타임 안돌았으면 false
		return false; 

	if (m_tSkillDesc.eSkillType == SKILL_TYPE::DURATION_SKILL) //지속형들 
	{
		if (m_fSkillDurtaion > 0.f) //지속시간 길어도 못하므로
			return false; 
	}

	if (!m_tSkillDesc.vecWeaponType.empty()) //사용가능한 웨폰타입이 비어있지 않을때
	{
		Player_Weapon* pPlayerActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon(); //현재 활성화된 무기
		if (pPlayerActiveWeapon == nullptr)
			return false;

		WEAPON_TYPE eCurrentWeaponType = pPlayerActiveWeapon->Get_WeaponType(); //활성화된 무기 타입

		_bool bAllowed = false;

		for (auto& eWeaponType : m_tSkillDesc.vecWeaponType) //초기화시점에 해당 스킬의 사용가능한 웨폰타입이 무엇인지 
		{
			if (eCurrentWeaponType == eWeaponType) //현재 활성화된 무기 타입과 같다면
			{
				bAllowed = true; //스킬 허용
				break;
			}
		}

		if (!bAllowed)
			return false; 

	}



	return true;
}

void Client::Skill_Base::Start_Cooldown()
{
	m_fCurrentCooldown = m_tSkillDesc.fCooldown;

	//여기에서 UI 쿨타임 시작하라는 이벤트 쏴주기
	Request_SkillUI_StartCooldown();

	m_bStartCoolDown = true;//쿨다운 시작/끝을 알리기위한 변수
}

void Client::Skill_Base::Update_Cooldown(_float fTimeDelta)
{
	if(m_fCurrentCooldown>0.f)
		m_fCurrentCooldown -= fTimeDelta;

	if (m_bStartCoolDown && m_fCurrentCooldown <= 0.f)
	{
		m_bStartCoolDown = false;
		Request_SkillUI_EndCoolDown();
	}
}

void Client::Skill_Base::Start_Duration()
{
	m_fSkillDurtaion = m_tSkillDesc.fSkillDuration;

	m_bDurationActive = true;

	m_bDurationEnter = true;


	//Request_SkillUI_Active();

}

void Client::Skill_Base::Update_Duration(_float fTimeDelta)
{
	if (m_tSkillDesc.eSkillType == INSTANT_SKILL) //단발형들은 바로 리턴
	{
		return;

	}
	if (!m_bDurationActive)return;

	if (m_fSkillDurtaion > 0.f)
		m_fSkillDurtaion -= fTimeDelta;

	if (m_fSkillDurtaion <= 0.f) //지속시간이 끝났으면
	{
		m_bDurationActive = false;
		On_DurationEnd(); 
		Start_Cooldown();

	}
}

_float Client::Skill_Base::Get_SkillDurationRatio()
{
	if (m_tSkillDesc.fSkillDuration == 0)
		return 0;

	return m_fSkillDurtaion / m_tSkillDesc.fSkillDuration;
}


#pragma region UI Evnet함수

void Client::Skill_Base::Request_SkillUI_Active()
{
	UIObj_QuickSlot::QuickSlotEvent			Event;
	Event.m_eEventType = UIObj_QuickSlot::QuickSlotEventType::START_ACTIVE;
	Event.m_pSkillBase = this;
	m_pGameInstance->Publish(Event);

}
void Client::Skill_Base::Request_SkillUI_StartCooldown()
{
	UIObj_QuickSlot::QuickSlotEvent CoolDownEvent;
	CoolDownEvent.m_eEventType = UIObj_QuickSlot::QuickSlotEventType::START_COOLDOWN;
	CoolDownEvent.m_pSkillBase = this;
	m_pGameInstance->Publish(CoolDownEvent);
}
void Client::Skill_Base::Request_SkillUI_EndCoolDown()
{
	UIObj_QuickSlot::QuickSlotEvent CoolDownEvent;
	CoolDownEvent.m_eEventType = UIObj_QuickSlot::QuickSlotEventType::END_COOLTIME;
	CoolDownEvent.m_pSkillBase = this;
	m_pGameInstance->Publish(CoolDownEvent);
}

void Client::Skill_Base::Cancle_Request()
{
	m_bDurationActive = false;
	m_fSkillDurtaion = 0.f;
	m_fCurrentCooldown = 0.f; // 쿨타임이 돌지 않게 함
	m_bStartCoolDown = false;

	UIObj_QuickSlot::QuickSlotEvent Event;
	Event.m_eEventType = UIObj_QuickSlot::QuickSlotEventType::CANCEL_ACTIVE;
	Event.m_pSkillBase = this;
	m_pGameInstance->Publish(Event);


}

_bool Client::Skill_Base::Can_Active_Skill()
{
	if (!m_tSkillDesc.vecWeaponType.empty()) //사용가능한 웨폰타입이 비어있지 않을때
	{
		Player_Weapon* pPlayerActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon(); //현재 활성화된 무기
		if (pPlayerActiveWeapon == nullptr)
			return false;

		WEAPON_TYPE eCurrentWeaponType = pPlayerActiveWeapon->Get_WeaponType(); //활성화된 무기 타입

		_bool bAllowed = false;

		for (auto& eWeaponType : m_tSkillDesc.vecWeaponType) //초기화시점에 해당 스킬의 사용가능한 웨폰타입이 무엇인지 
		{
			if (eCurrentWeaponType == eWeaponType) //현재 활성화된 무기 타입과 같다면
			{
				bAllowed = true; //스킬 허용
				break;
			}
		}

		if (!bAllowed)
			return false;

	}

	return true;	// 26.03.23 리턴값 없길래 기본 리턴값 true로 함
}


#pragma endregion
void Client::Skill_Base::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

}