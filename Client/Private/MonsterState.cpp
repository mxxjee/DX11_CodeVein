#include "Client_Define.h"
#include "MonsterState.h"

#include "Monster.h"

Client::MonsterState::MonsterState()
    :State()
{
}

HRESULT Client::MonsterState::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
    CHECK_FAILED(__super::Initialize(pOwner),E_FAIL);

    m_iAnimIdx = iAnimIdx;
    m_fAnimSpeed = fSpeed;

    return S_OK;
}

void Client::MonsterState::Free()
{
    __super::Free();

}
