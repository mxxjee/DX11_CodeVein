#include "MT_Defines.h"
#include "TriggerBox.h"
#include "GameInstance.h"
#include "Level_Main.h"

CTriggerBox::CTriggerBox(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : MapObject(pD, pC)
{
}

CTriggerBox::CTriggerBox(const CTriggerBox& original)
    : MapObject(original)
{
}

HRESULT CTriggerBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTriggerBox::Initialize(void* arg)
{
    if (FAILED(__super::Initialize(arg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    static _uint namenum = 0;
    m_wstrName = L"TriggerBox_" + to_wstring(namenum++);

    m_eObjType = OBJTYPE::TYPE_MAP;

    return S_OK;
}

_int CTriggerBox::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

HRESULT CTriggerBox::Ready_Components()
{   

    return S_OK;
}

_int CTriggerBox::Update_Parallel(const _float fTimeDelta)
{
    __super::Update_Parallel(fTimeDelta);

    return 0;
}

_int CTriggerBox::Update(const _float fDT)
{
    if (__super::Update(fDT) < 0) return -1;

    if (m_bIsTriggered) return 0;

    GameObject* pPlayer = m_pGameInstance->Get_GameObject(UINT(LEVEL::TOOL), L"Layer_Player", L"Player_0");


    return 0;
}

_int CTriggerBox::Update_Late(const _float fDT)
{
    //디버깅모드일때 자기자신을 그리는 코드정도만
    //Add_RenderGroup(RENDER_GROUP::NONBLEND);
    __super::Update_Late(fDT);

    return 0;
}

HRESULT CTriggerBox::Render(const _float fDT)
{

    return S_OK;
}

CTriggerBox* CTriggerBox::Create(ID3D11Device* pD, ID3D11DeviceContext* pC)
{
    CTriggerBox* pInstance = new CTriggerBox(pD, pC);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* CTriggerBox::Clone(void* arg)
{
    CTriggerBox* pInstance = new CTriggerBox(*this);
    if (FAILED(pInstance->Initialize(arg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CTriggerBox::Free()
{
    __super::Free();
}

