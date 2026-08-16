#include "Client_Define.h"
#include "Skill_StrikeRiser.h"
#include "Player_MasterRig.h"
#include "Player_Stat.h"

Client::Skill_StrikeRiser::Skill_StrikeRiser()
{
}

HRESULT Client::Skill_StrikeRiser::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_StrikeRiser::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);
}

void Client::Skill_StrikeRiser::Update_Skill(_float fTimeDelta)
{

}

void Client::Skill_StrikeRiser::Exit_Skill()
{

}

void Client::Skill_StrikeRiser::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	m_pPlayerStatCom->Set_DamageMultiplier(1.1f);	//공격력 1.1배 적용하기
	__super::Start_Duration();
	
}

void Client::Skill_StrikeRiser::On_DurationEnd()
{
	m_pPlayerStatCom->Set_DamageMultiplier(1.0f);
}

Skill_StrikeRiser* Client::Skill_StrikeRiser::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_StrikeRiser* pInstance = new Skill_StrikeRiser();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_StrikeRiser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_StrikeRiser::Free()
{
	__super::Free();
}
