#include "Client_Define.h"
#include "Monster_Hit.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Hit::Monster_Hit()
{
}


HRESULT Client::Monster_Hit::Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}



void Client::Monster_Hit::Enter_State()
{

	_vector vLook = XMVector3Normalize(m_pMonsterTransformCom->Get_State(DIRECTION::LOOK));
	_vector vRight = XMVector3Normalize(m_pMonsterTransformCom->Get_State(DIRECTION::RIGHT));

	_float3 vDir = m_pMonster->Get_DirectionNorm();
	_vector vToPlayer = XMLoadFloat3(&vDir);

	_float fForwardDot = XMVectorGetX(XMVector3Dot(vLook, vToPlayer)); // 전후 판정
	_float fRightDot = XMVectorGetX(XMVector3Dot(vRight, vToPlayer)); // 좌우 판정

	_uint iAnimIndex = FLIdx; 

	if (fForwardDot >= 0.f) 
	{
		iAnimIndex = (fRightDot >= 0.f) ? FRIdx : FLIdx;
	}
	else 
	{
		iAnimIndex = (fRightDot >= 0.f) ? BRIdx : BLIdx;
	}

	m_pMonster->Get_Model()->Set_Animation_CS(iAnimIndex, false, 0.1f, m_fAnimSpeed);
	//COUT("MonsterState::HIT"<<"AnimIdx:"<<iAnimIndex);
}

void Client::Monster_Hit::Update_State(_float fTimeDelta)
{
	if (m_pMonster->Get_Model()->Is_AnimFinished())
	{
		//m_pMonster->LookAt_Target();
		m_pStateMachine->Change_State(Monster::IDLE);
	}
}

void Client::Monster_Hit::Exit_State()
{
}

Monster_Hit* Client::Monster_Hit::Create(GameObject* pOwner, _uint FL, _uint FR, _uint BL, _uint BR, _float fSpeed)
{
	Monster_Hit* pInstance = new Monster_Hit();

	if (FAILED(pInstance->Initialize(pOwner, 0, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Hit");
		Safe_Release(pInstance);
	}

	/*Initliaze바꾸기 뭐해서 여기서 캐싱*/
	pInstance->FLIdx = FL;
	pInstance->FRIdx = FR;
	pInstance->BLIdx = BL;
	pInstance->BRIdx = BR;

	return pInstance;
}

void Client::Monster_Hit::Free()
{
	__super::Free();

}
