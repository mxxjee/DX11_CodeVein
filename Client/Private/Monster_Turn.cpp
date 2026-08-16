#include "Client_Define.h"
#include "Monster_Turn.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Turn::Monster_Turn()
{
}

HRESULT Client::Monster_Turn::Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster ==nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Turn::Enter_State()
{
	if (m_pMonster == nullptr)
		return;

	m_fTimeAcc = 0.f;

	_vector vLook = XMVector3Normalize(m_pMonsterTransformCom->Get_State(DIRECTION::LOOK));
	_vector vRight = XMVector3Normalize(m_pMonsterTransformCom->Get_State(DIRECTION::RIGHT));

	_float3 vDirF3 = m_pMonster->Get_DirectionNorm();
	_vector vTargetDir = XMLoadFloat3(&vDirF3);

	if (XMVectorGetX(XMVector3LengthSq(vTargetDir)) > 0.0001f)
	{
		_float fAngleDegree = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenVectors(vLook, vTargetDir)));

		_float fDotRight = XMVectorGetX(XMVector3Dot(vRight, vTargetDir));
		_bool bIsRight = (fDotRight > 0.f);

		_uint iAnimIndex = 11;

		if (fAngleDegree < 91.f)
		{
			iAnimIndex = bIsRight ? 11 : 12;
		}
		else
		{
			iAnimIndex = bIsRight ? 13 : 14;
		}

		m_pMonster->Get_Model()->Set_Animation_CS(iAnimIndex, false, 0.2f, m_fAnimSpeed);

	}
	else
	{
		m_pMonster->Get_Model()->Set_Animation_CS(11, false, 0.2f, m_fAnimSpeed);
	}

	//COUT("MonsterState::TURN");
}

void Client::Monster_Turn::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr || m_pMonsterTransformCom == nullptr)
		return;

	_vector vLook = XMVector3Normalize(m_pMonsterTransformCom->Get_State(DIRECTION::LOOK));
	_float3 vTargetDirF3 = m_pMonster->Get_DirectionNorm();
	_vector vTargetDir = XMLoadFloat3(&vTargetDirF3);

	_float fAngleDegree = XMConvertToDegrees(XMVectorGetX(XMVector3AngleBetweenVectors(vLook, vTargetDir)));

	_bool bIsAnimFinished = m_pMonster->Get_Model()->Is_AnimFinished();

	if (fAngleDegree < 10.f || bIsAnimFinished)
	{
		m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());
		return;
	}

}

void Client::Monster_Turn::Exit_State()
{
}

Monster_Turn* Client::Monster_Turn::Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Turn* pInstance = new Monster_Turn();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Turn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Turn::Free()
{
	__super::Free();
}
