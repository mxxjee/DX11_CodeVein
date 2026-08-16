#include "Client_Define.h"
#include "Skill_IndraCoil.h"
#include "Player_Stat.h"
#include "Player_MasterRig.h"
#include "IndraCoil_Thunder.h"
#include "Player.h"
#include "PoolingManager.h"
#include "ParticleSystem.h"

Client::Skill_IndraCoil::Skill_IndraCoil()
{
}

HRESULT Client::Skill_IndraCoil::Initialize(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	__super::Initialize(pPlayer, pMasterRig, pArg);

	return S_OK;
}

void Client::Skill_IndraCoil::Enter_Skill()
{
	m_pPlayerMasterRig->Set_Animation(m_tSkillDesc.iAnimIndex, m_tSkillDesc.bIsLoop);
}

void Client::Skill_IndraCoil::Update_Skill(_float fTimeDelta)
{
	if (!m_bFire)
		return;

	m_fElapsedTime += fTimeDelta;

	while (m_iFireCount < 3 && m_fElapsedTime >= m_fDelayTime[m_iFireCount])
	{
		Create_IndraCoilThunder();
		++m_iFireCount;
	}

	if (m_iFireCount >= 3)
		m_bFire = false;
}

void Client::Skill_IndraCoil::Exit_Skill()
{
	m_bFire = false;
	m_fElapsedTime = 0.f;
	m_iFireCount = 0;
}

void Client::Skill_IndraCoil::On_StartKetsugi()
{
	m_pPlayerStatCom->Apply_Myeonghyeol(-m_tSkillDesc.fMyeonghyeolCost);
	__super::Start_Cooldown();
}

void Client::Skill_IndraCoil::On_SkillProjectileEvent()
{
	m_bFire = true;
	m_fElapsedTime = 0.f;
	m_iFireCount = 0;
}

void Client::Skill_IndraCoil::Create_IndraCoilThunder()
{
	//이거 이펙트랑 같이 타이밍 맞춰야겠는데 
	m_pGameInstance->Play_Sound("SE_Renketsu_IndraCoil_000", 0.2f);
	m_pGameInstance->Play_Sound("SE_Renketsu_IndraCoil_002", 0.2f);


	IndraCoil_Thunder::INDRACOILTHUNDER_DESC Desc;

	_vector vSpawnPos = {};

	if (m_pPlayer->Get_PlayerInputDesc().bLockOn && m_pPlayer->Get_LockOnTarget() != nullptr) //락온일때는 해당 상대방의 위치에서 생성
	{
		vSpawnPos = m_pPlayer->Get_LockOnTarget()->Get_Position();
	}
	else
	{
		_vector vLook = XMVector3Normalize(m_pPlayerTransformCom->Get_State(DIRECTION::LOOK));

		vSpawnPos = XMVectorAdd(m_pPlayer->Get_Position(), XMVectorScale(vLook, 4.f)); //바라보는 방향 기준 5f 거리 앞에
	}

	_float2 vXOffset = { -2.f,2.f };
	_float2 vZOffset = { -2.f,2.f };

	_float vRandomX = m_pGameInstance->RandomValue(vXOffset.x, vXOffset.y);
	_float vRandomZ = m_pGameInstance->RandomValue(vZOffset.x, vZOffset.y);

	Desc.vPosition.x = XMVectorGetX(vSpawnPos) + vRandomX;
	Desc.vPosition.y = XMVectorGetY(vSpawnPos);
	Desc.vPosition.z = XMVectorGetZ(vSpawnPos) + vRandomZ;
	Desc.vPosition.w = 1.f;

	Desc.fAttackRadius = 1.0f;
	Desc.fLifeTime = 1.0f;

	GameObject* pThunder = PoolingManager::Get_Instance()->Acquire(POOL_ID::INDRACOIL_THUNDER, &Desc);

	// Effect
	ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire_Effect(POOL_ID::EFFECT_SKILL_IC_THUNDER));
	if (pSystem == nullptr)
		return;

	pSystem->Set_WorldMatrix(pThunder->Get_WorldFloat4x4());
	pSystem->Play();
}

Skill_IndraCoil* Client::Skill_IndraCoil::Create(Player* pPlayer, Player_MasterRig* pMasterRig, void* pArg)
{
	Skill_IndraCoil* pInstance = new Skill_IndraCoil();

	if (FAILED(pInstance->Initialize(pPlayer, pMasterRig, pArg)))
	{
		MSG_BOX("Failed to Created : Skill_IndraCoil");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Skill_IndraCoil::Free()
{
	__super::Free();
}
