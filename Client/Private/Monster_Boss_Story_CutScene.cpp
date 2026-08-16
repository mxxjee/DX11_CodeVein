#include "Client_Define.h"
#include "Monster_Boss_Story_CutScene.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"
#include "St01_BossOliver.h" 

Client::Monster_Boss_Story_CutScene::Monster_Boss_Story_CutScene()
{
}

HRESULT Client::Monster_Boss_Story_CutScene::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));

	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Boss_Story_CutScene::Enter_State()
{
	if (m_pMonster == nullptr) return;


}

void Client::Monster_Boss_Story_CutScene::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;

}

void Client::Monster_Boss_Story_CutScene::Exit_State()
{
	if (m_pMonster == nullptr) return;
	m_pMonster->Set_SuperArmor(false);
}

Monster_Boss_Story_CutScene* Client::Monster_Boss_Story_CutScene::Create(GameObject* pOwner)
{
	Monster_Boss_Story_CutScene* pInstance = new Monster_Boss_Story_CutScene();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Monster_Boss_Story_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Boss_Story_CutScene::Free()
{
	__super::Free();
}
