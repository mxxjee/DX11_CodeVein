#include "Engine_Define.h"
#include "VIBuffer_Particle_Point.h"

#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::VIBuffer_Particle_Point::VIBuffer_Particle_Point()
{
}

Engine::VIBuffer_Particle_Point::VIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer_Particle(pDevice, pContext)
{
}

Engine::VIBuffer_Particle_Point::VIBuffer_Particle_Point(const VIBuffer_Particle_Point& original)
    : VIBuffer_Particle(original)
{
}

Engine::VIBuffer_Particle_Point::~VIBuffer_Particle_Point()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Particle_Point::Initialize_Prototype(const PARTICLE_DESC* _desc)
{
    m_iVertexStride = sizeof(VTXPOS);	// 한 정점의 크기(바이트)
    m_iNumVertices = 1;		// 정점의 개수 (사각형이니까 4개)
    m_iIndexStride = 0;		// 인덱스 하나의 크기(바이트)

    m_iNumIndices = 0;		// 인덱스 개수
    m_iNumVertexBuffers = 2; // 버텍스 버퍼의 개수(메인 버텍스 버퍼와 버텍스 인스턴스 버퍼)
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;


    const PARTICLE_POINT_DESC* particleDesc = CAST(const PARTICLE_POINT_DESC*)(_desc);

    m_bIsLoop = particleDesc->IsLoop;
    m_iNumInstance = particleDesc->iNumInstance;
    m_iInstanceVertexStride = sizeof(VTXPARTICLE);
    m_iIndexCountPerInstance = m_iNumIndices;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// 버퍼 전체의 크기 = 정점 하나의 바이트 * 정점 갯수
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;			// 정점의 CPU/GPU접근방식
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 바인딩 방식(정점으로)
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;	// 정점 하나의 크기

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices]{};

    m_pVertexPositions = new _float3[m_iNumVertices]{};

    m_pVertexPositions[0] = pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);

    D3D11_SUBRESOURCE_DATA VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    MSG_FAIL(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB), L"버텍스 버퍼 생성에 실패했습니다.", L"버퍼 생성 실패.", E_FAIL);

    Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INSTANCE_BUFFER
    m_InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;  // 인스턴스 버퍼의 크기 = 인스턴스 개수 * 인스턴스 하나의 크기
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_InstanceBufferDesc.MiscFlags = 0;
    m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

    m_pInstanceVertices = new VTXPARTICLE[m_iNumInstance]{};

    m_pSpeeds = new _float[m_iNumInstance]{0.f};

    //m_pVelocities = new _float3[m_iNumInstance];
    //ZeroMemory(m_pVelocities, sizeof(_float3) * m_iNumInstance);

    m_vPivot = particleDesc->vPivot;
    m_fGravity = particleDesc->fGravity;
    m_fAlpha = particleDesc->fAlpha;

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        _float scale = m_pGameInstance->RandomValue(particleDesc->vSize.x, particleDesc->vSize.y);  // 대입한 값의 x~y 사이의 랜덤값으로 scale결정
        m_pSpeeds[i] = m_pGameInstance->RandomValue(particleDesc->vSpeed.x, particleDesc->vSpeed.y); // 대입한 값의 x~y 사이의 랜덤값으로 speed결정

        m_pInstanceVertices[i].vRight = _float4(scale, 0.f, 0.f, 0.f);
        m_pInstanceVertices[i].vUp    = _float4(0.f, scale, 0.f, 0.f);
        m_pInstanceVertices[i].vLook  = _float4(0.f, 0.f, scale, 0.f);
        m_pInstanceVertices[i].vTranslation = _float4(
            m_pGameInstance->RandomValue(particleDesc->vCenter.x - particleDesc->vRange.x * 0.5f, particleDesc->vCenter.x + particleDesc->vRange.x * 0.5f)
            , m_pGameInstance->RandomValue(particleDesc->vCenter.y - particleDesc->vRange.y * 0.5f, particleDesc->vCenter.y + particleDesc->vRange.y * 0.5f)
            , m_pGameInstance->RandomValue(particleDesc->vCenter.z - particleDesc->vRange.z * 0.5f, particleDesc->vCenter.z + particleDesc->vRange.z * 0.5f)
            , 1.f);

        m_pInstanceVertices[i].vLifeTime = _float2(0.f, m_pGameInstance->RandomValue(particleDesc->vLifeTime.x, particleDesc->vLifeTime.y));

        _float3 vDirection = _float3(m_pInstanceVertices[i].vTranslation.x - m_vPivot.x,
            m_pInstanceVertices[i].vTranslation.y - m_vPivot.y,
            m_pInstanceVertices[i].vTranslation.z - m_vPivot.z);

        XMStoreFloat3(&vDirection, XMVector3Normalize(XMLoadFloat3(&vDirection)));

        //m_pVelocities[i] = _float3(vDirection.x * m_pSpeeds[i],
        //    vDirection.y * m_pSpeeds[i] + 0.05f,
        //    vDirection.z * m_pSpeeds[i]);
    }


#pragma endregion


    //if(m_bIsAnimated)
    //    *m_pSkinnedVertexPositions = *m_pVertexPositions;

    return S_OK;
}

HRESULT Engine::VIBuffer_Particle_Point::Initialize(void* arg)
{
    D3D11_SUBRESOURCE_DATA instanceInitialData{};
    instanceInitialData.pSysMem = m_pInstanceVertices;

    CHECK_FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &instanceInitialData, &m_pVBInstance), E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더타겟 세팅 함수 ////////////////////////////////////////////////////////
HRESULT Engine::VIBuffer_Particle_Point::Bind_Resource()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
        m_pVBInstance
    };

    _uint		 iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride,
    };

    _uint		 iOffsets[] = {
        0,
        0
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);    
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

    return S_OK;
}

HRESULT Engine::VIBuffer_Particle_Point::Bind_EffectResources(Shader* pShader)
{
    //CHECK_FAILED(pShader->Bind_RawValue_FullSlot(BUFFER_EFFECT, "g_vScaleUV", &m_vScaleUV, sizeof(_float2)), E_FAIL);
    //CHECK_FAILED(pShader->Bind_RawValue_FullSlot(BUFFER_EFFECT, "g_vOffsetUV", &m_vOffsetUV, sizeof(_float2)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_FullSlot(BUFFER_EFFECT, "g_fTransparency", &m_fTransparency, sizeof(_float)), E_FAIL);
    CHECK_FAILED(pShader->Bind_RawValue_FullSlot(BUFFER_EFFECT, "g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

    return S_OK;
}

HRESULT Engine::VIBuffer_Particle_Point::Render(const _float fTimeDelta)
{
    m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

    return S_OK;
}
/******************************************************* 렌더타겟 세팅 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
VIBuffer_Particle_Point* Engine::VIBuffer_Particle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const PARTICLE_DESC* _desc)
{
    VIBuffer_Particle_Point* pInstance = new VIBuffer_Particle_Point(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_desc), L"VIBuffer_Particle_Point 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::VIBuffer_Particle_Point::Clone(void* arg)
{
    VIBuffer_Particle_Point* pInstance = new VIBuffer_Particle_Point(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"VIBuffer_Particle_Point 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::VIBuffer_Particle_Point::Free()
{
    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

