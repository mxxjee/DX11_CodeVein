#include "Engine_Define.h"
#include "Player_Stat.h"
#include "GameInstance.h"

Player_Stat::Player_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Stat(pDevice,pContext)
{
}

Player_Stat::Player_Stat(const Player_Stat& original)
	:Stat(original)
{
}

void Player_Stat::Decrease_Stamina(_float fStamina)
{
	m_PlayerStat.fCurrentStamina = clamp(m_PlayerStat.fCurrentStamina- fStamina, 0.f, m_PlayerStat.fMaxStamina);
}

void Engine::Player_Stat::ContinuousDecrease_Stamina(_float fDecrasePerSecond, _float fTimeDelta)
{
	m_PlayerStat.fCurrentStamina -= fDecrasePerSecond * fTimeDelta;

	m_PlayerStat.fCurrentStamina = clamp(m_PlayerStat.fCurrentStamina, 0.f, m_PlayerStat.fMaxStamina);
}

void Engine::Player_Stat::Regen_Stamina(_float fRegenPerSecond, _float fTimeDelta)
{
	if (m_PlayerStat.fCurrentStamina < m_PlayerStat.fMaxStamina)
	{
		m_PlayerStat.fCurrentStamina += fTimeDelta * fRegenPerSecond;
	}
}

void Engine::Player_Stat::FullRestore_Stamina()
{
	m_PlayerStat.fCurrentStamina = m_PlayerStat.fMaxStamina;
}

void Player_Stat::Apply_Myeonghyeol(_float iMyeonghyeol)
{
	m_PlayerStat.iCurrentMyeonghyeol = clamp<_float>(m_PlayerStat.iCurrentMyeonghyeol + iMyeonghyeol, 0, m_PlayerStat.iMaxMyeonghyeol);


	/*이벤트발송!(명혈 UI갱신)*/
	ManaUIEvent Event;
	Event.eType = ManaUIEventType::UPDATE_CURRENTMANA;
	Event.fValue = m_PlayerStat.iCurrentMyeonghyeol;
	m_pGameInstance->Publish(Event);
}

void Engine::Player_Stat::Update_UIEvent()
{
	if (m_PlayerStat.fCurrentStamina < m_PlayerStat.fLowStamina) 	//일정스테미나 이하면 빨간색깜빡깜빢스 실행 ㄱㄱ
	{
		UI_MasterEvent	Event;
		Event.m_ActionName = "OnStaminaLow";
		Event.m_EventTarget = UI_EVENT_TARGET::UIOBJECT;
		Event.m_ActionStrHash = hash<string>{}(Event.m_ActionName);
		m_pGameInstance->Publish(Event);
	}

	if (m_PlayerStat.fCurrentStamina >= m_PlayerStat.fLowStamina)
	{
		UI_MasterEvent	Event;
		Event.m_ActionName = "OnStaminaRecovered";
		Event.m_EventTarget = UI_EVENT_TARGET::UIOBJECT;
		Event.m_ActionStrHash = hash<string>{}(Event.m_ActionName);
		GameInstance::GetInstance()->Publish(Event);

	}
}

_bool Engine::Player_Stat::Check_RoyalHeartSuperArmor()
{
	if (!m_bRoyalHeartActive)
		return false;

	_float fRand = static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX); //0~1

	return fRand <= m_fRoyalHeartChance; //로얄하트확률보다 낮으면 성공 (0.66)로 줄예정 , 3분의2로 성공 / 0.66너무 잘터진다 0.33으로 수정 
}

_float Engine::Player_Stat::Apply_EnchantDamage()
{
	if (!m_bEnchantActive)
		return 0.f;

	_float fAdditionalDamage = m_pGameInstance->RandomValue(m_vAdditionalDamage.x, m_vAdditionalDamage.y);

	return fAdditionalDamage;
}

HRESULT Player_Stat::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Player_Stat::Initialize(void* arg)
{
	__super::Initialize(arg);
	//공용으로 사용하는 스탯들은 부모에게 전달 , 자식 구조체들만 값 담아주기

	PLAYERSTAT_DESC* Desc = static_cast<PLAYERSTAT_DESC*>(arg);


	m_PlayerStat.fCurrentStamina = Desc->fCurrentStamina;
	m_PlayerStat.fMaxStamina = Desc->fMaxStamina;
	m_PlayerStat.fLowStamina = Desc->fLowStamina;
	m_PlayerStat.eBloodCode = Desc->eBloodCode;
	m_PlayerStat.iCurrentMyeonghyeol = Desc->iCurrentMyeonghyeol;
	m_PlayerStat.iMaxMyeonghyeol = Desc->iMaxMyeonghyeol;

	m_PlayerStat.iSTR = Desc->iSTR;
	m_PlayerStat.iDEX = Desc->iDEX;
	m_PlayerStat.iMND = Desc->iMND;
	m_PlayerStat.iWIL = Desc->iWIL;
	m_PlayerStat.iVIT = Desc->iVIT;
	m_PlayerStat.iFOR = Desc->iFOR;

	m_PlayerStat.iLevel = Desc->iLevel;


	return S_OK;
}

void Engine::Player_Stat::LevelUp(_uint iLevel, _uint iMaxHp, _uint iMaxST, _uint iAttack)
{
	m_PlayerStat.iLevel = iLevel;
	m_PlayerStat.iAttack = iAttack;

	//다시 max값으로 회복
	m_Stat.fCurrentHp = (_float)iMaxHp;
	m_Stat.fMaxHp = (_float)iMaxHp;

	m_PlayerStat.fMaxStamina = (_float)iMaxST;
	m_PlayerStat.fCurrentStamina = (_float)iMaxST;

}

Player_Stat* Player_Stat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Player_Stat* pInstance = new Player_Stat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Player_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component* Engine::Player_Stat::Clone(void* pArg)
{
	Player_Stat* pInstance = new Player_Stat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Player_Stat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Player_Stat::Free()
{
	__super::Free();
}
