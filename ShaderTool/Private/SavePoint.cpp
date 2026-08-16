#include "ShaderTool_Define.h"
#include "SavePoint.h"
#include "GameInstance.h"

SavePoint::SavePoint(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : MapObject(pD, pC)
{
}

SavePoint::SavePoint(const SavePoint& original)
    : MapObject(original)
{
}

HRESULT SavePoint::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT SavePoint::Initialize(void* arg)
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

    m_SavePointInfo.iIndex = m_iSPIndex;
    m_SavePointInfo.mapType = m_eSPMapType;
    m_SavePointInfo.spawnPosition = _float4(vPos.x, vPos.y, vPos.z, 1.f);
    m_SavePointInfo.wsName = m_wstrName;

    Set_Value();

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_vecObjPass.push_back(0);
    }

    Set_AllPass_VecObjPass(5);

    return S_OK;
}

_int SavePoint::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

HRESULT SavePoint::Ready_Components()
{
    CHECK_FAILED(Add_Component((_uint)LEVEL::STATIC, L"Prototype_Component_Shader_VTXAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
    CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_SavePoint", Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

    m_pModelCom->Set_Animation_CS(1);

    if (m_pModelCom != nullptr)
    {
        m_vecObjPass.resize(m_pModelCom->Get_NumMeshes(), 0);
    }

    m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());


    return S_OK;
}

_int SavePoint::Update_Parallel(const _float fTimeDelta)
{
    __super::Update_Parallel(fTimeDelta);

    return 0;
}

_int SavePoint::Update(const _float fDT)
{
    m_pModelCom->Play_Animation_CS(fDT);
    return __super::Update(fDT);
}

_int SavePoint::Update_Late(const _float fDT)
{

    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    Add_RenderGroup(RENDER_GROUP::SHADOW); 
    __super::Update_Late(fDT);
    if (m_pColliderCom)
    {
        m_pColliderCom->Update(m_pLightComponent->Get_Light_Matrix());
        _matrix temp = m_pTransformCom->Get_WorldMatrix();
        m_pColliderCom->Add_Debug_Render();
    }

    return 0;
}

HRESULT SavePoint::Render(const _float fDT)
{

    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
    CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint bitflag = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_SHININESS, 0, &bitflag), E_FAIL);
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

HRESULT SavePoint::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Begin(4, true), E_FAIL);

    for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(4), E_FAIL);

        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }
    return S_OK;
}

HRESULT SavePoint::Player_Teleport(const _float fDT)
{
    //if (m_SavePointInfo.targetLevel == LEVEL::END)
    //{
    //    MSG_BOX("목표 레벨이 설정되지 않았습니다!");
    //    return E_FAIL;
    //}

    //GameObject* pPlayer = m_pGameInstance->Get_Player();
    //if (nullptr == pPlayer)
    //{
    //    MSG_BOX("플레이어를 찾을 수 없습니다!");
    //    return E_FAIL;
    //}

    //m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING),
    //    Level_Load::Create(m_pDevice, m_pContext, m_SavePointInfo.targetLevel)
    //);

    //m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));

    //pPlayer->Set_State(DIRECTION::POSITION, m_SavePointInfo.spawnPosition);

    return S_OK;
}

ordered_json SavePoint::Get_ExtraData()
{
    ordered_json j;
    j["MyMapType"] = (_int)m_eSPMapType;
    j["MyIndex"] = m_iSPIndex;
    return j;
}

void SavePoint::Set_Value()
{
    //if (m_eSPMapType == MAP_TYPE::ST00_BASE)
    //{
    //    m_SavePointInfo.targetLevel = LEVEL::BASE;

    //    switch (m_iSPIndex)
    //    {
    //    case 0:
    //        m_SavePointInfo.spawnPosition = _float4(10.0f, 2.0f, 15.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"활동 거점";
    //        break;
    //    }
    //}

    //else if (m_eSPMapType == MAP_TYPE::ST01_UNDER)
    //{
    //    m_SavePointInfo.targetLevel = LEVEL::MAIN;

    //    switch (m_iSPIndex)
    //    {
    //    case 0:
    //        m_SavePointInfo.spawnPosition = _float4(15.0f, 3.0f, 10.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"자연 동굴";
    //        break;
    //    case 1:
    //        m_SavePointInfo.spawnPosition = _float4(35.0f, 3.0f, 25.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"자연 동굴 심부";
    //        break;
    //    case 2:
    //        m_SavePointInfo.spawnPosition = _float4(55.0f, 5.0f, 15.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"실외의 십자로";
    //        break;
    //    }
    //}

    //else if (m_eSPMapType == MAP_TYPE::ST02_CHURCH)
    //{
    //    m_SavePointInfo.targetLevel = LEVEL::CHURCH;

    //    switch (m_iSPIndex)
    //    {
    //    case 0:
    //        m_SavePointInfo.spawnPosition = _float4(20.0f, 1.0f, 30.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"C맵_입구";
    //        break;
    //    case 1:
    //        m_SavePointInfo.spawnPosition = _float4(40.0f, 3.0f, 20.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"C맵_중간";
    //        break;
    //    }
    //}

    //m_SavePointInfo.spawnRotation = _float4(0.0f, 0.0f, 0.0f, 1.0f);
}

SavePoint* SavePoint::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, LEVEL _level)
{
    SavePoint* pInstance = new SavePoint(pD, pC);
    if (FAILED(pInstance->Initialize_Prototype(_level)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* SavePoint::Clone(void* arg)
{
    SavePoint* pInstance = new SavePoint(*this);
    if (FAILED(pInstance->Initialize(arg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void SavePoint::Free()
{
    __super::Free();
}

