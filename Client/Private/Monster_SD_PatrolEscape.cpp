#include "Client_Define.h"
#include "Monster_SD_PatrolEscape.h"
#include "GameObject.h"
#include "Monster.h"

Client::Monster_SD_PatrolEscape::Monster_SD_PatrolEscape()
{
}

HRESULT Client::Monster_SD_PatrolEscape::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
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

void Client::Monster_SD_PatrolEscape::Enter_State()
{
	if (m_pMonster == nullptr)
		return;

	m_pMonster->Set_Speed(m_pMonster->Get_WalkSpeed());

	auto* ActiveWeapon = m_pMonster->Get_ActiveMonsterWeapon();
	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, true, 0.2f, m_fAnimSpeed);

	vStartPos = m_pMonster->Get_Position();
	m_vEscapeDir = m_pMonster->Get_Transform()->Get_State(DIRECTION::LOOK);

	m_vEscapeDir = XMVector3Normalize(m_vEscapeDir);
	m_pMonster->Set_NavDir(m_vEscapeDir);
}

void Client::Monster_SD_PatrolEscape::Update_State(_float fTimeDelta)
{
	
	// 시작점으로부터 1m 이상 멀어졌는지 체크
	m_pMonster->Set_NavDir(m_vEscapeDir);
	_vector vCurrentPos = m_pMonster->Get_Position();
	_vector vDiff = vCurrentPos - vStartPos;

	_vector vDist = XMVector3Length(vDiff);
	_float fDist= XMVectorGetX(vDist);

	if (fDist >= 1.0f)
	{
		m_pStateMachine->Change_State(Monster::IDLE);
	}
}

void Client::Monster_SD_PatrolEscape::Exit_State()
{
}
Monster_SD_PatrolEscape* Client::Monster_SD_PatrolEscape::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_SD_PatrolEscape* pInstance = new Monster_SD_PatrolEscape();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_SD_PatrolEscape");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void Client::Monster_SD_PatrolEscape::Free()
{
	__super::Free();
}
