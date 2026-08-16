#include "MT_Defines.h"
#include "MapSeal.h"
#include "GameInstance.h"

CMapSeal::CMapSeal(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : MapObject(pD, pC)
{
}

CMapSeal::CMapSeal(const CMapSeal& original)
    : MapObject(original)
{
}

HRESULT CMapSeal::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapSeal::Initialize(void* arg)
{
    if (FAILED(__super::Initialize(arg)))
        return E_FAIL;

    if (nullptr != arg)
    {
        MAPOBJECT_DESC* pDesc = (MAPOBJECT_DESC*)arg;
        if (pDesc->jExtraData.contains("MyMapType"))
        {
            m_eMSMapType = (MAP_TYPE)pDesc->jExtraData["MyMapType"].get<int>();
            m_iMSIndex = pDesc->jExtraData["MyIndex"].get<int>();
        }
    }
    else
    {
        m_eMSMapType = MAP_TYPE::ST01_UNDER;
        m_iMSIndex = 0;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

    static _uint namenum = 0;
    m_wstrName = L"MapSeal_" + to_wstring(namenum++);

    m_eObjType = OBJTYPE::TYPE_MAP;

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_vecObjPass.push_back(0);
    }

    //PHYSX_ACTOR_DESC actordesc;
    //actordesc.matWorld = m_pTransformCom->Get_WorldFloat4x4();
    //actordesc.pModel = m_pModelCom;

    //m_pGameInstance->PhysX_Load_Static_Actor_Auto(actordesc);

    return S_OK;
}

HRESULT CMapSeal::Ready_Components()
{
    _uint iLevel = (_uint)LEVEL::TOOL;

    CHECK_FAILED(Add_Component(iLevel, L"Prototype_Component_Shader_VTXAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
    CHECK_FAILED(Add_Component(iLevel, L"Prototype_Component_Model_MapSeal", Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

    if (m_pModelCom != nullptr)
    {
        m_vecObjPass.resize(m_pModelCom->Get_NumMeshes(), 0);
    }

    m_pModelCom->Set_Animation_CS(1, true);

    m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());


    return S_OK;
}

_int CMapSeal::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int CMapSeal::Update_Parallel(const _float fTimeDelta)
{

    __super::Update_Parallel(fTimeDelta);

    return 0;
}

_int CMapSeal::Update(const _float fDT)
{
    m_pModelCom->Play_Animation_CS(fDT);
    return __super::Update(fDT);
}

_int CMapSeal::Update_Late(const _float fDT)
{
    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    __super::Update_Late(fDT);

    return 0;
}

HRESULT CMapSeal::Render(const _float fDT)
{

    CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);
    CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

    _uint iObjID = Get_ObjectID();
    m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_OBJECT, "g_ObjectID", &iObjID, sizeof(_uint));

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom),E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

        CHECK_FAILED(m_pModelCom->Render(fDT, i), E_FAIL);
    }

    return S_OK;
}

ordered_json CMapSeal::Get_ExtraData()
{
    ordered_json j = __super::Get_ExtraData();
    j["MyMapType"] = (_int)m_eMSMapType;
    j["MyIndex"] = m_iMSIndex;
    return j;
}

CMapSeal* CMapSeal::Create(ID3D11Device* pD, ID3D11DeviceContext* pC)
{
    CMapSeal* pInstance = new CMapSeal(pD, pC);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* CMapSeal::Clone(void* arg)
{
    CMapSeal* pInstance = new CMapSeal(*this);
    if (FAILED(pInstance->Initialize(arg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CMapSeal::Free()
{
    __super::Free();
}

