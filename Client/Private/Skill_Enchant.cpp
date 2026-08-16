#include "Client_Define.h"
#include "Skill_Enchant.h"
#include "Player_Stat.h"
#include "Player_MasterRig.h"

Client::Skill_Enchant::Skill_Enchant()
{
}

HRESULT Client::Skill_Enchant::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_Enchant::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);
}

void Client::Skill_Enchant::Update_Skill(_float fTimeDelta)
{
}

void Client::Skill_Enchant::Exit_Skill()
{
}

void Client::Skill_Enchant::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	m_pPlayerStatCom->Set_Enchant(true, _float2(10.f, 50.f));

	__super::Start_Duration();
}

void Client::Skill_Enchant::On_DurationEnd()
{
	m_pPlayerStatCom->Set_Enchant(false, _float2(0.f, 0.f));
}

Skill_Enchant* Client::Skill_Enchant::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_Enchant* pInstance = new Skill_Enchant();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_Enchant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_Enchant::Free()
{
	__super::Free();
}
