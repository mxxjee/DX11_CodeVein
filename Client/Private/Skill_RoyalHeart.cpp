#include "Client_Define.h"
#include "Skill_RoyalHeart.h"
#include "Player_Stat.h"
#include "Player_MasterRig.h"

Client::Skill_RoyalHeart::Skill_RoyalHeart()
{
}

HRESULT Client::Skill_RoyalHeart::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_RoyalHeart::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);
}

void Client::Skill_RoyalHeart::Update_Skill(_float fTimeDelta)
{
}

void Client::Skill_RoyalHeart::Exit_Skill()
{
}

void Client::Skill_RoyalHeart::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	m_pPlayerStatCom->Set_RoyalHeart(true, 0.33f);
	__super::Start_Duration();
}

void Client::Skill_RoyalHeart::On_DurationEnd()
{
	m_pPlayerStatCom->Set_RoyalHeart(false, 0.f);
}

Skill_RoyalHeart* Client::Skill_RoyalHeart::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_RoyalHeart* pInstance = new Skill_RoyalHeart();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_RoyalHeart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_RoyalHeart::Free()
{
	__super::Free();
}
