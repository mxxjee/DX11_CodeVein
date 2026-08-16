#include "MT_Defines.h"
#include "StaticObj.h"
#include "GameInstance.h"
#include "Level_Main.h"

CStaticObj::CStaticObj(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : MapObject(pD, pC)
{
}

CStaticObj::CStaticObj(const CStaticObj& original)
    : MapObject(original)
{
}

HRESULT CStaticObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStaticObj::Initialize(void* arg)
{
    static _uint namenum = 0;

    MAPOBJECT_DESC* pDesc = nullptr;

    if (nullptr != arg)
    {
        pDesc = (MAPOBJECT_DESC*)arg;

        if (pDesc->wstrModelName != L"")
            m_pProtoname = pDesc->wstrModelName;
        else
            m_pProtoname = pDesc->prototypename;
    }

    CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"StaticObj_" + to_wstring(namenum++);
    if (FAILED(__super::Initialize(arg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eObjType = OBJTYPE::TYPE_MAP;

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_vecObjPass.push_back(0);
    }

    PHYSX_ACTOR_DESC actordesc;
    actordesc.matWorld = m_pTransformCom->Get_WorldFloat4x4();
    actordesc.pModel = m_pModelCom;

    m_pPhysXActor = m_pGameInstance->PhysX_Load_Static_Actor_Auto(actordesc);
    Set_AllPass_VecObjPass(7);

    if (pDesc != nullptr && pDesc->jExtraData.contains("HasPointLight") && pDesc->jExtraData["HasPointLight"] == true)
    {
        auto& lightJson = pDesc->jExtraData["PointLight"];
        LIGHT_DESC loadDesc;

        loadDesc.eType = (LIGHT)lightJson["eType"].get<int>();
        loadDesc.vDiffuse = { lightJson["Diffuse"][0].get<float>(),  lightJson["Diffuse"][1].get<float>(),  lightJson["Diffuse"][2].get<float>(),  lightJson["Diffuse"][3].get<float>() };
        loadDesc.vAmbient = { lightJson["Ambient"][0].get<float>(),  lightJson["Ambient"][1].get<float>(),  lightJson["Ambient"][2].get<float>(),  lightJson["Ambient"][3].get<float>() };
        loadDesc.vSpecular = { lightJson["Specular"][0].get<float>(), lightJson["Specular"][1].get<float>(), lightJson["Specular"][2].get<float>(), lightJson["Specular"][3].get<float>() };
        loadDesc.vDirection = { lightJson["Direction"][0].get<float>(), lightJson["Direction"][1].get<float>(), lightJson["Direction"][2].get<float>(), lightJson["Direction"][3].get<float>() };

        if (lightJson.contains("Position")) {
            loadDesc.vPosition = {
                lightJson["Position"][0].get<float>(),
                lightJson["Position"][1].get<float>(),
                lightJson["Position"][2].get<float>(),
                lightJson["Position"][3].get<float>()
            };
        }
        else {
            XMStoreFloat4(&loadDesc.vPosition, m_pTransformCom->Get_State(DIRECTION::POSITION));
        }

        loadDesc.fRange = lightJson["Range"].get<float>();

        _uint iNewLightIndex = m_pGameInstance->Get_LightCnt();

        if (SUCCEEDED(Add_Component(_UINT(LEVEL::TOOL), L"Proto_Component_Point_Light",
            TEXT("Com_Light" + to_wstring(iNewLightIndex)),
            reinterpret_cast<Component**>(&m_pLightComponent),
            &loadDesc)))
        {
            _vector vLoadedPos = XMLoadFloat4(&loadDesc.vPosition);
            m_pLightComponent->Update(0, vLoadedPos);

            Bounding_Sphere::BOUNDSPHERE_DESC boundingdesc;
            boundingdesc.fRadius = 1.f;
            boundingdesc.vCenter = _float3(0.f, 0.f, 0.f);
            Add_Collider(COLLISION_GROUP::INTERACTION, COLLIDER::SPHERE, &m_pColliderCom, &boundingdesc);
        }
    }

    return S_OK;
}

HRESULT CStaticObj::Ready_Components()
{
    _uint iLevel = (_uint)LEVEL::TOOL;

    CHECK_FAILED(Add_Component(iLevel, L"Prototype_Component_Shader_VTXMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
    CHECK_FAILED(Add_Component(iLevel, m_pProtoname, Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);
    
    Add_Object_To_Octree(RENDER_GROUP::NONBLEND);

    return S_OK;
}

_int CStaticObj::Update(const _float fDT)
{
    m_fAccTime += fDT;

    extern GameObject* g_pSelectedObject;
    if (this == g_pSelectedObject)
    {
        if (m_bWasSelected == false)
        {
            m_bOriginalVisible = this->Is_Visible();
            m_bWasSelected = true;
        }

        if ((int)(m_fAccTime * 2.0f) % 2 == 0)
        {
            this->Set_Visible(false);
        }
        else
        {
            this->Set_Visible(true);
        }
    }
    else
    {
        if (m_bWasSelected == true)
        {
            this->Set_Visible(m_bOriginalVisible);
            m_bWasSelected = false;
        }
    }

    return __super::Update(fDT);
}

_int CStaticObj::Update_Late(const _float fDT)
{
    m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW_BAKE, this);
    __super::Update_Late(fDT);

    m_pGameInstance->Add_RenderObject(RENDER_GROUP::SHADOW_BAKE, this);
    if (m_pColliderCom)
    {
        m_pColliderCom->Update(m_pLightComponent->Get_Light_Matrix());
        _matrix temp = m_pTransformCom->Get_WorldMatrix();
        m_pColliderCom->Add_Debug_Render();
    }

    return 0;
}

//푸쉬가안돼!!!!!!!!!!!!!!
//푸쉬가안돼~~~~~~~~~~
HRESULT CStaticObj::Render(const _float fDT)
{
    MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);
    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

    // m_pModelCom->Add_DebugRender();
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint test = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &test), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &test), E_FAIL);

        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &test), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &test), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &test), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS, 0, &test), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0, &test), E_FAIL); 
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0, &test), E_FAIL); 

        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_PBRTexture", i, aiTextureType_METALNESS, 0, &test), E_FAIL); 
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0, &test), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_OpacityTexture", i, aiTextureType_OPACITY, 0, &test), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS, 0, &test), E_FAIL);
        //// 1 g_bitFlag
        m_pShaderCom->Bind_RawValue_FullSlot(1, "g_bitFlag", &test, sizeof(_uint));
        m_pShaderCom->Bind_RawValue_FullSlot(1, "g_fTime", &m_fAccTime, sizeof(_float));
        //여기서 다 구분하고 던져줘야되는데
        //여기서 던져준다
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);

        //CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

        /* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
        CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

        // CHECK_FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i), E_FAIL);

        /* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
        CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

        /* 그린다 */
        CHECK_FAILED(m_pModelCom->Render(fDT, i), E_FAIL);
    }

    return S_OK;
}

HRESULT CStaticObj::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
    /*m_pShaderCom->Bind_RawValue_FullSlot(11, "g_ShadowCascadeNum", &iCascadeNum, sizeof(_int));
    m_pShaderCom->Bind_Matrix("g_ShadowCascade_ViewProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEWPROJ));*/

    CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4()), E_FAIL);
    //CHECK_FAILED(m_pTransformCom->Bind_ShaderResource_FullSlot(m_pShaderCom, BUFFER_OBJECT, "g_WorldMatrix"), E_FAIL);

    //CHECK_FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_VIEW)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_LightMatrix(D3DTRANSFORM::D3DTS_PROJ)), E_FAIL);
    //cout << m_iObjectID << endl;
    //CHECK_FAILED(Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        /*if (FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i)))
            return E_FAIL;*/

        if (FAILED(m_pShaderCom->Begin(5, true)))
            return E_FAIL;

        CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL); //패스번호맞춰서? 

        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }
    return S_OK;
}

CStaticObj* CStaticObj::Create(ID3D11Device* pD, ID3D11DeviceContext* pC)
{
    CStaticObj* pInstance = new CStaticObj(pD, pC);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* CStaticObj::Clone(void* arg)
{
    CStaticObj* pInstance = new CStaticObj(*this);
    if (FAILED(pInstance->Initialize(arg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CStaticObj::Free()
{
    __super::Free();

    m_pGameInstance->PhysX_Remove_Actor(m_pPhysXActor);
    m_pGameInstance->Remove_StaticObject(this);
}

