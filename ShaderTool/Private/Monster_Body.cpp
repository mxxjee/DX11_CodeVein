#include "ShaderTool_Define.h"
#include "Monster_Body.h"
#include "GameInstance.h"
#include "Buffer.h"
#include "ContainerObject.h"

ShaderTool::Monster_Body::Monster_Body()
{
}

ShaderTool::Monster_Body::Monster_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : ContainerObject{ pDevice, pContext }
{
}

ShaderTool::Monster_Body::Monster_Body(const Monster_Body& Prototype)
    : ContainerObject( Prototype )
{
}

const _float4x4* ShaderTool::Monster_Body::Get_SocketMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrixPtr(pBoneName);
}

HRESULT ShaderTool::Monster_Body::Initialize_Prototype()
{
    return S_OK;
}

HRESULT ShaderTool::Monster_Body::Initialize(void* pArg)
{
    static _uint namenum = 0;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Components(), E_FAIL);

    m_pModelCom->Play_Animation_CS(0.f);

    m_pTransformCom->Set_State(DIRECTION::POSITION, _float4(-155.f, 10.f, 100.f, 1.f));
    return S_OK;
}

_int ShaderTool::Monster_Body::Update_Priority(_float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Priority(fTimeDelta));

    m_pModelCom->Play_Animation_CS(fTimeDelta);

    return 0;
}

_int ShaderTool::Monster_Body::Update(_float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update(fTimeDelta));
    return 0;
}

_int ShaderTool::Monster_Body::Update_Parallel(const _float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Parallel(fTimeDelta));
    return 0;
}
_int ShaderTool::Monster_Body::Update_Late(_float fTimeDelta)
{
    CHECK_FUNCTION_FAIL(__super::Update_Late(fTimeDelta));
    m_pGameInstance->Add_RenderObject(RENDER_GROUP::NONBLEND, this); 
    return 0;
}

HRESULT ShaderTool::Monster_Body::Render(const _float fTimeDelta)
{
    if (m_pShaderCom == nullptr)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Begin(10), E_FAIL);

    //일단 여기서 구현해봐
   
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint bitFlag = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &bitFlag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitFlag), E_FAIL);

        m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitFlag, sizeof(_uint));
        _float2 UV = { -0.13, 0.2 };
        m_pShaderCom->Bind_RawValue_ByHandle(g_vEmissionUVPos, &UV, sizeof(_float2));

        m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(10), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }

    return S_OK;
}

HRESULT ShaderTool::Monster_Body::Ready_Components()
{
    CHECK_FAILED(Add_Shader(Proto_Com_Shader_VTXAnimMesh), E_FAIL);
    CHECK_FAILED(Add_Component(_UINT(LEVEL::LOGO), Proto_Model(L"SlaveDevil"), Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

    m_pModelCom->Set_Animation_CS(1, true);
    m_pModelCom->Play_Animation(0.2f);
    m_pModelCom->Set_Animation_CS(0, true);

    m_vecObjPass.clear();
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
        m_vecObjPass.push_back(0);

    return S_OK;
}

HRESULT ShaderTool::Monster_Body::Bind_ShaderResources()
{
    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

    return S_OK;
}

Monster_Body* ShaderTool::Monster_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
    Monster_Body* pInstance = new Monster_Body(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Monster_Body");
        Safe_Release(pInstance);
    }
    return pInstance;
}

GameObject* ShaderTool::Monster_Body::Clone(void* pArg)
{
    Monster_Body* pInstance = new Monster_Body(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Monster_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void ShaderTool::Monster_Body::Free()
{
    __super::Free();
}
