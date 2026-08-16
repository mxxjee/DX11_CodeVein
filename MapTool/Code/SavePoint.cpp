#include "MT_Defines.h"
#include "SavePoint.h"
#include "GameInstance.h"

CSavePoint::CSavePoint(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : MapObject(pD, pC)
{
}

CSavePoint::CSavePoint(const CSavePoint& original)
    : MapObject(original)
{
}

HRESULT CSavePoint::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSavePoint::Initialize(void* arg)
{
    if (FAILED(__super::Initialize(arg)))
        return E_FAIL;

    if (nullptr != arg)
    {
        MAPOBJECT_DESC* pDesc = (MAPOBJECT_DESC*)arg;
        if (pDesc->jExtraData.contains("MyMapType"))
        {
            m_eSPMapType = (MAP_TYPE)pDesc->jExtraData["MyMapType"].get<int>();
            m_iSPIndex = pDesc->jExtraData["MyIndex"].get<int>();
        }
    }
    else
    {
        m_eSPMapType = MAP_TYPE::ST00_BASE;
        m_iSPIndex = 0;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

    static _uint namenum = 0;
    m_wstrName = L"SavePoint_" + to_wstring(namenum++);

    m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);

    m_eObjType = OBJTYPE::TYPE_MAP;

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_vecObjPass.push_back(0);
    }

    Set_AllPass_VecObjPass(3);

    return S_OK;
}

_int CSavePoint::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

HRESULT CSavePoint::Ready_Components()
{
    _uint iLevel = (_uint)LEVEL::TOOL;

    CHECK_FAILED(Add_Component(iLevel, L"Prototype_Component_Shader_VTXAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
    CHECK_FAILED(Add_Component(iLevel, L"Prototype_Component_Model_SavePoint", Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);
    
    m_pModelCom->Set_Animation_CS(1);

    if (m_pModelCom != nullptr)
    {
        m_vecObjPass.resize(m_pModelCom->Get_NumMeshes(), 0);
    }

    m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());


    return S_OK;
}

_int CSavePoint::Update_Parallel(const _float fTimeDelta)
{

    __super::Update_Parallel(fTimeDelta);

    return 0;
}

_int CSavePoint::Update(const _float fDT)
{
    m_pModelCom->Play_Animation_CS(fDT);
    return __super::Update(fDT);
}

_int CSavePoint::Update_Late(const _float fDT)
{
    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    __super::Update_Late(fDT);

    return 0;
}

HRESULT CSavePoint::Render(const _float fDT)
{

    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
    CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

    _uint iObjID = Get_ObjectID();
    m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_OBJECT, "g_ObjectID", &iObjID, sizeof(_uint));

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint bitflag = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitflag), E_FAIL);
        //AO나중에 처리
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);
        m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

        CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fDT, i), E_FAIL);
    }

    return S_OK;
}

ordered_json CSavePoint::Get_ExtraData()
{
    ordered_json j = __super::Get_ExtraData();
    j["MyMapType"] = (_int)m_eSPMapType;
    j["MyIndex"] = m_iSPIndex;
    return j;
}

CSavePoint* CSavePoint::Create(ID3D11Device* pD, ID3D11DeviceContext* pC)
{
    CSavePoint* pInstance = new CSavePoint(pD, pC);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* CSavePoint::Clone(void* arg)
{
    CSavePoint* pInstance = new CSavePoint(*this);
    if (FAILED(pInstance->Initialize(arg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CSavePoint::Free()
{
    __super::Free();
}

