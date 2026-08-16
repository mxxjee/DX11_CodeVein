#include "ShaderTool_Define.h"
#include "Monster.h"
#include "Monster_Body.h"
#include "GameInstance.h"

Monster::Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : ContainerObject{ pDevice, pContext }
{
}

Monster::Monster(const Monster& Prototype)
    : ContainerObject(Prototype)
{
}

HRESULT Monster::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT Monster::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_eObjType = OBJTYPE::TYPE_CONTAINER;

    m_pTransformCom->Set_State(DIRECTION::POSITION, XMVectorSet(3.f, 4.f, 0.f, 1.f));
    return S_OK;
}

_int Monster::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Monster::Update(_float fTimeDelta)
{
    if (GetKeyState(VK_LEFT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
    }
    if (GetKeyState(VK_RIGHT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
    }
    if (GetKeyState(VK_DOWN) & 0x8000)
    {
        m_pTransformCom->Move_Backward(fTimeDelta);
    }

    if (GetKeyState(VK_UP) & 0x8000)
    {
        m_pTransformCom->Move_Forward(fTimeDelta);

        if (m_iState & IDLE)
            m_iState ^= IDLE;

        m_iState |= RUN;
    }
    else
    {
        if (m_iState & RUN)
            m_iState ^= RUN;

        m_iState |= IDLE;
    }
    //임시로 카메라 따라오게?
    _float4 temp = m_pGameInstance->Get_CameraPosition();
    m_pGameInstance->Follow_ShadowCascade_Matrix(XMLoadFloat4(&temp)); //m_pTransformCom->Get_State(DIRECTION::POSITION)
    __super::Update(fTimeDelta);
    return 0;
}

_int Monster::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);


    m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this);
    return 0;
}

HRESULT Monster::Render(const _float fTimeDelta)
{
    return S_OK;
}

HRESULT Monster::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{

    return S_OK;
}

HRESULT Monster::Ready_Components()
{
    
    return S_OK;
}

HRESULT Monster::Ready_PartObjects()
{

    return S_OK;
}


HRESULT Monster::Bind_ShaderResources()
{
    return S_OK;
}

Monster* Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    Monster* pInstance = new Monster(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(_level)))
    {
        MSG_BOX("Failed to Created : Monster");
        Safe_Release(pInstance);
    }
    return pInstance;
}

GameObject* Monster::Clone(void* pArg)
{
    Monster* pInstance = new Monster(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Monster");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void Monster::Free()
{
    __super::Free();
}
