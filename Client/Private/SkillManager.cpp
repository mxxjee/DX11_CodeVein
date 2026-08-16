#include "Client_Define.h"
#include "SkillManager.h"
#include "GameInstance.h"
#include "Skill_Base.h"

#include "Player.h"

IMPLEMENT_SINGLETON(SkillManager);

Client::SkillManager::SkillManager()
{
}

Client::SkillManager::~SkillManager()
{
}

HRESULT Client::SkillManager::Initialize()
{
	m_pGameInstance = GameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

HRESULT Client::SkillManager::Register_Skill(_uint iSkillId, Skill_Base* pSkill)
{
	if (pSkill == nullptr)
		return E_FAIL;

	auto iter = m_umapSkills.find(iSkillId);

	if (iter != m_umapSkills.end()) //등록되어있으면 E_FAIL;
	{
		return E_FAIL;
	}

	m_umapSkills.emplace(iSkillId, pSkill);

	return S_OK;
}

Skill_Base* Client::SkillManager::Find_Skill(_uint iSkillId)
{
	auto iter = m_umapSkills.find(iSkillId);

	if (iter == m_umapSkills.end())
		return nullptr;

	return iter->second;
}

HRESULT Client::SkillManager::Equip_SkillToSlot(_uint iSlotId, _uint iSkillId)
{
	Skill_Base* pSkill = Find_Skill(iSkillId);
	if (pSkill == nullptr)
		return E_FAIL;

	m_umapEquip[iSlotId] = iSkillId;

	return S_OK;
}

Skill_Base* Client::SkillManager::Get_SkillToSlot(_uint iSlotId)
{
	auto iter = m_umapEquip.find(iSlotId);
	
	if (iter == m_umapEquip.end())
		return nullptr;
	
	return Find_Skill(iter->second);
}

void Client::SkillManager::Update_CooldownAndDuration(_float fTimeDelta)
{
	for (auto& Skill : m_umapSkills)
	{
		Skill.second->Update_Cooldown(fTimeDelta);
		Skill.second->Update_Duration(fTimeDelta);
	}
}

Skill_Base* Client::SkillManager::Requset_Skill(_uint iSlotId)
{
	Skill_Base* pSkill = Get_SkillToSlot(iSlotId);
	if (pSkill == nullptr)
		return nullptr;

	_bool bCanSkill = pSkill->Can_SkillEnter();

	if (!bCanSkill)
		return nullptr;
	
	///*스킬 UI 슬롯한테 ACTIVE EVENT보내기! - UI는 각자 자기가 점유하고있는 스킬과 대조해서 상태변경*/
	pSkill->Request_SkillUI_Active();
	if (!m_pMainPlayer)
	{
		m_pMainPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
		Safe_AddRef(m_pMainPlayer);
	}

	if (m_pMainPlayer)
		m_pMainPlayer->Set_SkillAttempt(true);

	return pSkill;
}

void Client::SkillManager::Debug_SkillReset()
{
	for (auto& Skill : m_umapSkills)
	{
		Skill.second->Set_DebugCoolAndDuration(0.f);
	}
}

void Client::SkillManager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (auto& pSkills : m_umapSkills)
	{
		Safe_Release(pSkills.second);
	}

	m_umapSkills.clear();
	m_umapEquip.clear();

	Safe_Release(m_pMainPlayer);


}