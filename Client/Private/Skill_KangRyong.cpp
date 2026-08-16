#include "Client_Define.h"
#include "Skill_KangRyong.h"
#include "Player_Stat.h"
#include "Player_MasterRig.h"

Client::Skill_KangRyong::Skill_KangRyong()
{
}

HRESULT Client::Skill_KangRyong::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_KangRyong::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);

}

void Client::Skill_KangRyong::Update_Skill(_float fTimeDelta)
{

}

void Client::Skill_KangRyong::Exit_Skill()
{

}

void Client::Skill_KangRyong::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	__super::Start_Cooldown();

	//UI 이벤트를 쏴줘야하는거고
}

Skill_KangRyong* Client::Skill_KangRyong::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_KangRyong* pInstance = new Skill_KangRyong();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_KangRyong");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_KangRyong::Free()
{
	__super::Free();
}
