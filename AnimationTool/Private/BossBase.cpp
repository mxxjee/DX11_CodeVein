#include "AnimationTool_Define.h"
#include "BossBase.h"

BossBase::BossBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Monster(pDevice, pContext)
{
}

BossBase::BossBase(const BossBase& original)
    : Monster(original)
    , m_iCurrentPhase(original.m_iCurrentPhase)
    , m_fMaxGroggy(original.m_fMaxGroggy)
    , m_fCurrentGroggy(original.m_fCurrentGroggy)
    , m_bIsSuperArmor(original.m_bIsSuperArmor)
{
}

HRESULT BossBase::Initialize(void* _arg)
{
    if (_arg != nullptr)
    {
        MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(_arg);

        pDesc->tControllerDesc.fRadius *= 1.15f;
        pDesc->tControllerDesc.fHeight *= 1.15f;
    }

    if (FAILED(__super::Initialize(_arg)))
        return E_FAIL;

    m_iCurrentPhase = 1;
    m_fCurrentGroggy = m_fMaxGroggy;
    m_bIsSuperArmor = true;

    m_pTransformCom->Set_Scale(1.15f, 1.15f, 1.15f);

    return S_OK;
}

_int BossBase::Update(const _float fTimeDelta)
{
    _int iProgress = __super::Update(fTimeDelta);
    if (iProgress < 0) return -1;

    Check_Phase();


    return iProgress;
}

HRESULT BossBase::Ready_Components()
{
    __super::Ready_Components();

    return S_OK;
}

void BossBase::Free()
{
    __super::Free();
}
