#include "Engine_Define.h"
#include "BlobShadow.h"

#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::BlobShadow::BlobShadow()
{
}

Engine::BlobShadow::BlobShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{
}

Engine::BlobShadow::BlobShadow(const BlobShadow& original)
    : Component(original)
    , m_fShadowRadius(original.m_fShadowRadius)
    , m_fShadowIntensity(original.m_fShadowIntensity)
    , m_fShadowSoftness(original.m_fShadowSoftness)
    , m_fYOffset(original.m_fYOffset)
{
}

Engine::BlobShadow::~BlobShadow()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::BlobShadow::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::BlobShadow::Initialize(void* arg)
{
    if (arg)
    {
        SHADOW_DESC* desc = CAST(SHADOW_DESC*)(arg);
        m_iLevel = desc->iLevel;
        m_fShadowIntensity = desc->fShadowIntensity;
        m_fShadowRadius = desc->fShadowRadius;
        m_fShadowSoftness = desc->fShadowSoftness;
        m_fYOffset = desc->fYOffset;
    }

    m_pShaderCom = CAST(Shader*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Shader_BlobShadow"));
    CHECK_NULLPTR(m_pShaderCom);

    m_pVIBufferCom = CAST(VIBuffer_Rect*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_VIBuffer_Rect"));
    CHECK_NULLPTR(m_pVIBufferCom);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::BlobShadow::Render(const _float fTimeDelta)
{
    // 컴포넌트를 가지고 있는 클래스의 Transform이 없으면 안 됨
    if (m_pOwnerTransform == nullptr)
        return E_FAIL;

    // 소유자 위치 가져오기
    _float4 vOwnerPos;
    XMStoreFloat4(&vOwnerPos, m_pOwnerTransform->Get_State(DIRECTION::POSITION));

    // Blob Shadow용 World Matrix 생성
    // XZ 평면에 납작하게 눕히고, 소유자 발 아래에 배치
    _float4x4 matBlobWorld;
    XMMATRIX matScale = XMMatrixScaling(m_fShadowRadius * 2.f, m_fShadowRadius * 2.f, 1.f);
    XMMATRIX matRotation = XMMatrixRotationX(XMConvertToRadians(90.f));
    XMMATRIX matTranslation = XMMatrixTranslation(vOwnerPos.x, vOwnerPos.y + m_fYOffset, vOwnerPos.z);

    XMStoreFloat4x4(&matBlobWorld, matScale * matRotation * matTranslation);

    // 쉐이더 바인딩
    GameInstance* pGameInstance = GameInstance::GetInstance();

    CHECK_FAILED(m_pShaderCom->Bind_Matrix_FullSlot(BUFFER_OBJECT, "g_WorldMatrix", matBlobWorld), E_FAIL);
    CHECK_FAILED(pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ViewMatrix", D3DTS_VIEW), E_FAIL);
    CHECK_FAILED(pGameInstance->Bind_PipeLineMatrix(m_pShaderCom, "g_ProjMatrix", D3DTS_PROJ), E_FAIL);

    // Blob Shadow 파라미터 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_BLOBSHADOW, "g_ShadowRadius", &m_fShadowRadius, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_BLOBSHADOW, "g_ShadowIntensity", &m_fShadowIntensity, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_BLOBSHADOW, "g_ShadowSoftness", &m_fShadowSoftness, sizeof(_float)), E_FAIL);

    // 렌더링
    CHECK_FAILED(m_pShaderCom->Begin(0), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Resources(0), E_FAIL);
    CHECK_FAILED(m_pVIBufferCom->Bind_Resource(), E_FAIL);
    CHECK_FAILED(m_pVIBufferCom->Render(fTimeDelta), E_FAIL);

    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
BlobShadow* Engine::BlobShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    BlobShadow* pInstance = new BlobShadow(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(), L"BlobShadow 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::BlobShadow::Clone(void* arg)
{
    BlobShadow* pInstance = new BlobShadow(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"BlobShadow 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::BlobShadow::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    // m_pOwnerTransform은 AddRef 안했으므로 Release 안함
}
/******************************************************* 객체 반환 함수 *******************************************************/
