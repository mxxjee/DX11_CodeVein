#include "MT_Defines.h"
#include "NonCulObj.h"
#include "GameInstance.h"
#include "Level_Main.h"

CNonCulObj::CNonCulObj(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : MapObject(pD, pC)
{
}

CNonCulObj::CNonCulObj(const CNonCulObj& original)
    : MapObject(original)
{
}

HRESULT CNonCulObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNonCulObj::Initialize(void* arg)
{
    if (FAILED(__super::Initialize(arg)))
        return E_FAIL;

    if (nullptr != arg)
    {
        MAPOBJECT_DESC* pDesc = (MAPOBJECT_DESC*)arg;

        m_pTransformCom->Set_State(DIRECTION::POSITION, XMLoadFloat4(&pDesc->vPosition));
        m_pTransformCom->Set_Scale(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z); m_pTransformCom->Set_Rotation_By_Matrix(XMMatrixRotationQuaternion(XMLoadFloat4(&pDesc->vRotationQuat)));
        m_pProtoname = pDesc->prototypename;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eObjType = OBJTYPE::TYPE_MAP;

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_vecObjPass.push_back(0);
    }

    return S_OK;
}

HRESULT CNonCulObj::Ready_Components()
{
    _uint iLevel = (_uint)LEVEL::TOOL;

    CHECK_FAILED(Add_Component(iLevel, L"Prototype_Component_Shader_VTXMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
    CHECK_FAILED(Add_Component(iLevel, m_pProtoname, Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

    if (m_pModelCom != nullptr)
    {
        m_vecObjPass.resize(m_pModelCom->Get_NumMeshes(), 0);
    }

    Add_RenderGroup(RENDER_GROUP::PRIORITY);

    return S_OK;
}

_int CNonCulObj::Update(const _float fDT)
{
    return __super::Update(fDT);
}

_int CNonCulObj::Update_Late(const _float fDT)
{
    __super::Update_Late(fDT);

    return 0;
}

HRESULT CNonCulObj::Render(const _float fDT)
{
    if (false == m_bIsVisible)
        return S_OK;

    CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
    CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

        CHECK_FAILED(m_pModelCom->Render(fDT, i), E_FAIL);
    }

    m_bIsHovered = false;
    return S_OK;
}

CNonCulObj* CNonCulObj::Create(ID3D11Device* pD, ID3D11DeviceContext* pC)
{
    CNonCulObj* pInstance = new CNonCulObj(pD, pC);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* CNonCulObj::Clone(void* arg)
{
    CNonCulObj* pInstance = new CNonCulObj(*this);
    if (FAILED(pInstance->Initialize(arg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CNonCulObj::Free()
{
    __super::Free();
}

