#include "Client_Define.h"
#include "Yakumo_GiftHeal.h"
#include "Yakumo.h"
#include "Player_Stat.h"
#include "Yakumo_DamageBlow.h"
#include "PoolingManager.h"
#include "Homing_Heal.h"
#include "ParticleSystem.h"

Client::Yakumo_GiftHeal::Yakumo_GiftHeal()
{
}

HRESULT Client::Yakumo_GiftHeal::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_GiftHeal::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	m_pYakumo->Set_Animation(38, false);

}

void Client::Yakumo_GiftHeal::Update_State(_float fTimeDelta)
{
	auto& tRunTimeEvent = m_pYakumo->Get_RunTimeEvent();
	auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();

	if (m_pYakumo->Get_Model()->Is_AnimFinished())
	{
		m_pStateMachine->Change_State(Yakumo::IDLE);
		return;
	}

	if (!tRunTimeEvent.bCanMove())
		m_pYakumo->Set_ApplyTranslation(false);

	if (tRunTimeEvent.bInputArea()) //입력이벤트를(AI를 평가로 사용)
	{
		m_pYakumo->Evaluate_AI();
	}

	if (!tAICommandDesc.bHasCommand)
		return;

	if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::SPECIALATTACK)
	{
		m_pStateMachine->Change_State(Yakumo::SPECIALATTACK);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKSTRONG)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKSTRONG);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKNORMAL)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKNORMAL);
		return;
	}
	else if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::ROLL)
	{
		m_pStateMachine->Change_State(Yakumo::ROLL);
		return;
	}
	else if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::BACKSTEP)
	{
		m_pStateMachine->Change_State(Yakumo::BACKSTEP);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::DASH)
	{
		m_pStateMachine->Change_State(Yakumo::DASH);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::RUN)
	{
		m_pStateMachine->Change_State(Yakumo::RUN);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::WALK)
	{
		m_pStateMachine->Change_State(Yakumo::WALK);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::IDLE)
	{
		m_pStateMachine->Change_State(Yakumo::IDLE);
		return;
	}


}

void Client::Yakumo_GiftHeal::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

void Client::Yakumo_GiftHeal::On_ProjectileEvent()
{
	Homing_Heal::HEAL_DESC Desc = {};

	_matrix matLeftHand = XMLoadFloat4x4(m_pYakumo->Get_LeftHandMatrix());
	_matrix matWorld = m_pYakumo->Get_Transform()->Get_WorldMatrix();
	_matrix matFinal = matLeftHand * matWorld;

	Desc.vPosition.x = XMVectorGetX(matFinal.r[3]);
	Desc.vPosition.y = XMVectorGetY(matFinal.r[3]);
	Desc.vPosition.z = XMVectorGetZ(matFinal.r[3]);

	Desc.vPosition.w = 1.f;
	_vector vDir = m_pYakumo->Get_AITargetInfo().vDirToPlayer;
	XMStoreFloat3(&Desc.vDir, vDir);

	Desc.iOwnerID = m_pYakumo->Get_ObjectID();
	// 회복량
	Desc.fHealAmount = 400.f;

	// 투사체 기본값
	Desc.fAttackRadius = 0.5f; 
	Desc.fSpeed = 20.f;
	Desc.fLifeTime = 10.f; 
	Desc.bPiercing = false;
	Desc.fAttackDamage = 0.f;
	Desc.vDebugColor = _float3(0.f, 1.f, 0.3f);
	Desc.eSpanwEffectID = POOL_ID::EFFECT_YAKUMO_HEAL;

	// 유도 파라미터
	Desc.fHomingTurnSpeed = 180.f;   // 초당 회전 각도
	Desc.fHomingDelay = 0.f;         // 유도 시작 시간
	Desc.fHomingDuration = 999.f;    // 수명 끝까지 유도
	Desc.fAcceleration = 0.f;        // 가속도
	Desc.fMaxSpeed = 0.f;            // 최고 속도(가속도 기반일때) (0이면 무제한)
	Desc.fMinSpeed = 0.f;			 // 최저 속도(가속도 기반할때)
	Desc.fHomingLosAngle = 360.f;    // 유도 범위(시야각 느낌)
	Desc.fArrivalRadius = 0.f;		 // 유도를 멈출 범위
	Desc.bPredictTarget = false;	 // 이동 경로 예측(아직 안 만듬)
	Desc.vTargetOffset = { 0.f, 1.0f, 0.f }; // 플레이어 몸통 높이 보정

	GameObject* pBullet = PoolingManager::Get_Instance()->Acquire(POOL_ID::HOMING_HEAL, &Desc);

}

Yakumo_GiftHeal* Client::Yakumo_GiftHeal::Create(GameObject* pOwner)
{
	Yakumo_GiftHeal* pInstance = new Yakumo_GiftHeal();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_GiftHeal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_GiftHeal::Free()
{
	__super::Free();
}
