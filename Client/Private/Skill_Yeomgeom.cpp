#include "Client_Define.h"
#include "Skill_Yeomgeom.h"
#include "Player_Stat.h"
#include "Player_MasterRig.h"
#include "Player.h"

Client::Skill_Yeomgeom::Skill_Yeomgeom()
{
}

HRESULT Client::Skill_Yeomgeom::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_Yeomgeom::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);
}

void Client::Skill_Yeomgeom::Update_Skill(_float fTimeDelta)
{
}

void Client::Skill_Yeomgeom::Exit_Skill()
{
}

void Client::Skill_Yeomgeom::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	m_pPlayer->Set_ActiveYeomgeom(true);
	__super::Start_Duration();
}

void Client::Skill_Yeomgeom::On_DurationEnd()
{
	m_pPlayer->Set_ActiveYeomgeom(false);
}

Skill_Yeomgeom* Client::Skill_Yeomgeom::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_Yeomgeom* pInstance = new Skill_Yeomgeom();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_Yeomgeom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_Yeomgeom::Free()
{
	__super::Free();
}
