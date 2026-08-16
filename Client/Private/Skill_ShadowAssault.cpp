#include "Client_Define.h"
#include "Skill_ShadowAssault.h"
#include "Player_Stat.h"
#include "Player_MasterRig.h"

Client::Skill_ShadowAssault::Skill_ShadowAssault()
{
}

HRESULT Client::Skill_ShadowAssault::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_ShadowAssault::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);
}

void Client::Skill_ShadowAssault::Update_Skill(_float fTimeDelta)
{
}

void Client::Skill_ShadowAssault::Exit_Skill()
{
}

void Client::Skill_ShadowAssault::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	__super::Start_Cooldown();
}

Skill_ShadowAssault* Client::Skill_ShadowAssault::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_ShadowAssault* pInstance = new Skill_ShadowAssault();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_ShadowAssault");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_ShadowAssault::Free()
{
	__super::Free();
}
