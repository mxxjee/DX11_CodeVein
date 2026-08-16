#include "Client_Define.h"
#include "Skill_RadiantBarrel.h"
#include "Player_Stat.h"
#include "Player_MasterRig.h"
#include "Bayonet_Bullet.h"
#include "Player.h"
#include "PoolingManager.h"

Client::Skill_RadiantBarrel::Skill_RadiantBarrel()
{
}

HRESULT Client::Skill_RadiantBarrel::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_RadiantBarrel::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);
}

void Client::Skill_RadiantBarrel::Update_Skill(_float fTimeDelta)
{
}

void Client::Skill_RadiantBarrel::Exit_Skill()
{
}

void Client::Skill_RadiantBarrel::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	__super::Start_Cooldown();
}

void Client::Skill_RadiantBarrel::On_SkillProjectileEvent()
{
	Bayonet_Bullet::BAYONETBULLET_DESC Desc = {};

	Desc.vPosition.x = m_pPlayer->Get_BayonetMuzzleMatrixPtr()->_41;
	Desc.vPosition.y = m_pPlayer->Get_BayonetMuzzleMatrixPtr()->_42;
	Desc.vPosition.z = m_pPlayer->Get_BayonetMuzzleMatrixPtr()->_43;
	Desc.vPosition.w = 1.f;
	Desc.bPiercing = true;
	Desc.fSpeed = 60.f;
	Desc.iMaxHitCount = 0;
	Desc.fAttackDamage = 1000.f;
	Desc.fLifeTime = 1.f;
	Desc.fAttackRadius = 0.2f;

	Desc.vDir = m_pPlayer->Get_Transform()->Get_Look_Float3();

	GameObject* pBullet = PoolingManager::Get_Instance()->Acquire(POOL_ID::BAYONET_BULLET, &Desc);
}

Skill_RadiantBarrel* Client::Skill_RadiantBarrel::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_RadiantBarrel* pInstance = new Skill_RadiantBarrel();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_RadiantBarrel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_RadiantBarrel::Free()
{
	__super::Free();
}
